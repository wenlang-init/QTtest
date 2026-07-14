#include "AVEncoder.h"
#include <QDebug>

// ---------- 构造函数 ----------
AVEncoder::AVEncoder(const QString& outputFile,
                     int videoWidth, int videoHeight, int fps,
                     int sampleRate, int channels,
                     int videoBitrate, int crf, int preset)
    : m_outputFile(outputFile)
    , m_videoWidth(videoWidth), m_videoHeight(videoHeight), m_fps(fps)
    , m_sampleRate(sampleRate), m_channels(channels)
    , m_videoBitrate(videoBitrate), m_crf(crf), m_preset(preset)
    , m_formatCtx(nullptr), m_videoCodecCtx(nullptr), m_audioCodecCtx(nullptr)
    , m_videoStream(nullptr), m_audioStream(nullptr)
    , m_swsCtx(nullptr), m_swrCtx(nullptr)
    , m_videoFrame(nullptr), m_audioFrame(nullptr), m_packet(nullptr)
    , m_videoPts(0), m_audioPts(0), m_initialized(false)
{
    avformat_network_init();
}

AVEncoder::~AVEncoder() {
    if (m_initialized) flush();
}

// ---------- 初始化 ----------
bool AVEncoder::init() {
    if (m_initialized) return true;

    if (!openOutputFile()) return false;

    if (!initVideoCodec()) return false;

    if (!initAudioCodec()) return false;

    // 创建视频转换上下文 (BGRA -> YUV420P)
    m_swsCtx = sws_getContext(
        m_videoWidth, m_videoHeight, AV_PIX_FMT_BGRA,
        m_videoWidth, m_videoHeight, AV_PIX_FMT_YUV420P,
        SWS_BILINEAR, nullptr, nullptr, nullptr);

    if (!m_swsCtx) {
        qCritical() << "Failed to create SwsContext";
        return false;
    }

    // 创建音频重采样上下文 (S16 -> FLTP)
    m_swrCtx = swr_alloc();

    if (!m_swrCtx) {
        qCritical() << "Failed to allocate SwrContext";
        return false;
    }

    // 输入参数（假设输入为 S16 交错，采样率 m_sampleRate，通道数 m_channels）
    av_opt_set_sample_fmt(m_swrCtx, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    av_opt_set_int(m_swrCtx, "in_sample_rate", m_sampleRate, 0);

    // 设置输入通道布局（根据 m_channels 获取默认布局）
    AVChannelLayout in_layout;
    av_channel_layout_default(&in_layout, m_channels);
    av_opt_set_chlayout(m_swrCtx, "in_chlayout", &in_layout, 0);

    // 输出参数（与音频编码器一致）
    av_opt_set_sample_fmt(m_swrCtx,
                          "out_sample_fmt",
                          m_audioCodecCtx->sample_fmt,
                          0);
    av_opt_set_int(m_swrCtx, "out_sample_rate", m_audioCodecCtx->sample_rate, 0);

    // 使用编码器的 ch_layout 作为输出布局
    av_opt_set_chlayout(m_swrCtx, "out_chlayout", &m_audioCodecCtx->ch_layout, 0);

    // 可选：设置矩阵混合参数等（如有需要）
    // av_opt_set_int(m_swrCtx, "matrix_encoding", AV_MATRIX_ENCODING_NONE, 0);

    int ret;

    // 初始化重采样器
    if ((ret = swr_init(m_swrCtx)) < 0) {
        av_strerror(ret, errbuf, sizeof(errbuf));
        qCritical() << "Failed to initialize SwrContext:" << errbuf;
        return false;
    }

    // 分配复用帧和包
    m_packet = av_packet_alloc();

    if (!m_packet) return false;

    m_videoFrame = av_frame_alloc();
    m_videoFrame->format = AV_PIX_FMT_YUV420P;
    m_videoFrame->width = m_videoWidth;
    m_videoFrame->height = m_videoHeight;

    if (av_frame_get_buffer(m_videoFrame, 32) < 0) return false;

    m_audioFrame = av_frame_alloc();
    m_audioFrame->format = m_audioCodecCtx->sample_fmt;
    m_audioFrame->sample_rate = m_audioCodecCtx->sample_rate;

    // 使用 ch_layout 设置通道布局（FFmpeg 8.x 推荐）
    av_channel_layout_copy(&m_audioFrame->ch_layout, &m_audioCodecCtx->ch_layout);
    m_audioFrame->nb_samples = m_audioCodecCtx->frame_size; // 通常为 1024

    if (av_frame_get_buffer(m_audioFrame, 0) < 0) return false;

    // 写入文件头
    if (avformat_write_header(m_formatCtx, nullptr) < 0) {
        qCritical() << "Failed to write header";
        return false;
    }

    m_initialized = true;
    return true;
}

// ---------- 打开输出文件 ----------
bool AVEncoder::openOutputFile() {
    avformat_alloc_output_context2(&m_formatCtx,
                                   nullptr,
                                   "mp4",
                                   m_outputFile.toUtf8().constData());

    if (!m_formatCtx) {
        qCritical() << "Failed to create output context";
        return false;
    }

    if (!(m_formatCtx->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&m_formatCtx->pb, m_outputFile.toUtf8().constData(),
                      AVIO_FLAG_WRITE) < 0) {
            qCritical() << "Failed to open output file";
            return false;
        }
    }
    return true;
}

