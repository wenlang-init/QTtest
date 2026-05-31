#ifndef FFTWORKTHREAD_H
#define FFTWORKTHREAD_H

#include <QObject>
#include <QAudioFormat>

class fftw3Object;
class FFTWorkThread : public QObject {
    Q_OBJECT

public:

    explicit FFTWorkThread(QObject *parent = nullptr);
    ~FFTWorkThread();

    int getFFTPointCount() {
        return m_windowSize;
    }

public slots:

    void startFFT(const QAudioFormat format,
                  const int          windowSize,
                  const int          overlap);
    void addStreamFFT(QByteArray data);
    void stoptFFT();

signals:

    void fftwData(QVector<double>        dsdata,
                  QList<QVector<double> >fftdata,
                  QList<QVector<double> >radianfftdata);

private:

    fftw3Object *m_fftw3obj;
    QByteArray m_rdata;
    QAudioFormat m_format;

    int m_windowSize = 2048;
    int m_overlap = m_windowSize / 2;
    int m_count = m_windowSize - m_overlap;
};

#endif // FFTWORKTHREAD_H
