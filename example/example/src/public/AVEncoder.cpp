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

    // 是否支持可变帧大小
    if (m_audioCodecCtx->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE) {
        qDebug() << "支持可变帧大小" << m_audioCodecCtx->codec->capabilities;
    }

    // 是否允许你发送一个比 m_audioCodecCtx->frame_size 小的帧作为最后一帧
    if (m_audioCodecCtx->codec->capabilities & AV_CODEC_CAP_SMALL_LAST_FRAME) {
        qDebug() << "允许小于frame_size的最后一帧" << m_audioCodecCtx->codec->capabilities;
    }

    if (av_frame_get_buffer(m_audioFrame, 0) < 0) {
        qCritical() << "Failed to allocate audio frame buffer";
        return false;
    }

    // qDebug() << m_videoStream->avg_frame_rate.num
    //          << m_videoStream->avg_frame_rate.den
    //          << m_videoStream->r_frame_rate.num
    //          << m_videoStream->r_frame_rate.den
    //          << m_videoCodecCtx->framerate.num
    //          << m_videoCodecCtx->framerate.den;

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

    if (!codec) {
        codec = avcodec_find_encoder_by_name("libaom-av1");

        if (!codec) {
            qCritical() <<
                "AV1 encoder not found (libsvtav1 or libaom-av1 required)";
            return false;
        }
        qInfo() << "used encoder libaom-av1";
    } else {
        qInfo() << "used encoder libsvtav1";
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

    // m_videoStream->time_base = AVRational{ 1, 90000 }; // MP4 常用

    // 显式设置流级别的帧率
    m_videoStream->avg_frame_rate = m_videoCodecCtx->framerate;
    m_videoStream->r_frame_rate = m_videoCodecCtx->framerate;

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

// ---------- 编码音频帧 ----------
bool AVEncoder::encodeAudio(const int16_t *samples, int nb_samples) {
    if (!m_initialized || !samples || (nb_samples <= 0)) {
        qCritical() << "Invalid audio input parameters"
                    << m_initialized << samples << nb_samples;
        return false;
    }

    int frameSize = getAudioFrameSize();                    // 例如 1024
    int channelCount = m_channels;                          // 例如 2
    int requiredSamplesPerFrame = frameSize * channelCount; // 例如 2048

    // 1. 将新数据追加到内部缓冲区
    m_audioBuffer.insert(m_audioBuffer.end(),
                         samples,
                         samples + nb_samples * channelCount);

    // 2. 循环检查缓冲区是否足够编码一帧
    while (static_cast<int>(m_audioBuffer.size()) >= requiredSamplesPerFrame) {
        // 提取一帧数据
        std::vector<int16_t> frameData(m_audioBuffer.begin(),
                                       m_audioBuffer.begin() +
                                       requiredSamplesPerFrame);

        // 从缓冲区移除已提取的数据
        m_audioBuffer.erase(m_audioBuffer.begin(),
                            m_audioBuffer.begin() + requiredSamplesPerFrame);

        // 编码这一帧
        if (!encodeAudioFrame(frameData.data(), frameSize)) {
            return false;
        }
    }

    return true;
}

bool AVEncoder::encodeAudioFrame(const int16_t *samples, int nb_samples)
{
    if (!m_initialized || !samples || (nb_samples <= 0)) {
        qCritical() << "Invalid audio input parameters"
                    << m_initialized << samples << nb_samples;
        return false;
    }
#if 0

    // 1. 计算重采样后的输出样本数（向上取整）
    // 注意：这里我们需要确保输出的样本数不超过 m_audioFrame 能容纳的大小
    // m_audioFrame->nb_samples 是在 init 中设置为 codec_ctx->frame_size 的
    int max_out_samples = m_audioFrame->nb_samples;

    int out_samples = av_rescale_rnd(nb_samples,
                                     m_audioCodecCtx->sample_rate,
                                     m_sampleRate,
                                     AV_ROUND_UP);

    // 如果重采样后的样本数超过了 frame 的大小，我们需要截断或分多次处理
    // 为了简单起见，这里我们限制一次处理的输入样本数，或者允许 swr_convert 输出部分数据
    // 但通常 swr_convert 会处理缓冲。
    // 更稳健的做法是：确保 out_samples <= max_out_samples

    if (out_samples > max_out_samples) {
        // 如果预期输出超过 frame 容量，限制本次处理的输入样本数
        // 反向计算允许的输入样本数
        int allowed_in_samples = av_rescale_rnd(max_out_samples,
                                                m_sampleRate,
                                                m_audioCodecCtx->sample_rate,
                                                AV_ROUND_DOWN);

        if (allowed_in_samples <= 0) allowed_in_samples = 1;

        // 递归或循环处理剩余部分比较复杂，这里简化为只处理能放入一帧的数据
        // 在实际生产中，应该建立一个循环直到所有输入样本都被消耗
        qWarning() << "Audio chunk too large, truncating to fit one frame."
                   << out_samples << max_out_samples;
        nb_samples = allowed_in_samples;
        out_samples = max_out_samples;
    }

    if (out_samples <= 0) {
        qCritical() << "Resampled sample count is zero";
        return false;
    }
#else // if 0
    int out_samples = m_audioFrame->nb_samples;
#endif // if 0

    // 2. 获取输出声道数
    int out_channels = m_audioCodecCtx->ch_layout.nb_channels;

    if (out_channels <= 0) {
        qCritical() << "Invalid output channel count";
        return false;
    }

    // 3. 准备输入数据（注意：输入是交错 S16）
    const uint8_t *in_data[1] = { reinterpret_cast<const uint8_t *>(samples) };

    // 4. 直接使用 m_audioFrame 的数据指针作为输出目标
    // m_audioFrame->data 已经由 av_frame_get_buffer 分配好
    uint8_t *out_data[8]; // 最大支持8通道

    for (int i = 0; i < out_channels; i++) {
        out_data[i] = m_audioFrame->data[i];
    }

    // 5. 执行重采样
    // 注意：swr_convert 的第三个参数是输出平面的数量（对于平面格式如FLTP，是每个通道的样本数）
    // 第四个参数是输入样本数
    int converted = swr_convert(m_swrCtx,
                                out_data,    // 输出数据指针数组 (Planar)
                                out_samples, // 每个通道的输出样本数
                                in_data,     // 输入数据指针数组 (Packed S16)
                                nb_samples); // 输入样本数

    if (converted < 0) {
        av_strerror(converted, errbuf, sizeof(errbuf));
        qCritical() << "swr_convert failed:" << errbuf;
        return false;
    }

    if (converted == 0) {
        qWarning() << "swr_convert returned 0 samples";
        return true;
    }

    // 6. 填充 AVFrame 元数据
    m_audioFrame->nb_samples = converted;
    m_audioFrame->pts = m_audioPts;
    m_audioPts += converted;

    // 7. 发送帧到编码器
    // 此时 m_audioFrame->data 指向的是 frame 自己拥有的内存，
    int ret = avcodec_send_frame(m_audioCodecCtx, m_audioFrame);

    if (ret < 0) {
        av_strerror(ret, errbuf, sizeof(errbuf));
        qCritical() << "Error sending audio frame:" << errbuf;
        return false;
    }

    // 8. 接收所有编码后的数据包
    return receivePackets(m_audioCodecCtx, m_audioStream);
}

// ---------- 写入单个包 ----------
bool AVEncoder::writePacket(AVPacket       *pkt,
                            AVStream       *stream,
                            AVCodecContext *codecCtx) {
    // 调整时间戳
    av_packet_rescale_ts(pkt, codecCtx->time_base, stream->time_base);
    pkt->stream_index = stream->index;

    // static int testindex = 0;
    // if (testindex++ < 6) qDebug() << pkt->pts << pkt->dts;

    // 确保 DTS 有效，如果为 NOPTS，则设为 PTS (适用于无 B 帧或简单情况)
    if (pkt->dts == AV_NOPTS_VALUE) {
        pkt->dts = pkt->pts;
    }

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

        if (!writePacket(m_packet, stream, codecCtx)) return false;
    }
    return true;
}

