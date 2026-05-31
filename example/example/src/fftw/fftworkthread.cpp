#include "fftworkthread.h"
#include "fftw3object.h"
#include <QDebug>
#include <QtEndian>

FFTWorkThread::FFTWorkThread(QObject *parent)
    : QObject{parent}
{
    m_fftw3obj = new fftw3Object(this);
}

FFTWorkThread::~FFTWorkThread()
{}

void FFTWorkThread::startFFT(const QAudioFormat format,
                             const int          windowSize,
                             const int          overlap)
{
    m_format = format;
    m_windowSize = windowSize;
    m_overlap = overlap;
    m_count = m_windowSize - m_overlap;

    m_fftw3obj->fftStreamStart(m_windowSize, m_overlap);
}

void FFTWorkThread::addStreamFFT(QByteArray data)
{
    QVector<double> dsdata;
    QList<QVector<double> > fftdata;
    QList<QVector<double> > radianfftdata;

    m_rdata.append(data);

    int channalCount = m_format.channelCount();
    QAudioFormat::SampleFormat sf = m_format.sampleFormat();
    int step = m_count * channalCount;

    switch (sf) {
    case QAudioFormat::UInt8:
        step = m_count * channalCount;
        break;

    case QAudioFormat::Int16:
        step = m_count * 2 * channalCount;
        break;

    case QAudioFormat::Int32:
        step = m_count * 4 * channalCount;
        break;

    case QAudioFormat::Float:
        step = m_count * 4 * channalCount;
        break;

    default:
        break;
    }

    QVector<double> tmpData;
    QVector<double> _fftdata;
    QVector<double> _radiandata;

    while (m_rdata.size() >= step) {
        tmpData.clear();

        switch (sf) {
        default:
        case QAudioFormat::UInt8: {
            quint8 *p = (quint8 *)m_rdata.data();

            for (int i = 0; i < m_count * channalCount; i += channalCount) {
                double v = p[i] / 127.0;
                tmpData.append(v - 1);
            }
            break;
        }

        case QAudioFormat::Int16: {
            qint16 *p = (qint16 *)m_rdata.data();

            for (int i = 0; i < m_count * channalCount; i += channalCount) {
                double v = p[i] / 32767.0;
                tmpData.append(v);
            }
            break;
        }

        case QAudioFormat::Int32: {
            qint32 *p = (qint32 *)m_rdata.data();

            for (int i = 0; i < m_count * channalCount; i += channalCount) {
                double v = p[i] / ((int)0x7FFFFFFF * 1.0);
                tmpData.append(v);
            }
            break;
        }

        case QAudioFormat::Float: {
            float *p = (float *)m_rdata.data();

            for (int i = 0; i < m_count * channalCount; i += channalCount) {
                double v = p[i];
                tmpData.append(v);
            }
            break;
        }
        }

        m_fftw3obj->fftAddStream(tmpData, _fftdata, _radiandata);

        dsdata.append(tmpData);
        fftdata.append(_fftdata);
        radianfftdata.append(_radiandata);

        m_rdata.remove(0, step);
    }

    if (fftdata.size() > 1) {
        qDebug() << fftdata.size();
    }

    emit fftwData(dsdata, fftdata, radianfftdata);
}

void FFTWorkThread::stoptFFT()
{
    m_fftw3obj->fftStreamSop();
    m_rdata.clear();
}
