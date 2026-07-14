#include "ffmpegscreen.h"
#include <QDebug>
#include <QTime>
#include <QImage>
#define qdebug qDebug().noquote()

ffmpegScreen::~ffmpegScreen()
{
    stopwork();
}

void ffmpegScreen::initparam(QString url,
                             QString framerate,
                             QString xoffset,
                             QString yoffset,
                             QString size,
                             bool    isMouse)
{
    m_url = url;
    m_framerate = framerate;
    m_xoffset = xoffset;
    m_yoffset = yoffset;
    m_size = size;
    m_isMouse = isMouse;
}

bool ffmpegScreen::isinit()
{
    if (m_formatContext) return true;
    else return false;
}

void ffmpegScreen::run()
{
    while (isrunning) {
        if (isinit() == false) {
            QThread::msleep(500);
            init();
            continue;
        }
        QImage image;

        // QTime  t = QTime::currentTime();

        if (getImage(image)) {
            // qdebug << "Got frame:" << image.size();
            emit gotFrame(image);
        } else {
            // qdebug << "Failed to get frame";
        }

        // qdebug << t.msecsTo(QTime::currentTime());
    }
}

ffmpegScreen::ffmpegScreen(QObject *parent)
    : QThread{parent}
{
    // init();
}

bool ffmpegScreen::init()
{
    if (isinit()) return true;

    if (avformat_network_init() < 0) {
        qdebug << "avformat_network_init failed";
        return false;
    }
    avdevice_register_all();

    // linux:x11grab ; DXGI:ddagrab
    const AVInputFormat *m_inputFormat = av_find_input_format("gdigrab");

    if (m_inputFormat == nullptr) {
        qdebug << "av_find_input_format failed";
        return false;
    }

    AVDictionary *dict = nullptr;
    av_dict_set(&dict, "framerate",  m_framerate.toUtf8().data(), 0);
    av_dict_set(&dict, "video_size", m_size.toUtf8().data(),      0);
    av_dict_set(&dict, "offset_x",   m_xoffset.toUtf8().data(),   0);
    av_dict_set(&dict, "offset_y",   m_yoffset.toUtf8().data(),   0);

    if (!m_isMouse) {
        // 不显示鼠标
        av_dict_set(&dict, "draw_mouse", "0", 0);
    }


    if (int err = avformat_open_input(&m_formatContext,
                                      m_url.toUtf8().data(),
                                      m_inputFormat,
                                      &dict) != 0) {
        char errbuf[1024];
        qdebug << "avformat_open_input failed"
               << av_make_error_string(errbuf, sizeof(errbuf), AVERROR(err));
        av_dict_free(&dict);
        return false;
    }
    av_dict_free(&dict);

    if (avformat_find_stream_info(m_formatContext, nullptr) < 0) {
        qdebug << "Failed to find stream info";
        avformat_close_input(&m_formatContext);
        m_formatContext = nullptr;
        return false;
    }
    qdebug << "m_formatContext->nb_streams:" << m_formatContext->nb_streams
           << m_formatContext->streams[0]->codecpar->codec_type;

    videoStreamIndex = -1;

    for (unsigned int i = 0; i < m_formatContext->nb_streams; ++i) {
        if (m_formatContext->streams[i]->codecpar->codec_type ==
            AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex < 0) {
        qdebug << "Failed to find video stream";
        avformat_close_input(&m_formatContext);
        m_formatContext = nullptr;
        return false;
    }

    AVCodecParameters *codecPar =
        m_formatContext->streams[videoStreamIndex]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(codecPar->codec_id);

    if (!codec) {
        qdebug << "Failed to find decoder";
        avformat_close_input(&m_formatContext);
        m_formatContext = nullptr;
        return false;
    }
    codecCtx = avcodec_alloc_context3(codec);

    if (codecCtx == nullptr) {
        qdebug << "Failed to allocate codec context";
        avformat_close_input(&m_formatContext);
        m_formatContext = nullptr;
        return false;
    }

    if (avcodec_parameters_to_context(codecCtx, codecPar) < 0) {
        qdebug << "Failed to copy codec parameters to context";
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        m_formatContext = nullptr;
        return false;
    }

    if (avcodec_open2(codecCtx, codec, nullptr) < 0) {
        qdebug << "Failed to open codec";
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        m_formatContext = nullptr;
        return false;
    }
    packet = av_packet_alloc();

    if (packet == nullptr) {
        qdebug << "Failed to allocate packet";
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        m_formatContext = nullptr;
        return false;
    }
    frame = av_frame_alloc();

    if (frame == nullptr) {
        qdebug << "Failed to allocate frame";
        av_packet_free(&packet);
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        m_formatContext = nullptr;
        return false;
    }
    rgbFrame = av_frame_alloc();

    if (rgbFrame == nullptr) {
        qdebug << "Failed to allocate rgbFrame";
        av_frame_free(&frame);
        av_packet_free(&packet);
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        m_formatContext = nullptr;
        return false;
    }
    swsCtx = sws_getContext(codecCtx->width,
                            codecCtx->height,
                            codecCtx->pix_fmt,
                            codecCtx->width,
                            codecCtx->height,
                            AV_PIX_FMT_RGB24, // AV_PIX_FMT_YUV420P,
                            SWS_BILINEAR,
                            nullptr,
                            nullptr,
                            nullptr);

    if (swsCtx == nullptr) {
        qdebug << "Failed to initialize swsCtx";
        av_frame_free(&rgbFrame);
        av_frame_free(&frame);
        av_packet_free(&packet);
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        m_formatContext = nullptr;
        return false;
    }

    rgbBufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGB24,
                                             codecCtx->width,
                                             codecCtx->height,
                                             1);
    rgbBuffer = (uint8_t *)av_malloc(rgbBufferSize);

    av_image_fill_arrays(rgbFrame->data,
                         rgbFrame->linesize,
                         rgbBuffer,
                         AV_PIX_FMT_RGB24,
                         codecCtx->width,
                         codecCtx->height,
                         1);

    // 使用 avcodec_send_packet / avcodec_receive_frame 解码
    // avcodec_send_packet(m_formatContext, nullptr);
    // 再用 sws_scale()转换为编码器支持的格式
    // 调用 avcodec_send_frame / avcodec_receive_packet 写入 MP4 文件
    // avformat_close_input(&m_formatContext);

    qDebug() << "url=" << m_url
             << "framerate=" << m_framerate
             << "xoffset=" << m_xoffset
             << "yoffset=" << m_yoffset
             << "video_size=" << m_size;

    return true;
}