// ---------- 冲刷 ----------
bool AVEncoder::flush() {
    if (!m_initialized) return true;

    // 处理音频缓冲区中剩余不足一帧的数据
    if (!m_audioBuffer.empty()) {
        int frameSize = getAudioFrameSize();
        int channelCount = m_channels;
        int requiredSamplesPerFrame = frameSize * channelCount;

        qDebug() << "Flushing remaining audio samples:" << m_audioBuffer.size() /
            channelCount;

        // 创建一个新的向量，大小为完整的一帧
        std::vector<int16_t> finalFrame(requiredSamplesPerFrame, 0); // 初始化为0
                                                                     // (静音)

        // 将剩余数据拷贝进去
        std::copy(m_audioBuffer.begin(), m_audioBuffer.end(), finalFrame.begin());

        // 编码这最后一帧（包含补零）
        encodeAudioFrame(finalFrame.data(), frameSize);

        m_audioBuffer.clear();
    }

    // 冲刷视频
    avcodec_send_frame(m_videoCodecCtx, nullptr);
    receivePackets(m_videoCodecCtx, m_videoStream, true);

    // 冲刷音频
    avcodec_send_frame(m_audioCodecCtx, nullptr);
    receivePackets(m_audioCodecCtx, m_audioStream, true);

    av_write_trailer(m_formatCtx);

    // 释放资源
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

#include <QPainter>
#include <QFile>
#include <QtEndian>

int AVEncoder::test(void)
{
    QByteArray audioData;
    QFile file("./20260725-083541.pcm");

    if (file.open(QIODevice::ReadOnly)) {
        audioData = file.readAll();
        file.close();
    }

    // 创建编码器
    AVEncoder encoder("123.mp4",
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

    int count = 300;

    // 每帧数据个数
    const double samplesPerFrameDouble =
        static_cast<double>(44100) / 30;
    const int samplesPerFrame =
        static_cast<int>(std::round(samplesPerFrameDouble));

    // const int samplesPerFrame = 1024;
    // double    videTime = count / 30.0;                  // 视频时长
    // double oneAudioTime = samplesPerFrame / 44100.0; // 每次的音频时长
    // int audioCount = videTime / oneAudioTime;           // 同步视频时长的音频生成次数

    int audioSize = count * samplesPerFrame * 2;
    QVector<int16_t> audioData16;
    {
        int size16 = audioData.size() / 2 - audioData.size() % 2;

        for (int i = 0; i < size16; i++) {
            int16_t value = *(int16_t *)(audioData.data() + i * 2);

            // value = qToBigEndian(value);
            audioData16.append(value);
        }

        if (size16 < audioSize) {
            int needcount = audioSize - size16;

            if (needcount % 2) needcount += 1;
            needcount = needcount / 2;

            int   index = 0;
            float w;

            for (int i = 0; i < needcount; i++) {
                // w = 2 * 3.14159 * 440 * i / 44100;

                w = 2 * 3.14159 * 440 * index / 44100;

                if (index < samplesPerFrame - 1) {
                    index++;
                } else {
                    index = 0;
                }
                int16_t sample =
                    static_cast<int16_t>(32767 * sin(w));
                audioData16.append(sample);
                audioData16.append(sample);
            }
        }
    }

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
        std::vector<int16_t> audio(samplesPerFrame * 2); // 立体声

        for (int j = 0; j < samplesPerFrame; ++j) {
            // int16_t sample =static_cast<int16_t>
            // (32767 * sin(2 * 3.14159 * 440 * j / 44100));
            // audio[j * 2] = sample;
            // audio[j * 2 + 1] = sample;
            audio[j * 2] = audioData16[i * samplesPerFrame * 2 + j * 2];
            audio[j * 2 + 1] = audioData16[i * samplesPerFrame * 2 + j * 2 + 1];
        }

        if (!encoder.encodeAudio(audio.data(), samplesPerFrame)) {
            qCritical() << "Audio encoding failed";
            break;
        }
    }

    encoder.flush();
    qDebug() << "Direct Encoding finished.";

    ///////////////////////////////////////////////////////////////////
    // 测试线程版本
    qDebug() << "Starting Thread Test...";
    AVEncoderThread *avent = new AVEncoderThread("./123_thread.mp4",
                                                 640, 480, 30,
                                                 44100, 2,
                                                 0, 30, 8);
    bool isfinish = false;

    auto finishLambda =
        [ = ]()mutable {
            qDebug() << "Thread Finish Signal Received";
            isfinish = true;
            avent->quit();
        };

    auto finishedLambda =
        [&]() {
            qDebug();
            isfinish = true;
            avent->deleteLater();
            avent = nullptr;
        };
    QObject::connect(avent, &AVEncoderThread::finish,   finishLambda);
    QObject::connect(avent, &AVEncoderThread::finished, finishedLambda);
    QObject::connect(avent, &AVEncoderThread::error,    [](const QString& msg)
    {
        qCritical() << "Thread Error:" << msg;
    });

    avent->usedUpdate(false);
    avent->start();

    for (int i = 0; i < count; ++i) {
        if (!avent) break;

        // 注意：QImage 不是线程安全的，如果在主线程修改 frame 同时子线程读取，需要拷贝
        // 这里 addImage 内部做了 append，如果是 usedUpdate(false)，它是拷贝进 List 的，所以安全
        frame.fill(Qt::black);
        painter.setBrush(Qt::red);
        painter.translate(320, 240);
        painter.rotate(angle);
        painter.drawRect(-100, -100, 200, 200);
        painter.resetTransform();
        angle += 2;

        avent->addImage(frame.copy()); // 显式拷贝以确保安全

        // 模拟音频数据
        std::vector<int16_t> audio(samplesPerFrame * 2);

        for (int j = 0; j < samplesPerFrame; ++j) {
            // int16_t sample =static_cast<int16_t>
            // (32767 * sin(2 * 3.14159 * 440 * j / 44100));
            // audio[j * 2] = sample;
            // audio[j * 2 + 1] = sample;
            audio[j * 2] = audioData16[i * samplesPerFrame * 2 + j * 2];
            audio[j * 2 + 1] = audioData16[i * samplesPerFrame * 2 + j * 2 + 1];
        }
        avent->addAudio(audio);
    }

    // QThread::sleep(2);

    // if (avent) avent->destory();
    if (avent) avent->stop();

    qDebug() << "Waiting for thread to finish...";

    while (!isfinish) {
        QThread::msleep(10);
    }

    qDebug() << "Thread Encoding finished.";
    return 0;
}
