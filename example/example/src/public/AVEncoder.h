#pragma once

#include <QThread>
#include <QMutex>
#include <QImage>
#include <QString>
#include <QtEndian>
#include <vector>
#include <deque>
#include <cstdint>
#include <QDebug>
#include <QFile>

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
    bool encodeAudio(const int16_t *samples,
                     int            nb_samples);
    bool flush();
    int  getAudioFrameSize() {
        if (m_audioCodecCtx) {
            return m_audioCodecCtx->frame_size;
        }
        return 1024; // 默认回退值
    }

    static int test(void);

private:

    // 辅助函数
    bool encodeAudioFrame(const int16_t *data,
                          int            samples);

    bool openOutputFile();
    bool initVideoCodec();
    bool initAudioCodec();
    bool writePacket(AVPacket       *pkt,
                     AVStream       *stream,
                     AVCodecContext *codecCtx);
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

    // 音频内部缓冲区 (存储交错的 int16_t 数据)
    std::deque<int16_t>m_audioBuffer;

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

    // 是否丢弃多余的音频帧数据
    void setDiscardMoreAudioFrame(bool enable) {
        m_isDiscardMoreAudioFrame = enable;
    }

    int getAudioFrameSize() {
        return m_audioFrameSize;
    }

    int getAudioOneFrameSize() {
        const double samplesPerFrameDouble =
            static_cast<double>(m_sampleRate) / m_fps;
        const int samplesPerFrame =
            static_cast<int>(std::round(samplesPerFrameDouble));

        // const int samplesPerFrame = getAudioFrameSize();
        return samplesPerFrame * m_channels;
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
        if (audio.size() < 2) return;

        const int16_t *dataPtr =
            reinterpret_cast<const int16_t *>(audio.constData());
        size_t dataCount = (audio.size() / 2);

        if (m_isRunning.load()) {
            // 转为16bit位宽数据
            int count = audio.size() / 2 - audio.size() % 2;

            if (count < m_channels) return;

            // if (count % m_channels) count = count - 1;  // m_channels 1 or 2

            QMutexLocker locker(&m_mutexAudio);
            m_audioData.insert(m_audioData.end(), dataPtr,
                               dataPtr + dataCount);
        }
    }

    void addAudio(const std::vector<int16_t>& audio) {
        if (m_isRunning.load()) {
            QMutexLocker locker(&m_mutexAudio);
            m_audioData.insert(m_audioData.end(), audio.begin(), audio.end());
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

        m_audioFrameSize = encoder.getAudioFrameSize();

        QImage image;
        std::vector<int16_t> audio;
        bool isNew = false;

        // 计算每帧视频对应的音频样本数
        // 44100 / 30 = 1470 样本/帧
        // encodeAudio内部有缓存分片处理，这里samplesPerFrame可以传入任意值
        const double samplesPerFrameDouble =
            static_cast<double>(m_sampleRate) / m_fps;
        const int samplesPerFrame =
            static_cast<int>(std::round(samplesPerFrameDouble));

        // const int samplesPerFrame = 1024;
        const int audioFarmeSize = samplesPerFrame * m_channels;

        qint64 videoFramCount = 0;
        qint64 audioFramCount = 0;

        while (m_isRunning.load()) {
            // 视频
            isNew = false;

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
                    qWarning() << "Encode Video failed";
                    emit error("Encode Video failed");
                    break;
                }
                videoFramCount++;
            }

            // 音频
            if (m_isDiscardMoreAudioFrame &&
                (videoFramCount < audioFramCount)) continue;
            isNew = false;

            QMutexLocker locker(&m_mutexAudio);

            if (m_audioData.size() >= audioFarmeSize) {
                // 1. 拷贝前 n 个元素构造新向量
                audio = std::vector<int16_t>(m_audioData.begin(),
                                             m_audioData.begin() +
                                             audioFarmeSize);

                // 使用移动迭代器构造新向量（元素被移走）
                // auto first =
                // std::make_move_iterator(m_audioData.begin());
                // auto last = std::make_move_iterator(m_audioData.begin()
                //                                     + audioFarmeSize);
                // audio = std::vector<int16_t>(first, last);

                // 2. 从原向量中移除这些元素
                // 使用移动迭代器,移除原向量中的前 audioFarmeSize 个元素（此时它们处于“有效但未指定”状态，会被析构）
                m_audioData.erase(m_audioData.begin(),
                                  m_audioData.begin() + audioFarmeSize);
                isNew = true;
            }
            locker.unlock();

            if (isNew) {
                if (!encoder.encodeAudio(audio.data(), samplesPerFrame)) {
                    stop();
                    qWarning() << "Encode Audio failed";
                    emit error("Encode Audio failed");
                    break;
                }
                audioFramCount++;
            }

            QThread::usleep(10);
        }

        if (!m_usedUpdate) {
            // 处理剩余队列
            QList<QImage> tempImgList;

            {
                QMutexLocker locker(&m_mutex);
                tempImgList = m_imageList;
                m_imageList.clear();
                qDebug() << tempImgList.size();
            }

            for (const auto& img : tempImgList) {
                if (!encoder.encodeVideo(img)) {
                    qWarning() << "Encode Video failed";
                    emit error("Encode Video failed");
                    break;
                }
                videoFramCount++;
            }
        }

        // 处于音频
        std::deque<int16_t> tempAudioData;
        {
            QMutexLocker locker(&m_mutexAudio);
            tempAudioData = m_audioData;
            m_audioData.clear();
            qDebug() << tempAudioData.size();
        }
        long long index = 0;

        while (tempAudioData.size() >= index + audioFarmeSize) {
            // 当音频帧大于视频帧时，不再编码音频
            if (m_isDiscardMoreAudioFrame &&
                (videoFramCount < audioFramCount)) break;

            if (index < 0) break;
            auto begin = tempAudioData.begin() + index;
            auto end = begin + audioFarmeSize;
            audio = std::vector<int16_t>(begin, end);
            index += audioFarmeSize;

            if (!encoder.encodeAudio(audio.data(), samplesPerFrame)) {
                qWarning() << "Encode Audio failed";
                emit error("Encode Audio failed");
                break;
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
    int m_count = 0;     // 总帧数
    int m_countTemp = 0; // 判断是否有新帧
    std::deque<int16_t>m_audioData;

    int m_audioFrameSize = 1024;

    std::atomic_bool m_isRunning{ true };

    bool m_isDiscardMoreAudioFrame = false;
};