void ffmpegScreen::startwork()
{
    if (isrunning) return;

    isrunning = true;
    start();
}

void ffmpegScreen::stopwork()
{
    isrunning = false;
    quit();
    wait();

    if (m_formatContext) {
        av_free(rgbBuffer);
        sws_freeContext(swsCtx);
        av_frame_free(&rgbFrame);
        av_frame_free(&frame);
        av_packet_free(&packet);
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        m_formatContext = nullptr;
    }
}

bool ffmpegScreen::getImage(QImage& image)
{
    bool gotFrame = false;

    if (isinit() == false) {
        qdebug << "ffmpegScreen is not initialized";
        return false;
    }

    // QTime t = QTime::currentTime();

    // qdebug << t.msecsTo(QTime::currentTime());

    // 读1帧
    // while (av_read_frame(m_formatContext, packet) >= 0) {
    if (av_read_frame(m_formatContext, packet) >= 0) {
        if (packet->stream_index == videoStreamIndex) {
            // avcodec_send_frame:送入要编码的AVFrame，
            //      AVFrame的格式必须与编码器的AVCodecContext设置的相同
            // avcodec_send_packet:将原始数据包作为输入给解码器
            if (avcodec_send_packet(codecCtx, packet) == 0) {
                // while (avcodec_receive_frame(codecCtx, frame) == 0)
                int ret = avcodec_receive_frame(codecCtx, frame);

                if (ret == 0)
                {
                    // 转换为RGB格式
                    sws_scale(swsCtx,
                              frame->data,
                              frame->linesize,
                              0,
                              codecCtx->height,
                              rgbFrame->data,
                              rgbFrame->linesize);

                    // 创建QImage
                    image = QImage(rgbFrame->data[0],
                                   codecCtx->width,
                                   codecCtx->height,
                                   rgbFrame->linesize[0],
                                   QImage::Format_RGB888);
                    image = image.copy(image.rect());
                    gotFrame = true;
                } else {
                    if (ret == AVERROR(EAGAIN)) {
                        // 在此状态下输出不可用,用户必须尝试发送新的输入
                    } else if (ret == AVERROR_EOF) {
                        // 编解码器已经被完全刷新了，以后不再输出帧
                    }  else if (ret == AVERROR(EINVAL)) {
                        // 编解码器未打开
                    } else if (ret < 0) {
                        // 此时一般意味着程序出bug了，多半是数据或编码参数有问题，或者OOM了，解bug吧......
                        // 无法继续进行编码
                    }
                }
            }
        }
        av_packet_unref(packet);
    }

    // qdebug << t.msecsTo(QTime::currentTime());
    return gotFrame;
}

