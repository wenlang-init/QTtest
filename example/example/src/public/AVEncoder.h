#pragma once

#include <QThread>
#include <QMutex>
#include <QImage>
#include <QString>
#include <QtEndian>
#include <vector>
#include <cstdint>
#include <QDebug>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

class AVEncoder {
public:

    AVEncoder(const QString& outputFile,
              int            videoWidth,
              int            videoHeight,
              int            fps,
              int            sampleRate,
              int            channels,
              int            videoBitrate = 0,
              int            crf = 30,
              int            preset = 8);
    ~AVEncoder();

    bool init();
    bool encodeVideo(const QImage& image);

    // int        convertAudioWithLoop(uint8_t **inData,
    //                                 int       nbInSamples);
    bool       encodeAudio(const int16_t *samples,
                           int            nb_samples);
    bool       flush();

    static int test(void);

private:

    bool openOutputFile();
    bool initVideoCodec();
    bool initAudioCodec();
    bool writePacket(AVPacket *pkt,
                     AVStream *stream);
    bool receivePackets(AVCodecContext *codecCtx,
                        AVStream       *stream,
                        bool            flush = false);

    AVFormatContext *m_formatCtx;
    AVCodecContext *m_videoCodecCtx;
    AVCodecContext *m_audioCodecCtx;
    AVStream *m_videoStream;
    AVStream *m_audioStream;
    SwsContext *m_swsCtx;
    SwrContext *m_swrCtx;
    AVFrame *m_videoFrame;
    AVFrame *m_audioFrame;
    AVPacket *m_packet;

    int64_t m_videoPts;
    int64_t m_audioPts; // 以采样数计

    QString m_outputFile;
    int m_videoWidth, m_videoHeight, m_fps;
    int m_sampleRate;
    int m_channels; // 通道数，用于初始化
    int m_videoBitrate;
    int m_crf, m_preset;
    bool m_initialized;

    char errbuf[AV_ERROR_MAX_STRING_SIZE] = { 0 };
};

class AVEncoderThread : public QThread {
    Q_OBJECT

private:

    // 参考AVEncoder::test函数进行操作delete
    ~AVEncoderThread() {
        qDebug() << "AVEncoderThread destroyed";
    }

public:

    // 阻塞等待析构
    void destory(unsigned long time = -1) {
        stop();
        quit();
        wait(time);

        if (isRunning()) {
            terminate();
            wait();
        }
        delete this;
    }

    explicit AVEncoderThread(const QString& outputFile,
                             int            videoWidth,
                             int            videoHeight,
                             int            fps,
                             int            sampleRate,
                             int            channels,
                             int            videoBitrate = 0,
                             int            crf = 30,
                             int            preset = 8) : QThread{nullptr} {
        m_outputFile = outputFile;
        m_videoWidth = videoWidth;
        m_videoHeight = videoHeight;
        m_fps = fps;
        m_sampleRate = sampleRate;
        m_channels = channels;
        m_videoBitrate = videoBitrate;
        m_crf = crf;
        m_preset = preset;
    }

    // 停止线程，并且会在编码完成后自动退出线程，然后自动deleteLater
    void stop() {
        m_isRunning.store(false);
    }

    void usedUpdate(bool usedUpdate) {
        m_usedUpdate = usedUpdate;
    }

    void addImage(const QImage& image) {
        if (m_isRunning.load()) {
            QMutexLocker locker(&m_mutex);
            m_count++;

            if (!m_usedUpdate) {
                m_imageList.append(image);
            } else {
                m_imageList.clear();
                m_image = image;
            }
        }
    }

    void addAudio(const QByteArray& audio) {
        if (m_isRunning.load()) {
            // 转为16bit位宽数据
            std::vector<int16_t> audiodata;

            for (int i = 0; i <= audio.size() - (m_channels * 2);
                 i += (m_channels * 2)) {
                const int16_t *p = (const int16_t *)(&audio.data()[i]);

                for (int j = 0; j < m_channels; j++) {
                    audiodata.push_back(p[j]);
                }
            }

            QMutexLocker locker(&m_mutexAudio);
            m_countAudio++;

            if (!m_usedUpdate) {
                m_audioList.append(audiodata);
            } else {
                m_audioList.clear();
                m_audio = audiodata;
            }
        }
    }

