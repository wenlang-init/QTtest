#include "ffmpegvideo.h"
#ifdef __cplusplus
extern "C" {
// # include <libavutil/avutil.h>
# include <libswscale/swscale.h>
# include <libavcodec/avcodec.h>
# include <libavformat/avformat.h>
# include <libavdevice/avdevice.h>
# include <libavutil/imgutils.h>
# include <libavutil/pixdesc.h>
}
#endif // ifdef __cplusplus
#include <QDebug>
#include <QTime>
#include <QImage>
#define qdebug qDebug().noquote()

ffmpegVideo::ffmpegVideo(QObject *parent)
    : QThread{parent}
{}

ffmpegVideo::~ffmpegVideo()
{
    stopwork();
}

void ffmpegVideo::restartwork(QString url)
{
    stopwork();
    m_url = url;
    isrunning = true;
    start();
}

void ffmpegVideo::stopwork()
{
    isrunning = false;
    quit();
    wait();
    pause = false;
}

bool ffmpegVideo::init()
{
    // if (avformat_network_init() < 0) {
    //     qdebug << "avformat_network_init failed";
    //     return false;
    // }
    // avdevice_register_all(); // 注册 FFmpeg 的所有组件。在 4.0 版本以后已经被弃用

    AVFormatContext *m_formatContext = nullptr;

    char errbuf[1024];
    QString errorinfo;

    // 打开媒体文件，并获得解封装上下文
    int ret = avformat_open_input(&m_formatContext,
                                  m_url.toStdString().c_str(),
                                  nullptr,
                                  nullptr);

    if (ret != 0) {
        errorinfo =  "avformat_open_input failed"
                    + QString(av_make_error_string(errbuf, sizeof(errbuf),
                                                   AVERROR(ret)));
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return false;
    }

    // 探测获取流信息
    ret = avformat_find_stream_info(m_formatContext, nullptr);

    if (ret < 0) {
        errorinfo =  "Failed to find stream info"
                    + QString(av_make_error_string(errbuf, sizeof(errbuf),
                                                   AVERROR(ret)));
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return false;
    }
    qdebug << "m_formatContext->nb_streams:" << m_formatContext->nb_streams
           << m_formatContext->streams[0]->codecpar->codec_type;

    int videoStreamIndex = -1;

    for (unsigned int i = 0; i < m_formatContext->nb_streams; ++i) {
        if (m_formatContext->streams[i]->codecpar->codec_type ==
            AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex < 0) {
        errorinfo = "Failed to find video stream";
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return false;
    }

    AVCodecParameters *codecPar =
        m_formatContext->streams[videoStreamIndex]->codecpar;

    // 查找解码器
    const AVCodec *codec = avcodec_find_decoder(codecPar->codec_id);

    if (!codec) {
        errorinfo = "Failed to find decoder"
                    + QString(av_make_error_string(errbuf, sizeof(errbuf),
                                                   AVERROR(errno)));
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return false;
    }

    // 创建并初始化解码器上下文
    AVCodecContext *codecCtx = avcodec_alloc_context3(codec);

    if (codecCtx == nullptr) {
        errorinfo = "Failed to allocate codec context"
                    + QString(av_make_error_string(errbuf, sizeof(errbuf),
                                                   AVERROR(errno)));
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return false;
    }

    // int audioStreamIndex = -1;
    // for (unsigned int i = 0; i < m_formatContext->nb_streams; ++i) {
    //     if (m_formatContext->streams[i]->codecpar->codec_type ==
    //         AVMEDIA_TYPE_AUDIO) {
    //         audioStreamIndex = i;
    //         break;
    //     }
    // }
    // const AVCodec *codecaudio = nullptr;

    // if (audioStreamIndex != -1) {
    //     codecParaudio = m_formatContext->streams[audioStreamIndex]->codecpar;
    //     codecaudio = avcodec_find_decoder(codecParaudio->codec_id);

    //     if (codecaudio) {
    //         AVCodecContext *codecCtxaudio =
    // avcodec_alloc_context3(codecaudio);

    //         if (codecCtxaudio) {}
    //     }
    // }

    // 将解码器参数从 AVCodecParameters 复制到 AVCodecContext
    ret = avcodec_parameters_to_context(codecCtx, codecPar);

    if (ret < 0) {
        errorinfo = "Failed to copy codec parameters to context"
                    + QString(av_make_error_string(errbuf, sizeof(errbuf),
                                                   AVERROR(ret)));
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return false;
    }

    // 打开解码器(初始化一个音视频编解码器的 AVCodecContext)
    ret = avcodec_open2(codecCtx, codec, nullptr);

    if (ret < 0) {
        errorinfo = "Failed to open codec"
                    + QString(av_make_error_string(errbuf, sizeof(errbuf),
                                                   AVERROR(ret)));
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return false;
    }

    AVPacket *packet = av_packet_alloc();

    if (packet == nullptr) {
        errorinfo = "Failed to allocate packet";
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return false;
    }

    AVFrame *frame = av_frame_alloc();

    if (frame == nullptr) {
        errorinfo = "Failed to allocate frame";
        av_packet_free(&packet);
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////
    QString info;

    // 时长
    info = QString::number(m_formatContext->duration / 1000000.0, 'f', 2) + "s, ";

    // 流数量
    info +=  QString::number(m_formatContext->nb_streams) + " streams, ";

    // 视频像素格式
    info += QString(av_get_pix_fmt_name((AVPixelFormat)codecPar->format));

    // 比特率
    info += ", bitrate: " + QString::number(codecPar->bit_rate / 1000) +
            " kbps, ";

    // 每个样本的比特数
    info += ", bits_per_coded_sample: " + QString::number(
        codecPar->bits_per_coded_sample) + " bits, ";

    // 每个原始样本的比特数
    info += ", bits_per_raw_sample: " + QString::number(
        codecPar->bits_per_raw_sample) + " bits, ";

    // 分辨率
    info += QString::number(codecPar->width) + "x" + QString::number(
        codecPar->height);

    // 帧率
    AVRational frame_rate = av_guess_frame_rate(m_formatContext,
                                                m_formatContext->streams[
                                                    videoStreamIndex],
                                                nullptr);
    info += ", frame rate: " + QString::number(frame_rate.num) + "/" +
            QString::number(frame_rate.den) + " fps";

    // 编解码器名称
    info += ", codec: " + QString(avcodec_get_name(codecPar->codec_id));

    // 音频采样率
    info += ", sample rate: " + QString::number(codecPar->sample_rate) + " Hz, ";

    emit videoInfo(info);

    ////////////////////////////////////////////////////////////////////////////
    AVFrame *rgbFrame = av_frame_alloc();

    if (rgbFrame == nullptr) {
        errorinfo = "Failed to allocate rgbFrame";
        av_frame_free(&frame);
        av_packet_free(&packet);
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return false;
    }

    // 分配SwsContext,使用sws_scale()函数进行图像格式转换
    SwsContext *swsCtx = sws_getContext(codecCtx->width,
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
        errorinfo = "Failed to initialize swsCtx";
        av_frame_free(&rgbFrame);
        av_frame_free(&frame);
        av_packet_free(&packet);
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return false;
    }

    int rgbBufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGB24,
                                                 codecCtx->width,
                                                 codecCtx->height,
                                                 1);
    uint8_t *rgbBuffer = (uint8_t *)av_malloc(rgbBufferSize);

    av_image_fill_arrays(rgbFrame->data,
                         rgbFrame->linesize,
                         rgbBuffer,
                         AV_PIX_FMT_RGB24,
                         codecCtx->width,
                         codecCtx->height,
                         1);

    av_free(rgbBuffer);
    av_frame_free(&rgbFrame);
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codecCtx);
    avformat_close_input(&m_formatContext);

    return true;
}