// ---------- 初始化视频编码器 ----------
bool AVEncoder::initVideoCodec() {
    const AVCodec *codec = avcodec_find_encoder_by_name("libsvtav1");

    if (!codec) codec = avcodec_find_encoder_by_name("libaom-av1");

    if (!codec) {
        qCritical() << "AV1 encoder not found (libsvtav1 or libaom-av1 required)";
        return false;
    }

    m_videoCodecCtx = avcodec_alloc_context3(codec);

    if (!m_videoCodecCtx) return false;

    // 基本参数
    m_videoCodecCtx->width = m_videoWidth;
    m_videoCodecCtx->height = m_videoHeight;
    m_videoCodecCtx->time_base = AVRational{ 1, m_fps };
    m_videoCodecCtx->framerate = AVRational{ m_fps, 1 };
    m_videoCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    m_videoCodecCtx->thread_count = 0; // 自动多线程

    // 码率控制：CRF 优先，否则用固定码率
    if (m_videoBitrate > 0) {
        m_videoCodecCtx->bit_rate = m_videoBitrate;
        m_videoCodecCtx->rc_max_rate = m_videoBitrate;
        m_videoCodecCtx->rc_buffer_size = m_videoBitrate;
    } else {
        m_videoCodecCtx->flags |= AV_CODEC_FLAG_QSCALE;
        m_videoCodecCtx->global_quality = m_crf * FF_QP2LAMBDA;
    }
    av_opt_set(m_videoCodecCtx->priv_data,
               "preset",
               QString::number(m_preset).toUtf8().constData(),
               0);

    if (strcmp(codec->name, "libaom-av1") == 0) {
        m_videoCodecCtx->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
    }

    if (avcodec_open2(m_videoCodecCtx, codec, nullptr) < 0) {
        qCritical() << "Failed to open video codec";
        return false;
    }

    // 添加视频流
    m_videoStream = avformat_new_stream(m_formatCtx, codec);

    if (!m_videoStream) return false;

    m_videoStream->id = m_formatCtx->nb_streams - 1;
    m_videoStream->time_base = m_videoCodecCtx->time_base;
    avcodec_parameters_from_context(m_videoStream->codecpar, m_videoCodecCtx);

    return true;
}

// ---------- 初始化音频编码器（FFmpeg 8.x 适配）----------
bool AVEncoder::initAudioCodec() {
    const AVCodec *codec = avcodec_find_encoder_by_name("aac");

    if (!codec) {
        qCritical() << "AAC encoder not found";
        return false;
    }

    m_audioCodecCtx = avcodec_alloc_context3(codec);

    if (!m_audioCodecCtx) return false;

    // ---- 使用 ch_layout 替代 channels (推荐) ----
    AVChannelLayout layout;

    // 根据声道数获取默认布局（例如：2声道 -> 立体声）
    av_channel_layout_default(&layout, m_channels);
    av_channel_layout_copy(&m_audioCodecCtx->ch_layout, &layout);

    // 同时保留 channels 字段（旧版兼容）
    // m_audioCodecCtx->ch_layout.nb_channels = m_channels;

    m_audioCodecCtx->sample_fmt = AV_SAMPLE_FMT_FLTP;
    m_audioCodecCtx->sample_rate = m_sampleRate;
    m_audioCodecCtx->bit_rate = 128000;
    m_audioCodecCtx->time_base = AVRational{ 1, m_sampleRate };

    if (avcodec_open2(m_audioCodecCtx, codec, nullptr) < 0) {
        qCritical() << "Failed to open audio codec";
        return false;
    }

    m_audioStream = avformat_new_stream(m_formatCtx, codec);

    if (!m_audioStream) return false;

    m_audioStream->id = m_formatCtx->nb_streams - 1;
    m_audioStream->time_base = m_audioCodecCtx->time_base;
    avcodec_parameters_from_context(m_audioStream->codecpar, m_audioCodecCtx);

    return true;
}