void ffmpegScreen::info()
{
    qdebug << "avcodec_version: " << avcodec_version()
           << QString("(%1.%2.%3)")
        .arg(AV_VERSION_MAJOR(avcodec_version()))
        .arg(AV_VERSION_MINOR(avcodec_version()))
        .arg(AV_VERSION_MICRO(avcodec_version()));
    qdebug << "avutil_version: " << avutil_version()
           << QString("(%1.%2.%3)")
        .arg(AV_VERSION_MAJOR(avutil_version()))
        .arg(AV_VERSION_MINOR(avutil_version()))
        .arg(AV_VERSION_MICRO(avutil_version()));
    qdebug << "avutil_license: " <<   avutil_license();
    qdebug << "av_version: " <<       av_version_info();
    qdebug << "av_license: " <<       avutil_license();
    qdebug << "av_configuration: " << avutil_configuration();

    QString qstr;
#if 0
    qstr = "codec list: ";

    for (int i = AV_CODEC_ID_NONE; i <= AV_CODEC_ID_ANULL; i++) {
        if (!avcodec_find_encoder((AVCodecID)i)) continue;
        const char *p = avcodec_get_name((AVCodecID)i);

        if ((strcmp("unknown_codec", p) != 0) && (strcmp("none", p) != 0)) {
            const AVCodec *avcode = avcodec_find_encoder_by_name(p);

            if (avcode) {
                qstr += QString("<%1>").arg(p);
            }
        }
    }
    qdebug << qstr;

    qstr =  "没有找到: ";

    for (int i = AV_CODEC_ID_NONE; i <= AV_CODEC_ID_ANULL; i++) {
        if (!avcodec_find_encoder((AVCodecID)i)) continue;
        const char *p = avcodec_get_name((AVCodecID)i);
        const AVCodec *avcode = avcodec_find_encoder_by_name(p);

        if (!avcode) {
            qstr += QString("<%1>").arg(p);
        }
    }
    qdebug << qstr;
#endif // if 0
    qstr = "av_demuxer_iterate list:\n";

    void *opaque = NULL; // 1. 初始化 opaque 为 NULL
    const AVInputFormat *fmt = NULL;

    // 2. 循环迭代，所有解码器,直到 av_demuxer_iterate 返回 NULL
    while ((fmt = av_demuxer_iterate(&opaque))) {
        // 3. 使用获取到的 AVInputFormat 指针
        // fmt->name 是解复用器的短名称，如 "mp4", "mov"
        // fmt->long_name 是描述性名称
        qstr += QString("<%1> : %2\n").arg(fmt->name).arg(fmt->long_name);
    }
    qdebug << qstr;

    qstr =  "av_muxer_iterate list:\n";

    opaque = NULL; // 1. 初始化 opaque 为 NULL
    const AVOutputFormat *fmto = NULL;

    // 2. 循环迭代，所有编码器,直到 av_muxer_iterate 返回 NULL
    while ((fmto = av_muxer_iterate(&opaque))) {
        // 3. 使用获取到的 AVOutputFormat 指针
        // fmt->name 是解复用器的短名称，如 "mp4", "mov"
        // fmt->long_name 是描述性名称
        qstr += QString("<%1> : %2\n").arg(fmto->name).arg(fmto->long_name);
    }
    qdebug << qstr;
}

