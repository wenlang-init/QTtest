#ifndef AUDIOOBJECT_H
#define AUDIOOBJECT_H

#include <QObject>
#include <QAudioSink>
#include <QAudioSource>
#include <QAudioDevice>
#include <QThread>

class FFTWorkThread;
class audioObject : public QObject {
    Q_OBJECT

public:

    explicit audioObject(QObject *parent = nullptr);
    ~audioObject();
    void openSource(QAudioFormat format,
                    QAudioDevice info,
                    int          buffCount = 1);
    void colseSource();

    // 0~1
    void setSourceVolume(qreal volume);
    void openSink(QAudioFormat format,
                  QAudioDevice info);
    void colseSink();
    void setSinkVolume(qreal volume);

    int  getFFTPointCount() {
        return m_windowSize;
    }

protected slots:

    void readAudioData();

private:

    void connectInit();

signals:

    void audioSourceStatus(bool isOpen);

    void openSourceSig(QAudioFormat format,
                       QAudioDevice info,
                       int          buffCount);
    void colseSourceSig();

    void setSourceVolumeSig(qreal volume);
    void openSinkSig(QAudioFormat format,
                     QAudioDevice info);
    void colseSinkSig();
    void setSinkVolumeSig(qreal volume);

    void readData(QByteArray data);
    void readData_private(QByteArray& data);

    void fftwData(QVector<double>        dsdata,
                  QList<QVector<double> >fftdata);

    void startFFT(const QAudioFormat format,
                  const int          windowSize,
                  const int          overlap);
    void addStreamFFT(QByteArray data);
    void stoptFFT();

private:

    QAudioSource *m_audioSource = nullptr;
    QAudioSink *m_audioSink = nullptr;
    QIODevice *m_ioSource = nullptr;

    FFTWorkThread *m_fftWorkThread;
    QThread *thread;

    const static int m_windowSize = 2048;
    const static int m_overlap = m_windowSize / 2;
    const static int m_count = m_windowSize - m_overlap;
};

#endif // AUDIOOBJECT_H