// ---------- 编码视频帧 ----------
bool AVEncoder::encodeVideo(const QImage& image) {
    if (!m_initialized) return false;

    // 转换 QImage 为 BGRA（若需）
    QImage img = image;

    if (img.format() != QImage::Format_ARGB32) {
        img = img.convertToFormat(QImage::Format_ARGB32);

        if (img.isNull()) return false;
    }

    // 使用 sws_scale 转为 YUV420P
    const uint8_t *srcData[4] = { img.bits() };
    int srcLinesize[4] = { static_cast<int>(img.bytesPerLine()) };
    sws_scale(m_swsCtx, srcData, srcLinesize, 0, m_videoHeight,
              m_videoFrame->data, m_videoFrame->linesize);

    // 设置 PTS（以帧为单位）
    m_videoFrame->pts = m_videoPts++;

    // 发送帧
    int ret = avcodec_send_frame(m_videoCodecCtx, m_videoFrame);

    if (ret < 0) {
        av_strerror(ret, errbuf, sizeof(errbuf));
        qCritical() << "Error sending video frame:" << errbuf;
        return false;
    }

    // 接收所有输出包
    return receivePackets(m_videoCodecCtx, m_videoStream);
}

// ---------- 编码音频帧----------
bool AVEncoder::encodeAudio(const int16_t *samples, int nb_samples) {
    if (!m_initialized || !samples || (nb_samples <= 0)) {
        qCritical() << "Invalid audio input parameters";
        return false;
    }

    // 1. 计算重采样后的输出样本数
    int out_samples = av_rescale_rnd(nb_samples,
                                     m_audioCodecCtx->sample_rate,
                                     m_sampleRate,
                                     AV_ROUND_UP);

    if (out_samples <= 0) return false;

    // 2. 准备输入数据（交错 S16 → 单指针即可）
    const uint8_t *in_data[1] = { reinterpret_cast<const uint8_t *>(samples) };

    // 3. 确保音频帧可写，直接用帧的 data 作为输出平面
    av_frame_make_writable(m_audioFrame);
    m_audioFrame->nb_samples = out_samples; // 临时设为最大，实际按转换结果修正

    // 4. 执行重采样，输出直接进帧
    int converted = swr_convert(m_swrCtx,
                                m_audioFrame->data, // 输出平面指针数组
                                out_samples,
                                in_data,
                                nb_samples);

    if (converted < 0) {
        av_strerror(converted, errbuf, sizeof(errbuf));
        qCritical() << "swr_convert failed:" << errbuf;
        return false;
    }

    if (converted == 0) {
        qWarning() << "swr_convert returned 0 samples";
        return true;
    }

    // 5. 根据实际转换样本数修正帧参数
    m_audioFrame->nb_samples = converted;
    m_audioFrame->pts = m_audioPts;
    m_audioPts += converted;

    // 6. 发送帧到编码器
    int ret = avcodec_send_frame(m_audioCodecCtx, m_audioFrame);

    if (ret < 0) {
        av_strerror(ret, errbuf, sizeof(errbuf));
        qCritical() << "Error sending audio frame:" << errbuf;
        return false;
    }

    return receivePackets(m_audioCodecCtx, m_audioStream);
}

// ---------- 写入单个包 ----------
bool AVEncoder::writePacket(AVPacket *pkt, AVStream *stream) {
    // 调整时间戳
    av_packet_rescale_ts(pkt, stream->time_base, stream->time_base);
    pkt->stream_index = stream->index;

    int ret = av_interleaved_write_frame(m_formatCtx, pkt);
    av_packet_unref(pkt);

    if (ret < 0) {
        av_strerror(ret, errbuf, sizeof(errbuf));
        qCritical() << "Error writing packet:" << errbuf;
        return false;
    }
    return true;
}

// ---------- 接收数据包并写入 ----------
bool AVEncoder::receivePackets(AVCodecContext *codecCtx,
                               AVStream       *stream,
                               bool            flush) {
    int ret;

    while (true) {
        ret = avcodec_receive_packet(codecCtx, m_packet);

        if ((ret == AVERROR(EAGAIN)) || (ret == AVERROR_EOF)) break;

        if (ret < 0) {
            av_strerror(ret, errbuf, sizeof(errbuf));
            qCritical() << "Error receiving packet:" << errbuf;
            return false;
        }

        if (!writePacket(m_packet, stream)) return false;
    }
    return true;
}

