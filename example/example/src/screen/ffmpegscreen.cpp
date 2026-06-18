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
            qdebug << "Failed to get frame";
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

    // linux:x11grab
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
    qdebug << "avutil_version: " << avutil_version();
    qdebug << "avutil_license: " <<   avutil_license();
    qdebug << "av_version: " <<       av_version_info();
    qdebug << "av_license: " <<       avutil_license();
    qdebug << "av_configuration: " << avutil_configuration();

    qdebug << "codec list:\n";

    for (int i = AV_CODEC_ID_NONE; i <= AV_CODEC_ID_ANULL; i++) {
        if (!avcodec_find_encoder((AVCodecID)i)) continue;
        const char *p = avcodec_get_name((AVCodecID)i);

        if ((strcmp("unknown_codec", p) != 0) && (strcmp("none", p) != 0)) {
            const AVCodec *avcode = avcodec_find_encoder_by_name(p);

            if (avcode) {
                printf("<%s>", p); fflush(stdout);
            }
        }
    }
    printf(  "\n"); fflush(stdout);
    qdebug << "-----------------------";
    printf("%s\n", QString::fromUtf8("没有找到:").toLocal8Bit().data());
    fflush(stdout);

    for (int i = AV_CODEC_ID_NONE; i <= AV_CODEC_ID_ANULL; i++) {
        if (!avcodec_find_encoder((AVCodecID)i)) continue;
        const char *p = avcodec_get_name((AVCodecID)i);
        const AVCodec *avcode = avcodec_find_encoder_by_name(p);

        if (!avcode) {
            printf("<%s>", p); fflush(stdout);
        }
    }
    printf("\n"); fflush(stdout);

    qdebug << "----------av_demuxer_iterate-------------";

    void *opaque = NULL; // 1. 初始化 opaque 为 NULL
    const AVInputFormat *fmt = NULL;

    // 2. 循环迭代，直到 av_demuxer_iterate 返回 NULL
    while ((fmt = av_demuxer_iterate(&opaque))) {
        // 3. 使用获取到的 AVInputFormat 指针
        // fmt->name 是解复用器的短名称，如 "mp4", "mov"
        // fmt->long_name 是描述性名称
        printf("%s-%s\n", fmt->name, fmt->long_name); fflush(stdout);
    }
    printf("\n"); fflush(stdout);

    qdebug << "----------av_muxer_iterate-------------";

    opaque = NULL; // 1. 初始化 opaque 为 NULL
    const AVOutputFormat *fmto = NULL;

    // 2. 循环迭代，直到 av_muxer_iterate 返回 NULL
    while ((fmto = av_muxer_iterate(&opaque))) {
        // 3. 使用获取到的 AVOutputFormat 指针
        // fmt->name 是解复用器的短名称，如 "mp4", "mov"
        // fmt->long_name 是描述性名称
        printf("%s-%s\n", fmto->name, fmto->long_name); fflush(stdout);
    }
    printf("\n"); fflush(stdout);
}