//////////////////////////////////////////////////////

AV1Encoder::AV1Encoder(const QString& outputFile,
                       int            width,
                       int            height,
                       int            fps,
                       int            crf,
                       int            preset)
    : m_outputFile(outputFile)
    , m_width(width)
    , m_height(height)
    , m_fps(fps)
    , m_crf(crf)
    , m_preset(preset)
    , m_formatCtx(nullptr)
    , m_codecCtx(nullptr)
    , m_stream(nullptr)
    , m_swsCtx(nullptr)
    , m_frame(nullptr)
    , m_packet(nullptr)
    , m_frameIndex(0)
    , m_isInitialized(false)
{}

AV1Encoder::~AV1Encoder()
{
    flush();
    close();
    qDebug();
}

bool AV1Encoder::encodeFrame(const QImage& image)
{
    if (!m_isInitialized) {
        if (!init()) {
            qWarning() << "编码器未初始化";
            return false;
        }
    }

    // 确保图像尺寸匹配
    if ((image.width() != m_width) || (image.height() != m_height)) {
        qWarning() << "图像尺寸与视频尺寸不匹配";
        return false;
    }

    // 将 QImage 转换为 BGRA 数据（FFmpeg 的 AV_PIX_FMT_BGRA 与 QImage::Format_ARGB32
    // 内存布局一致）
    QImage converted = image;

    if (converted.format() != QImage::Format_ARGB32) {
        converted = converted.convertToFormat(QImage::Format_ARGB32);

        if (converted.isNull()) {
            qWarning() << "图像格式转换失败";
            return false;
        }
    }

    // 使用 sws_scale 将 BGRA 转换为 YUV420P，直接写入 m_frame 的缓冲区
    const uint8_t *srcData[4] = { converted.bits() };
    int srcLinesize[4] = { static_cast<int>(converted.bytesPerLine()) };
    sws_scale(m_swsCtx, srcData, srcLinesize, 0, m_height,
              m_frame->data, m_frame->linesize);

    // 设置时间戳（递增）
    m_frame->pts = m_frameIndex++;

    // 发送帧到编码器
    int ret = avcodec_send_frame(m_codecCtx, m_frame);

    if (ret < 0) {
        av_strerror(ret, errbuf, sizeof(errbuf));
        qWarning() << "发送帧到编码器失败: " << errbuf; // av_err2str(ret)
        return false;
    }

    // 接收编码后的数据包（可能一次 send 产生多个包，循环接收）
    while (true) {
        ret = avcodec_receive_packet(m_codecCtx, m_packet);

        if ((ret == AVERROR(EAGAIN)) || (ret == AVERROR_EOF)) {
            break; // 需要更多帧或已结束
        } else if (ret < 0) {
            av_strerror(ret, errbuf, sizeof(errbuf));
            qWarning() << "接收编码包失败: " << errbuf;
            return false;
        }

        // 将包写入容器
        av_packet_rescale_ts(m_packet,
                             m_codecCtx->time_base,
                             m_stream->time_base);
        m_packet->stream_index = m_stream->index;
        ret = av_interleaved_write_frame(m_formatCtx, m_packet);
        av_packet_unref(m_packet);

        if (ret < 0) {
            av_strerror(ret, errbuf, sizeof(errbuf));
            qWarning() << "写入包失败: " << errbuf;
            return false;
        }
    }

    return true;
}

