#ifndef SYSTEMAUDIOSOURCE_H
#define SYSTEMAUDIOSOURCE_H

#include <QObject>
#include <QAudioSink>
#include <QAudioSource>
#include <QAudioDevice>
#include <QBuffer>

class systemAudioSource : public QObject {
    Q_OBJECT

public:

    explicit systemAudioSource(QObject *parent = nullptr);
    ~systemAudioSource();
    void openSource(QAudioFormat format,
                    QAudioDevice info,
                    int          bufferSize);
    void colseSource();

    // 0~1
    void setSourceVolume(qreal volume);

    void susPend();
    void resume();

protected slots:

    void readAudioData();

signals:

    void audioSourceStatus(QAudio::State state);

    void openSourceSig(QAudioFormat format,
                       QAudioDevice info,
                       int          bufferSize);
    void colseSourceSig();

    void setSourceVolumeSig(qreal volume);

    void susPendSig();
    void resumeSig();

    void readData(QByteArray data);

private:

    QAudioSource *m_audioSource = nullptr;
    QIODevice *m_ioSource = nullptr;

    QAudioSink *m_audioSinkPCM = nullptr;
    QByteArray m_data;
};

#endif // SYSTEMAUDIOSOURCE_H
