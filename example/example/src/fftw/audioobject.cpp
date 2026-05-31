#include "audioobject.h"
#include "fftworkthread.h"
#include <QDebug>
#include <QMetaMethod>
#include <QtEndian>

audioObject::audioObject(QObject *parent)
    : QObject{parent}
{
    FFTWorkThread *m_fftWorkThread = new FFTWorkThread;
    QThread *thread = new QThread(this);

    m_fftWorkThread->moveToThread(thread);

    connectInit();

    connect(m_fftWorkThread,
            SIGNAL(fftwData(QVector<double>,QList<QVector<double> >)),
            this,
            SIGNAL(fftwData(QVector<double>,QList<QVector<double> >)));

    connect(this,
            SIGNAL(startFFT(const QAudioFormat,const int,const int)),
            m_fftWorkThread,
            SLOT(startFFT(const QAudioFormat,const int,const int)));
    connect(this,
            SIGNAL(addStreamFFT(QByteArray)),
            m_fftWorkThread,
            SLOT(addStreamFFT(QByteArray)));
    connect(this, &audioObject::stoptFFT,
            m_fftWorkThread, &FFTWorkThread::stoptFFT);

    thread->start();
}

audioObject::~audioObject()
{
    thread->quit();
    thread->wait();
    delete thread;
    delete m_fftWorkThread;
}

void audioObject::openSource(QAudioFormat format,
                             QAudioDevice info,
                             int          buffCount)
{
    emit openSourceSig(format, info, buffCount);
}

void audioObject::colseSource()
{
    emit colseSourceSig();
}

void audioObject::setSourceVolume(qreal volume)
{
    emit setSourceVolumeSig(volume);
}

void audioObject::openSink(QAudioFormat format, QAudioDevice info)
{
    emit openSinkSig(format, info);
}

void audioObject::colseSink()
{
    emit colseSinkSig();
}

void audioObject::setSinkVolume(qreal volume)
{
    emit setSinkVolumeSig(volume);
}

void audioObject::writeAudioData(QByteArray rdata, QAudioFormat format)
{
    if (m_audioSinkPCM) {
        delete m_audioSinkPCM;
    }

    if (buffer) {
        delete buffer;
    }

    m_data = rdata;
    m_audioSinkPCM = new QAudioSink(format, this);
    buffer = new QBuffer(&m_data, this);

    if (false == buffer->open(QIODevice::ReadOnly)) {
        delete m_audioSinkPCM;
        delete buffer;
        m_audioSinkPCM = nullptr;
        buffer = nullptr;
        m_data.clear();
        return;
    }

    m_audioSinkPCM->start(buffer);

    connect(m_audioSinkPCM, &QAudioSink::stateChanged, this,
            [ = ](QAudio::State state) {
        if (state == QAudio::IdleState) {
            m_audioSinkPCM->stop();
            delete m_audioSinkPCM;
            delete buffer;
            m_audioSinkPCM = nullptr;
            buffer = nullptr;
            m_data.clear();
        }
    });
}

void audioObject::readAudioData()
{
    if (!m_ioSource) return;

    QByteArray rdata = m_ioSource->readAll();
    emit readData(rdata);
    emit readData_private(rdata);

    emit addStreamFFT(rdata);
}

void audioObject::connectInit()
{
    // QMetaMethod signalMethod =
    // QMetaMethod::fromSignal(&audioObject::openSourceSig);
    // bool isConnected = this->isSignalConnected(signalMethod);
    // disconnect(this, &audioObject::openSourceSig, this, nullptr);
    connect(this, &audioObject::openSourceSig, this,
            [ = ](QAudioFormat format,
                  QAudioDevice info,
                  int buffCount) {
        if (m_audioSource != nullptr) return;

        m_audioSource = new QAudioSource(info, format, this);

        // connect(m_audioSource, &QAudioSource::stateChanged, this,
        //         [ = ](QAudio::State state) {
        // });

        emit startFFT((const QAudioFormat)m_audioSource->format(),
                      m_windowSize,
                      m_overlap);

        int channalCount = m_audioSource->format().channelCount();
        int bufferSize = m_count * channalCount;

        switch (m_audioSource->format().sampleFormat()) {
        case QAudioFormat::UInt8:
            bufferSize = m_count * channalCount;
            break;

        case QAudioFormat::Int16:
            bufferSize = m_count * 2 * channalCount;
            break;

        case QAudioFormat::Int32:
            bufferSize = m_count * 4 * channalCount;
            break;

        case QAudioFormat::Float:
            bufferSize = m_count * 4 * channalCount;
            break;

        default:
            break;
        }

        m_audioSource->setBufferSize(bufferSize * buffCount);

        qDebug() << "QAudioSource info:" <<
            m_audioSource->format().sampleFormat() <<
            m_audioSource->format().channelCount() <<
            m_audioSource->bufferSize();

        m_ioSource = m_audioSource->start();
        connect(m_ioSource, &QIODevice::readyRead,
                this, &audioObject::readAudioData,
                Qt::ConnectionType(Qt::UniqueConnection | Qt::AutoConnection));

        emit emit audioSourceStatus(true);
    });

    connect(this, &audioObject::colseSourceSig, this, [ = ]() {
        if (m_audioSource) {
            m_audioSource->stop();
            delete m_audioSource;
            m_ioSource = nullptr;
            m_audioSource = nullptr;

            emit stoptFFT();
            emit audioSourceStatus(false);
        }
    });

    connect(this, &audioObject::setSourceVolumeSig, this, [ = ](qreal volume) {
        if (m_audioSource) {
            m_audioSource->setVolume(volume);
        }
    });

    connect(this, &audioObject::openSinkSig, this,
            [ = ](QAudioFormat format, QAudioDevice info) {
        if (m_audioSink != nullptr) return;

        m_audioSink = new QAudioSink(info, format, this);

        // connect(m_audioSink, &QAudioSink::stateChanged, this,
        //         [ = ](QAudio::State) {});

        m_ioSink = m_audioSink->start();
        disconnect(this, &audioObject::readData_private, this, nullptr);
        connect(this, &audioObject::readData_private, this,
                [ = ](QByteArray& rdata) {
            if (m_ioSink) m_ioSink->write(rdata);
        });
    });

    connect(this, &audioObject::colseSinkSig, this, [ = ]() {
        if (m_audioSink) {
            m_audioSink->stop();
            delete m_audioSink;
            m_audioSink = nullptr;
            m_ioSink = nullptr;
        }
    });

    connect(this, &audioObject::setSinkVolumeSig,
            this,
            [ = ](qreal volume) {
        if (m_audioSink) {
            m_audioSink->setVolume(volume);
        }
    });
}