void ffmpegVideo::run()
{
    // if (avformat_network_init() < 0) {
    //     qdebug << "avformat_network_init failed";
    //     return false;
    // }
    // avdevice_register_all(); // 注册 FFmpeg 的所有组件。在 4.0 版本以后已经被弃用

    AVFormatContext *m_formatContext = nullptr;

    char errbuf[1024];
    QString errorinfo;

    // 打开媒体文件，并获得解封装上下文
    int ret = avformat_open_input(&m_formatContext,
                                  m_url.toStdString().c_str(),
                                  nullptr,
                                  nullptr);

    if (ret != 0) {
        errorinfo =  "avformat_open_input failed"
                    + QString(av_make_error_string(errbuf, sizeof(errbuf),
                                                   AVERROR(ret)));
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return;
    }

    // 探测获取流信息
    ret = avformat_find_stream_info(m_formatContext, nullptr);

    if (ret < 0) {
        errorinfo =  "Failed to find stream info"
                    + QString(av_make_error_string(errbuf, sizeof(errbuf),
                                                   AVERROR(ret)));
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return;
    }
    qdebug << "m_formatContext->nb_streams:" << m_formatContext->nb_streams
           << m_formatContext->streams[0]->codecpar->codec_type;

    int videoStreamIndex = -1;

    for (unsigned int i = 0; i < m_formatContext->nb_streams; ++i) {
        if (m_formatContext->streams[i]->codecpar->codec_type ==
            AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex < 0) {
        errorinfo = "Failed to find video stream";
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return;
    }

    AVCodecParameters *codecPar =
        m_formatContext->streams[videoStreamIndex]->codecpar;

    // 查找解码器
    const AVCodec *codec = avcodec_find_decoder(codecPar->codec_id);

    if (!codec) {
        errorinfo = "Failed to find decoder"
                    + QString(av_make_error_string(errbuf, sizeof(errbuf),
                                                   AVERROR(errno)));
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return;
    }

    // 创建并初始化解码器上下文
    AVCodecContext *codecCtx = avcodec_alloc_context3(codec);

    if (codecCtx == nullptr) {
        errorinfo = "Failed to allocate codec context"
                    + QString(av_make_error_string(errbuf, sizeof(errbuf),
                                                   AVERROR(errno)));
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return;
    }

    // int audioStreamIndex = -1;
    // for (unsigned int i = 0; i < m_formatContext->nb_streams; ++i) {
    //     if (m_formatContext->streams[i]->codecpar->codec_type ==
    //         AVMEDIA_TYPE_AUDIO) {
    //         audioStreamIndex = i;
    //         break;
    //     }
    // }
    // const AVCodec *codecaudio = nullptr;

    // if (audioStreamIndex != -1) {
    //     codecParaudio = m_formatContext->streams[audioStreamIndex]->codecpar;
    //     codecaudio = avcodec_find_decoder(codecParaudio->codec_id);

    //     if (codecaudio) {
    //         AVCodecContext *codecCtxaudio =
    // avcodec_alloc_context3(codecaudio);

    //         if (codecCtxaudio) {}
    //     }
    // }

    // 将解码器参数从 AVCodecParameters 复制到 AVCodecContext
    ret = avcodec_parameters_to_context(codecCtx, codecPar);

    if (ret < 0) {
        errorinfo = "Failed to copy codec parameters to context"
                    + QString(av_make_error_string(errbuf, sizeof(errbuf),
                                                   AVERROR(ret)));
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return;
    }

    // 打开解码器(初始化一个音视频编解码器的 AVCodecContext)
    ret = avcodec_open2(codecCtx, codec, nullptr);

    if (ret < 0) {
        errorinfo = "Failed to open codec"
                    + QString(av_make_error_string(errbuf, sizeof(errbuf),
                                                   AVERROR(ret)));
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return;
    }

    AVPacket *packet = av_packet_alloc();

    if (packet == nullptr) {
        errorinfo = "Failed to allocate packet";
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return;
    }

    AVFrame *frame = av_frame_alloc();

    if (frame == nullptr) {
        errorinfo = "Failed to allocate frame";
        av_packet_free(&packet);
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return;
    }

    /////////////////////////////////////////////////////////////////////////
    QString info;

    // 时长
    info = QString::number(m_formatContext->duration / 1000000.0, 'f', 2) + "s, ";

    // 流数量
    info +=  QString::number(m_formatContext->nb_streams) + " streams, ";

    // 视频像素格式
    info += QString(av_get_pix_fmt_name((AVPixelFormat)codecPar->format));

    // 比特率
    info += ", bitrate: " + QString::number(codecPar->bit_rate / 1000) +
            " kbps, ";

    // 每个样本的比特数
    info += ", bits_per_coded_sample: " + QString::number(
        codecPar->bits_per_coded_sample) + " bits, ";

    // 每个原始样本的比特数
    info += ", bits_per_raw_sample: " + QString::number(
        codecPar->bits_per_raw_sample) + " bits, ";

    // 分辨率
    info += QString::number(codecPar->width) + "x" + QString::number(
        codecPar->height);

    // 帧率
    AVRational frame_rate = av_guess_frame_rate(m_formatContext,
                                                m_formatContext->streams[
                                                    videoStreamIndex],
                                                nullptr);
    info += ", frame rate: " + QString::number(frame_rate.num) + "/" +
            QString::number(frame_rate.den) + " fps";

    // 编解码器名称
    info += ", codec: " + QString(avcodec_get_name(codecPar->codec_id));

    // 音频采样率
    info += ", sample rate: " + QString::number(codecPar->sample_rate) + " Hz, ";

    emit videoInfo(info);

    ////////////////////////////////////////////////////////////////////////////
    AVFrame *rgbFrame = av_frame_alloc();

    if (rgbFrame == nullptr) {
        errorinfo = "Failed to allocate rgbFrame";
        av_frame_free(&frame);
        av_packet_free(&packet);
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return;
    }

    // 分配SwsContext,使用sws_scale()函数进行图像格式转换
    SwsContext *swsCtx = sws_getContext(codecCtx->width,
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
        errorinfo = "Failed to initialize swsCtx";
        av_frame_free(&rgbFrame);
        av_frame_free(&frame);
        av_packet_free(&packet);
        avcodec_free_context(&codecCtx);
        avformat_close_input(&m_formatContext);
        emit errorInfo(errorinfo);
        qdebug << errorinfo;
        return;
    }

    int rgbBufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGB24,
                                                 codecCtx->width,
                                                 codecCtx->height,
                                                 1);
    uint8_t *rgbBuffer = (uint8_t *)av_malloc(rgbBufferSize);

    av_image_fill_arrays(rgbFrame->data,
                         rgbFrame->linesize,
                         rgbBuffer,
                         AV_PIX_FMT_RGB24,
                         codecCtx->width,
                         codecCtx->height,
                         1);

    ///////////////////////////////////////////////////////////////////

    while (isrunning) {
        if (pause) {
            msleep(100);
            continue;
        }

        if (av_read_frame(m_formatContext, packet) < 0) {
            errorinfo = "Failed to read frame"
                        + QString(av_make_error_string(errbuf, sizeof(errbuf),
                                                       AVERROR(errno)));
            emit errorInfo(errorinfo);
            qdebug << errorinfo;
            break;
        }

        if (packet->stream_index == videoStreamIndex) {
            if (avcodec_send_packet(codecCtx, packet) == 0) {
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
                    QImage image = QImage(rgbFrame->data[0],
                                          codecCtx->width,
                                          codecCtx->height,
                                          rgbFrame->linesize[0],
                                          QImage::Format_RGB888);

                    emit gotFrame(image);
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
                        errorinfo = "Failed to decode frame"
                                    + QString(av_make_error_string(errbuf,
                                                                   sizeof(errbuf),
                                                                   AVERROR(ret)));
                        emit errorInfo(errorinfo);
                        qdebug << errorinfo;
                        break;
                    }
                }
            }
        }
        av_packet_unref(packet);
    }

    ///////////////////////////////////////////////////////////////////
    av_free(rgbBuffer);
    av_frame_free(&rgbFrame);
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codecCtx);
    avformat_close_input(&m_formatContext);
}