    void addAudio(const std::vector<int16_t>& audio) {
        if (m_isRunning.load()) {
            QMutexLocker locker(&m_mutexAudio);
            m_countAudio++;

            if (!m_usedUpdate) {
                m_audioList.append(audio);
            } else {
                m_audioList.clear();
                m_audio = audio;
            }
        }
    }

protected:

    void run() override
    {
        AVEncoder encoder(m_outputFile, m_videoWidth, m_videoHeight,
                          m_fps, m_sampleRate, m_channels,
                          m_videoBitrate, m_crf, m_preset);

        if (!encoder.init()) {
            qCritical() << "init failed!";
            emit error("init failed");
            stop();
            emit finish();
            return;
        }

        QImage image;
        std::vector<int16_t> audio;
        bool isNew = false;

        while (m_isRunning.load()) {
            if (m_usedUpdate) {
                QMutexLocker locker(&m_mutex);

                if (m_countTemp != m_count) {
                    m_countTemp = m_count;
                    isNew = true;
                    image = m_image;
                }
                locker.unlock();
            } else {
                QMutexLocker locker(&m_mutex);

                if (m_imageList.size() > 0) {
                    image = m_imageList.first();
                    m_imageList.removeFirst();
                    isNew = true;
                }
                locker.unlock();
            }

            if (isNew) {
                if (!encoder.encodeVideo(image)) {
                    stop();
                    emit error("Encode failed");
                    break;
                }
            }
            isNew = false;

            if (m_usedUpdate) {
                QMutexLocker locker(&m_mutexAudio);

                if (m_countTempAudio == m_countAudio) {
                    m_countTempAudio = m_countAudio;
                    audio = m_audio;
                    isNew = true;
                }
                locker.unlock();
            } else {
                QMutexLocker locker(&m_mutexAudio);

                if (m_audioList.size() > 0) {
                    audio = m_audioList.first();
                    m_audioList.removeFirst();
                    isNew = true;
                }
                locker.unlock();
            }

            if (isNew) {
                if (!encoder.encodeAudio(audio.data(),
                                         audio.size() / m_channels)) {
                    stop();
                    emit error("Encode failed");
                    break;
                }
            }

            QThread::usleep(10);
        }

        if (!m_usedUpdate) {
            // 处理剩余队列
            QList<QImage> tempImgList;
            QList<std::vector<int16_t> > tempAudioList;

            {
                QMutexLocker locker(&m_mutex);
                tempImgList = m_imageList;
                m_imageList.clear();
            }
            {
                QMutexLocker locker(&m_mutexAudio);
                tempAudioList = m_audioList;
                m_audioList.clear();
            }

            for (const auto& img : tempImgList) {
                if (!encoder.encodeVideo(img)) {
                    emit error("Encode failed");
                    break;
                }
            }

            for (const auto& aud : tempAudioList) {
                if (!encoder.encodeAudio(aud.data(), aud.size() / m_channels)) {
                    emit error("Encode failed");
                    break;
                }
            }
        }

        encoder.flush();
        emit finish();
    }

signals:

    void finish();
    void error(const QString& msg);

private:

    QString m_outputFile;
    int m_videoWidth;
    int m_videoHeight;
    int m_fps;
    int m_sampleRate;
    int m_channels;
    int m_videoBitrate;
    int m_crf;
    int m_preset;

    QMutex m_mutex;
    QMutex m_mutexAudio;

    bool m_usedUpdate = true;

    QList<QImage>m_imageList;
    QImage m_image;
    int m_count = 0;          // 总帧数
    int m_countTemp = 0;      // 判断是否有新帧
    QList<std::vector<int16_t> >m_audioList;
    std::vector<int16_t>m_audio;
    int m_countAudio = 0;     // 总帧数
    int m_countTempAudio = 0; // 判断是否有新帧

    std::atomic_bool m_isRunning{ true };
};