// ---------- 冲刷 ----------
bool AVEncoder::flush() {
    if (!m_initialized) return true;

    // 冲刷视频
    avcodec_send_frame(m_videoCodecCtx, nullptr);
    receivePackets(m_videoCodecCtx, m_videoStream, true);

    // 冲刷音频
    avcodec_send_frame(m_audioCodecCtx, nullptr);
    receivePackets(m_audioCodecCtx, m_audioStream, true);

    av_write_trailer(m_formatCtx);

    // 释放资源（无需调用 avcodec_close）
    if (m_swsCtx) sws_freeContext(m_swsCtx);

    if (m_swrCtx) swr_free(&m_swrCtx);

    if (m_videoFrame) av_frame_free(&m_videoFrame);

    if (m_audioFrame) av_frame_free(&m_audioFrame);

    if (m_packet) av_packet_free(&m_packet);

    if (m_videoCodecCtx) avcodec_free_context(&m_videoCodecCtx);

    if (m_audioCodecCtx) avcodec_free_context(&m_audioCodecCtx);

    if (m_formatCtx) {
        if (m_formatCtx->pb) avio_closep(&m_formatCtx->pb);
        avformat_free_context(m_formatCtx);
    }

    m_initialized = false;
    return true;
}

#include <QImage>
#include <QPainter>
int AVEncoder::test(void)
{
    // 创建编码器
    AVEncoder encoder("output_av1.mp4",
                      640, 480, 30, // 视频参数
                      44100, 2,     // 音频参数：44.1kHz，立体声
                      0, 30, 8);    // 码率控制：CRF=30, preset=8

    if (!encoder.init()) {
        qCritical() << "Encoder init failed";
        return -1;
    }

    // 模拟视频生成（生成旋转的矩形）
    QImage   frame(640, 480, QImage::Format_ARGB32);
    QPainter painter(&frame);
    int angle = 0;

    int count = 1000;

    for (int i = 0; i < count; ++i) {
        frame.fill(Qt::black);
        painter.setBrush(Qt::red);
        painter.translate(320, 240);
        painter.rotate(angle);
        painter.drawRect(-100, -100, 200, 200);
        painter.resetTransform();
        angle += 2;

        if (!encoder.encodeVideo(frame)) {
            qCritical() << "Video encoding failed";
            break;
        }

        // 模拟音频数据（此处使用生成的正弦波，实际应从设备采集）
        const int samplesPerFrame = 1024;
        std::vector<int16_t> audio(samplesPerFrame * 2); // 立体声

        for (int j = 0; j < samplesPerFrame; ++j) {
            int16_t sample =
                static_cast<int16_t>(32767 * sin(2 * 3.14159 * 440 * j / 44100));
            audio[j * 2] = sample;
            audio[j * 2 + 1] = sample;
        }

        if (!encoder.encodeAudio(audio.data(), samplesPerFrame)) {
            qCritical() << "Audio encoding failed";
            break;
        }
    }

    encoder.flush();
    qDebug() << "Encoding finished.";

    ///////////////////////////////////////////////////////////////////
    AVEncoderThread *avent = new AVEncoderThread("./123.mp4",
                                                 640, 480, 30,
                                                 44100, 2,
                                                 0, 30, 8);
    bool isfinish = false;

    auto finishLambda =
        [ = ]()mutable {
            qDebug();
            isfinish = true;
            avent->quit();
        };
    auto finishedLambda =
        [&]() {
            qDebug();
            isfinish = true;
            avent->deleteLater();
        };
    QObject::connect(avent, &AVEncoderThread::finish,   finishLambda);
    QObject::connect(avent, &AVEncoderThread::finished, finishedLambda);
    QObject::connect(avent, &AVEncoderThread::error,    [](const QString& msg)
    {
        qCritical() << msg;
    });

    avent->usedUpdate(false);
    avent->start();

    for (int i = 0; i < count; ++i) {
        if (!avent) break;
        frame.fill(Qt::black);
        painter.setBrush(Qt::red);
        painter.translate(320, 240);
        painter.rotate(angle);
        painter.drawRect(-100, -100, 200, 200);
        painter.resetTransform();
        angle += 2;

        avent->addImage(frame);

        // 模拟音频数据（此处使用生成的正弦波，实际应从设备采集）
        const int samplesPerFrame = 1024;
        std::vector<int16_t> audio(samplesPerFrame * 2); // 立体声

        for (int j = 0; j < samplesPerFrame; ++j) {
            int16_t sample =
                static_cast<int16_t>(32767 * sin(2 * 3.14159 * 440 * j / 44100));
            audio[j * 2] = sample;
            audio[j * 2 + 1] = sample;
        }

        avent->addAudio(audio);
    }

    // if (avent) avent->destory();
    if (avent) avent->stop();

    qDebug() << "wait Encoding finished.";

    while (!isfinish) {
        QThread::msleep(10);
    }
    qDebug() << "Encoding finished.";
    return 0;
}