bool AV1Encoder::flush()
{
    if (!m_isInitialized) return true;

    // 发送 NULL 帧，告知编码器结束
    int ret = avcodec_send_frame(m_codecCtx, nullptr);

    if (ret < 0) {
        av_strerror(ret, errbuf, sizeof(errbuf));
        qWarning() << "发送 flush 帧失败: " << errbuf;
        return false;
    }

    // 接收所有剩余包
    while (true) {
        ret = avcodec_receive_packet(m_codecCtx, m_packet);

        if ((ret == AVERROR(EAGAIN)) || (ret == AVERROR_EOF)) {
            break;
        } else if (ret < 0) {
            av_strerror(ret, errbuf, sizeof(errbuf));
            qWarning() << "接收剩余包失败: " << errbuf;
            return false;
        }

        av_packet_rescale_ts(m_packet,
                             m_codecCtx->time_base,
                             m_stream->time_base);
        m_packet->stream_index = m_stream->index;
        av_interleaved_write_frame(m_formatCtx, m_packet);
        av_packet_unref(m_packet);
    }

    // 写入文件尾
    av_write_trailer(m_formatCtx);

    // 释放资源
    close();
    return true;
}

void AV1Encoder::close()
{
    if (m_packet) {
        av_packet_free(&m_packet);
        m_packet = nullptr;
    }

    if (m_frame) {
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }

    if (m_swsCtx) {
        sws_freeContext(m_swsCtx);
        m_swsCtx = nullptr;
    }

    if (m_codecCtx) {
        avcodec_free_context(&m_codecCtx);
        m_codecCtx = nullptr;
    }

    if (m_formatCtx) {
        if (m_formatCtx->pb) {
            avio_closep(&m_formatCtx->pb);
        }
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
    }
    m_isInitialized = false;
}

