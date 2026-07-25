#include "systemAudioSource.h"
#include <QDebug>
#include <QMetaMethod>
#include <QtEndian>

systemAudioSource::systemAudioSource(QObject *parent)
    : QObject{parent}
{
    connect(this, &systemAudioSource::openSourceSig, this,
            [ = ](QAudioFormat format,
                  QAudioDevice info, int bufferSize) {
        if (m_audioSource != nullptr) return;

        m_audioSource = new QAudioSource(info, format, this);

        connect(m_audioSource, &QAudioSource::stateChanged,
                this, [ = ](QAudio::State state) {
            emit audioSourceStatus(state);
        });
        m_audioSource->setVolume(1);

        // int formatSize = 2;
        // switch (m_audioSource->format().sampleFormat()) {
        // case QAudioFormat::UInt8:
        //     formatSize = 1;
        //     break;
        // case QAudioFormat::Int16:
        //     formatSize = 2;
        //     break;
        // case QAudioFormat::Int32:
        //     formatSize = 4;
        //     break;
        // case QAudioFormat::Float:
        //     formatSize = 4;
        //     break;
        // default:
        //     break;
        // }
        // m_audioSource->setBufferSize(1024 * format.channelCount() *
        // formatSize);

        m_audioSource->setBufferSize(bufferSize);

        qInfo() << "QAudioSource info:" <<
            info.description() << m_audioSource->volume() <<
            m_audioSource->format().sampleFormat() <<
            m_audioSource->format().channelCount() <<
            m_audioSource->bufferSize();

        m_ioSource = m_audioSource->start();
        connect(m_ioSource, &QIODevice::readyRead,
                this, &systemAudioSource::readAudioData,
                Qt::ConnectionType(Qt::UniqueConnection | Qt::AutoConnection));
    });

    connect(this, &systemAudioSource::colseSourceSig, this, [ = ]() {
        if (m_audioSource) {
            m_audioSource->stop();
            delete m_audioSource;
            m_ioSource = nullptr;
            m_audioSource = nullptr;
        }
    });

    connect(this, &systemAudioSource::setSourceVolumeSig,
            this, [ = ](qreal volume) {
        if (m_audioSource) {
            m_audioSource->setVolume(volume);
        }
    });

    connect(this, &systemAudioSource::susPendSig,
            this, [ = ]() {
        if (m_audioSource) {
            if (m_audioSource->state() == QAudio::ActiveState) {
                m_audioSource->suspend();
            }
        }
    });

    connect(this, &systemAudioSource::resumeSig,
            this, [ = ]() {
        if (m_audioSource) {
            if (m_audioSource->state() == QAudio::SuspendedState) {
                m_audioSource->resume();
            }
        }
    });
}

systemAudioSource::~systemAudioSource()
{}

void systemAudioSource::openSource(QAudioFormat format,
                                   QAudioDevice info, int bufferSize)
{
    // QAudioFormat m_format;
    // m_format.setSampleRate(48000);
    // m_format.setSampleFormat(QAudioFormat::Int16);
    // m_format.setChannelConfig(QAudioFormat::ChannelConfigStereo);
    // m_format.setChannelCount(2);

    // QAudioDevice m_info(QMediaDevices::audioInputs().at(0));
    // if (!m_info.isFormatSupported(format))return;

    // // 1. 获取所有音频输入设备
    // const QList<QAudioDevice> inputDevices = QMediaDevices::audioInputs();
    // QAudioDevice stereoMixDevice;
    // // 2. 遍历查找立体声混音设备
    // for (const QAudioDevice& device : inputDevices) {
    //     // 通过设备描述进行匹配，可根据实际情况调整关键词
    //     if (device.description().contains("立体声混音", Qt::CaseInsensitive) ||
    //         device.description().contains("Stereo Mix", Qt::CaseInsensitive))
    // {
    //         stereoMixDevice = device;
    //         qDebug() << "找到立体声混音设备:" << device.description();
    //         break;
    //     }
    // }
    // if (stereoMixDevice.isNull()) return;

    emit openSourceSig(format, info, bufferSize);
}

void systemAudioSource::colseSource()
{
    emit colseSourceSig();
}

void systemAudioSource::setSourceVolume(qreal volume)
{
    emit setSourceVolumeSig(volume);
}

void systemAudioSource::susPend()
{
    emit susPendSig();
}

void systemAudioSource::resume()
{
    emit resumeSig();
}

void systemAudioSource::readAudioData()
{
    if (!m_ioSource) return;

    m_data = m_ioSource->readAll();
    emit readData(m_data);
}