bool AV1Encoder::init()
{
    if (m_isInitialized) return true;

    // 1. 注册所有组件（FFmpeg 4.0+ 可省略，但保留兼容）
    avdevice_register_all();
    avformat_network_init();

    // 2. 查找 AV1 编码器（优先 libsvtav1）
    const AVCodec *codec = avcodec_find_encoder_by_name("libsvtav1");

    if (!codec) {
        codec = avcodec_find_encoder_by_name("libaom-av1");
    }

    if (!codec) {
        qWarning() << "未找到 AV1 编码器，请确保 FFmpeg 编译时启用了 libsvtav1 或 libaom-av1";
        return false;
    }

    // 3. 分配编码器上下文
    m_codecCtx = avcodec_alloc_context3(codec);

    if (!m_codecCtx) {
        qWarning() << "无法分配编码器上下文";
        return false;
    }

    // 4. 设置编码参数
    m_codecCtx->width = m_width;
    m_codecCtx->height = m_height;
    m_codecCtx->time_base = AVRational{ 1, m_fps };
    m_codecCtx->framerate = AVRational{ m_fps, 1 };
    m_codecCtx->pix_fmt = AV_PIX_FMT_YUV420P; // AV1 常用
    m_codecCtx->thread_count = 0;             // 自动多线程

    // 设置 CRF 质量控制
    m_codecCtx->flags |= AV_CODEC_FLAG_QSCALE;
    m_codecCtx->global_quality = m_crf * FF_QP2LAMBDA;

    // 设置 preset（通过 AVOption）
    av_opt_set(m_codecCtx->priv_data,
               "preset",
               QString::number(m_preset).toUtf8().constData(),
               0);

    // 针对 libaom-av1 的额外兼容
    if (strcmp(codec->name, "libaom-av1") == 0) {
        // 旧版需要 experimental 标志
        m_codecCtx->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
    }

    // 5. 打开编码器
    if (avcodec_open2(m_codecCtx, codec, nullptr) < 0) {
        qWarning() << "无法打开编码器";
        avcodec_free_context(&m_codecCtx);
        return false;
    }

    // 6. 创建输出上下文（容器格式为 MP4）
    avformat_alloc_output_context2(&m_formatCtx,
                                   nullptr,
                                   "mp4",
                                   m_outputFile.toUtf8().constData());

    if (!m_formatCtx) {
        qWarning() << "无法创建 MP4 输出上下文";
        avcodec_free_context(&m_codecCtx);
        return false;
    }

    // 7. 添加视频流
    m_stream = avformat_new_stream(m_formatCtx, codec);

    if (!m_stream) {
        qWarning() << "无法创建视频流";
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        avcodec_free_context(&m_codecCtx);
        return false;
    }
    m_stream->id = m_formatCtx->nb_streams - 1;
    m_stream->time_base = m_codecCtx->time_base;

    // 将编码器参数复制到流
    avcodec_parameters_from_context(m_stream->codecpar, m_codecCtx);

    // 8. 打开输出文件并写入头信息
    if (!(m_formatCtx->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&m_formatCtx->pb, m_outputFile.toUtf8().constData(),
                      AVIO_FLAG_WRITE) < 0) {
            qWarning() << "无法打开输出文件 " << m_outputFile;
            avformat_free_context(m_formatCtx);
            m_formatCtx = nullptr;
            avcodec_free_context(&m_codecCtx);
            return false;
        }
    }

    if (avformat_write_header(m_formatCtx, nullptr) < 0) {
        qWarning() << "写入文件头失败";
        avio_closep(&m_formatCtx->pb);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        avcodec_free_context(&m_codecCtx);
        return false;
    }

    // 9. 创建 SwsContext 用于颜色空间转换（BGRA -> YUV420P）
    m_swsCtx = sws_getContext(
        m_width, m_height, AV_PIX_FMT_BGRA,
        m_width, m_height, AV_PIX_FMT_YUV420P,
        SWS_BILINEAR, nullptr, nullptr, nullptr
        );

    if (!m_swsCtx) {
        qWarning() << "无法创建颜色转换上下文";
        close();
        return false;
    }

    // 10. 分配一个 AVFrame 用于存放转换后的 YUV 帧（复用）
    m_frame = av_frame_alloc();

    if (!m_frame) {
        qWarning() << "无法分配 AVFrame";
        close();
        return false;
    }
    m_frame->format = AV_PIX_FMT_YUV420P;
    m_frame->width = m_width;
    m_frame->height = m_height;

    if (av_frame_get_buffer(m_frame, 32) < 0) {
        qWarning() << "无法为 AVFrame 分配缓冲区";
        close();
        return false;
    }

    // 11. 分配 AVPacket（复用）
    m_packet = av_packet_alloc();

    if (!m_packet) {
        qWarning() << "无法分配 AVPacket";
        close();
        return false;
    }

    m_isInitialized = true;
    return true;
}

#include <QPainter>
int AV1Encoder::test()
{
    const int  width = 640, height = 480, fps = 30;
    AV1Encoder encoder("output_av1.mp4", width, height, fps, 30 /* CRF */,
                       8 /* preset */);

    // 模拟生成连续帧（例如旋转的矩形）
    QImage   frame(width, height, QImage::Format_ARGB32);
    QPainter painter(&frame);
    int angle = 0;

    for (int i = 0; i < 300; ++i) {
        frame.fill(Qt::black);
        painter.setBrush(Qt::red);
        painter.translate(width / 2, height / 2);
        painter.rotate(angle);
        painter.drawRect(-100, -100, 200, 200);
        painter.resetTransform();
        angle += 2;

        if (!encoder.encodeFrame(frame)) {
            break;
        }
    }

    encoder.flush();
    return 0;
}
