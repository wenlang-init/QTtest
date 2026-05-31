#include "ffmpegobject.h"
#include <QDebug>
#include <QFile>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

// 错误信息宏（把FFmpeg的错误码转成文字）
#define ERROR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof(errbuf));


// 辅助函数：检查编码器是否支持指定的采样格式
static int check_sample_fmt(const AVCodec      *codec,
                            enum AVSampleFormat sample_fmt) {
    const enum AVSampleFormat *p = codec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt) return 1;  // 支持

        p++;
    }
    return 0; // 不支持
}

// 核心编码函数：把PCM数据编码成AAC，写入文件
static int encode(AVCodecContext *ctx,
                  AVFrame        *frame,
                  AVPacket       *pkt,
                  QFile         & outFile) {
    // 1. 把PCM帧（frame）发送到编码器
    int ret = avcodec_send_frame(ctx, frame);

    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "发送PCM数据到编码器失败：" << errbuf;
        return ret;
    }

    // 2. 循环从编码器取出编码后的AAC数据
    while (true) {
        ret = avcodec_receive_packet(ctx, pkt);

        // 两种正常情况：需要更多PCM数据 / 编码完成
        if ((ret == AVERROR(EAGAIN)) || (ret == AVERROR_EOF)) {
            return 0;
        } else if (ret < 0) { // 编码出错
            ERROR_BUF(ret);
            qDebug() << "获取AAC数据失败：" << errbuf;
            return ret;
        }

        // 3. 把编码后的AAC数据写入文件
        outFile.write((char *)pkt->data, pkt->size);

        // 4. 释放AAC数据包的资源（FFmpeg的内存必须手动释放！）
        av_packet_unref(pkt);
    }
}

FfmpegObject::FfmpegObject(QObject *parent)
    : QObject{parent}
{}

void FfmpegObject::getInfo()
{
    qDebug() << "avutil_version: " << avutil_version();
    qDebug() << "avutil_license: " <<   avutil_license();
    qDebug() << "av_version: " <<       av_version_info();
    qDebug() << "av_license: " <<       avutil_license();
    qDebug() << "av_configuration: " << avutil_configuration();

    qDebug() << "codec list:\n";

    QString sstr;

    for (int i = AV_CODEC_ID_NONE; i <= AV_CODEC_ID_ANULL; i++) {
        if (!avcodec_find_encoder((AVCodecID)i)) continue;
        const char *p = avcodec_get_name((AVCodecID)i);

        if ((strcmp("unknown_codec", p) != 0) && (strcmp("none", p) != 0)) {
            const AVCodec *avcode = avcodec_find_encoder_by_name(p);

            if (avcode) {
                // printf("<%s>", p); fflush(stdout);
                sstr += QString("<%1>").arg(p);
            }
        }
    }

    // printf("\n"); fflush(stdout);
    sstr += "\n";
    qDebug().noquote() << sstr;
    qDebug() << "-----------------------";
    printf("%s\n", QString::fromUtf8("没有找到:").toLocal8Bit().data());
    fflush(stdout);
    sstr = QString("没有找到:");

    for (int i = AV_CODEC_ID_NONE; i <= AV_CODEC_ID_ANULL; i++) {
        if (!avcodec_find_encoder((AVCodecID)i)) continue;
        const char *p = avcodec_get_name((AVCodecID)i);
        const AVCodec *avcode = avcodec_find_encoder_by_name(p);

        if (!avcode) {
            // printf("<%s>", p); fflush(stdout);
            sstr += QString("<%1>").arg(p);
        }
    }

    // printf("\n"); fflush(stdout);
    sstr += "\n";
    qDebug().noquote() << sstr;

    qDebug() << "----------av_demuxer_iterate-------------";

    void *opaque = NULL; // 1. 初始化 opaque 为 NULL
    const AVInputFormat *fmt = NULL;
    sstr.clear();

    // 2. 循环迭代，直到 av_demuxer_iterate 返回 NULL
    while ((fmt = av_demuxer_iterate(&opaque))) {
        // 3. 使用获取到的 AVInputFormat 指针
        // fmt->name 是解复用器的短名称，如 "mp4", "mov"
        // fmt->long_name 是描述性名称
        // printf("%s-%s\n", fmt->name, fmt->long_name); fflush(stdout);
        sstr += QString("%1-%2\n").arg(fmt->name).arg(fmt->long_name);
    }

    // printf("\n"); fflush(stdout);
    sstr += "\n";
    qDebug().noquote() << sstr;

    qDebug() << "----------av_muxer_iterate-------------";

    opaque = NULL; // 1. 初始化 opaque 为 NULL
    const AVOutputFormat *fmto = NULL;
    sstr.clear();

    // 2. 循环迭代，直到 av_muxer_iterate 返回 NULL
    while ((fmto = av_muxer_iterate(&opaque))) {
        // 3. 使用获取到的 AVOutputFormat 指针
        // fmt->name 是解复用器的短名称，如 "mp4", "mov"
        // fmt->long_name 是描述性名称
        // printf("%s-%s\n", fmto->name, fmto->long_name); fflush(stdout);
        sstr += QString("%1-%2\n").arg(fmto->name).arg(fmto->long_name);
    }

    // printf("\n"); fflush(stdout);
    sstr += "\n";
    qDebug().noquote() << sstr;
}

void FfmpegObject::aacEncode(const QString      & filename,
                             const int            sampleRate,
                             const AVSampleFormat sampleFmt,
                             const int            channels,
                             const QString      & outFilename)
{
    // ===== 第一步：初始化变量 =====
    QFile inFile(filename);          // 输入PCM文件
    QFile outFile(outFilename);      // 输出AAC文件
    int   ret = 0;                   // FFmpeg函数的返回值（小于0就是出错）

    // FFmpeg核心对象（小白记作用途就行）：
    const AVCodec  *codec = nullptr; // 编码器（AAC编码器）
    AVCodecContext *ctx = nullptr;   // 编码器上下文（管理编码器的所有参数）
    AVFrame  *frame = nullptr;       // 存放PCM数据的帧
    AVPacket *pkt = nullptr;         // 存放AAC数据的包

    // ===== 第二步：获取AAC编码器 =====
    // 用libfdk_aac编码器（音质好、兼容性强）
    // codec = avcodec_find_encoder_by_name("libfdk_aac");
    codec = avcodec_find_encoder(AV_CODEC_ID_AAC);

    if (!codec) {
        qDebug() << "找不到AAC编码器！";
        return;
    }

#if 1

    // 检查编码器是否支持我们的PCM采样格式（这里是16位整数）
    if (!check_sample_fmt(codec, sampleFmt)) {
        qDebug() << "编码器不支持该采样格式！";
        return;
    }
#else // if 0
    int avcodec_get_supported_config(const AVCodecContext * avctx,
                                     const AVCodec *codec,
                                     enum AVCodecConfig config,
                                     unsigned flags,
                                     const void **out_configs,
                                     int *out_num_configs);
    const void **out_configs = NULL;
    int number;
    int num_configs = avcodec_get_supported_config(NULL,
                                                   codec,
                                                   AV_CODEC_CONFIG_CHANNEL_LAYOUT,
                                                   0,
                                                   out_configs,
                                                   &number);

    if ((num_configs <= 0) || !out_configs) {
        ERROR_BUF(num_configs);
        qDebug() << "No supported configs found";
        return;
    } else {
        for (int i = 0; i < num_configs; i++) {
            QString str = QString("Config %1: %2").
                          arg(i).arg(((const char **)out_configs)[i]);
            qDebug() << str;
        }
    }
#endif // if 0

    // ===== 第三步：创建并配置编码器上下文 =====
    ctx = avcodec_alloc_context3(codec); // 分配上下文内存

    if (!ctx) {
        qDebug() << "创建编码器上下文失败！";
        return;
    }

    // avcodec_get_supported_config()

    // 设置编码器参数（和输入PCM的参数一致）
    ctx->sample_rate = sampleRate;         // 采样率
    ctx->sample_fmt = sampleFmt;           // 采样格式
    ctx->ch_layout.nb_channels = channels; // 声道布局
    ctx->bit_rate = 32000;                 // 比特率（越高音质越好，文件越大）
    ctx->profile = AV_PROFILE_AAC_HE_V2;   // AAC编码规格（HE-V2是高效压缩）

    // 打开编码器
    ret = avcodec_open2(ctx, codec, nullptr);

    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "打开编码器失败：" << errbuf;
        goto end; // 跳转到结尾释放资源
    }

    // ===== 第四步：创建PCM帧（frame）和AAC包（pkt） =====
    frame = av_frame_alloc(); // 分配frame内存

    if (!frame) {
        qDebug() << "创建PCM帧失败！";
        goto end;
    }

    // 设置frame的参数（和编码器一致）
    frame->nb_samples = ctx->frame_size;                       // 每一帧的样本数（编码器决定）
    frame->format = ctx->sample_fmt;                           // 采样格式
    frame->ch_layout.nb_channels = ctx->ch_layout.nb_channels; // 声道布局
    // 为frame分配实际的内存缓冲区（存放PCM数据）
    ret = av_frame_get_buffer(frame, 0);

    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "为PCM帧分配缓冲区失败：" << errbuf;
        goto end;
    }

    pkt = av_packet_alloc(); // 分配AAC包内存

    if (!pkt) {
        qDebug() << "创建AAC包失败！";
        goto end;
    }

    // ===== 第五步：打开文件 =====
    if (!inFile.open(QFile::ReadOnly)) {
        qDebug() << "打开PCM文件失败：" << filename;
        goto end;
    }

    if (!outFile.open(QFile::WriteOnly)) {
        qDebug() << "打开AAC文件失败：" << outFilename;
        goto end;
    }

    // ===== 第六步：读取PCM数据，编码成AAC =====
    // 循环读取PCM文件数据到frame的缓冲区
    while ((ret = inFile.read((char *)frame->data[0], frame->linesize[0])) > 0) {
        // 如果读取的字节数不够一帧，调整有效样本数（避免编码冗余数据）
        if (ret < frame->linesize[0]) {
            // 每个样本的字节数
            int bytes = av_get_bytes_per_sample((AVSampleFormat)frame->format);

            // 声道数
            int ch = frame->ch_layout.nb_channels; // av_get_channel_layout_nb_channels(frame->channel_layout);

            // 实际有效的样本数
            frame->nb_samples = ret / (bytes * ch);
        }

        // 调用编码函数，把PCM转AAC并写入文件
        if (encode(ctx, frame, pkt, outFile) < 0) {
            goto end;
        }
    }

    // ===== 第七步：刷新编码器（处理最后一批数据） =====
    encode(ctx, nullptr, pkt, outFile);

    // ===== 结尾：释放资源 =====
end:

    // 关闭文件
    inFile.close();
    outFile.close();

    // 释放FFmpeg对象（必须释放，否则内存泄漏）
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&ctx);

    qDebug() << "编码完成！";
}

void FfmpegObject::aacEncode(AudioEncodeSpec& in, const char *outFilename)
{
    // ===== 第一步：初始化变量 =====
    QFile inFile(in.filename);       // 输入PCM文件
    QFile outFile(outFilename);      // 输出AAC文件
    int   ret = 0;                   // FFmpeg函数的返回值（小于0就是出错）

    // FFmpeg核心对象（小白记作用途就行）：
    const AVCodec  *codec = nullptr; // 编码器（AAC编码器）
    AVCodecContext *ctx = nullptr;   // 编码器上下文（管理编码器的所有参数）
    AVFrame  *frame = nullptr;       // 存放PCM数据的帧
    AVPacket *pkt = nullptr;         // 存放AAC数据的包

    // ===== 第二步：获取AAC编码器 =====
    // 用libfdk_aac编码器（音质好、兼容性强）
    codec = avcodec_find_encoder_by_name("libfdk_aac");

    if (!codec) {
        qDebug() << "找不到AAC编码器！";
        return;
    }

#if 0

    // 检查编码器是否支持我们的PCM采样格式（这里是16位整数）
    if (!check_sample_fmt(codec, in.sampleFmt)) {
        qDebug() << "编码器不支持该采样格式！";
        return;
    }
#else // if 0
    int avcodec_get_supported_config(const AVCodecContext * avctx,
                                     const AVCodec *codec,
                                     enum AVCodecConfig config,
                                     unsigned flags,
                                     const void **out_configs,
                                     int *out_num_configs);
    const void **out_configs = NULL;
    int number;
    int num_configs = avcodec_get_supported_config(NULL,
                                                   codec,
                                                   AV_CODEC_CONFIG_CHANNEL_LAYOUT,
                                                   0,
                                                   out_configs,
                                                   &number);

    if ((num_configs <= 0) || !out_configs) {
        ERROR_BUF(num_configs);
        qDebug() << "No supported configs found";
        return;
    } else {
        for (int i = 0; i < num_configs; i++) {
            QString str = QString("Config %1: %2").
                          arg(i).arg(((const char **)out_configs)[i]);
            qDebug() << str;
        }
    }
#endif // if 0

    // ===== 第三步：创建并配置编码器上下文 =====
    ctx = avcodec_alloc_context3(codec); // 分配上下文内存

    if (!ctx) {
        qDebug() << "创建编码器上下文失败！";
        return;
    }

    // avcodec_get_supported_config()

    // 设置编码器参数（和输入PCM的参数一致）
    ctx->sample_rate = in.sampleRate;         // 采样率
    ctx->sample_fmt = in.sampleFmt;           // 采样格式
    ctx->ch_layout.nb_channels = in.chLayout; // 声道布局
    ctx->bit_rate = 32000;                    // 比特率（越高音质越好，文件越大）
    ctx->profile = AV_PROFILE_AAC_HE_V2;      // AAC编码规格（HE-V2是高效压缩）

    // 打开编码器
    ret = avcodec_open2(ctx, codec, nullptr);

    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "打开编码器失败：" << errbuf;
        goto end; // 跳转到结尾释放资源
    }

    // ===== 第四步：创建PCM帧（frame）和AAC包（pkt） =====
    frame = av_frame_alloc(); // 分配frame内存

    if (!frame) {
        qDebug() << "创建PCM帧失败！";
        goto end;
    }

    // 设置frame的参数（和编码器一致）
    frame->nb_samples = ctx->frame_size;                       // 每一帧的样本数（编码器决定）
    frame->format = ctx->sample_fmt;                           // 采样格式
    frame->ch_layout.nb_channels = ctx->ch_layout.nb_channels; // 声道布局
    // 为frame分配实际的内存缓冲区（存放PCM数据）
    ret = av_frame_get_buffer(frame, 0);

    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "为PCM帧分配缓冲区失败：" << errbuf;
        goto end;
    }

    pkt = av_packet_alloc(); // 分配AAC包内存

    if (!pkt) {
        qDebug() << "创建AAC包失败！";
        goto end;
    }

    // ===== 第五步：打开文件 =====
    if (!inFile.open(QFile::ReadOnly)) {
        qDebug() << "打开PCM文件失败：" << in.filename;
        goto end;
    }

    if (!outFile.open(QFile::WriteOnly)) {
        qDebug() << "打开AAC文件失败：" << outFilename;
        goto end;
    }

    // ===== 第六步：读取PCM数据，编码成AAC =====
    // 循环读取PCM文件数据到frame的缓冲区
    while ((ret = inFile.read((char *)frame->data[0], frame->linesize[0])) > 0) {
        // 如果读取的字节数不够一帧，调整有效样本数（避免编码冗余数据）
        if (ret < frame->linesize[0]) {
            // 每个样本的字节数
            int bytes = av_get_bytes_per_sample((AVSampleFormat)frame->format);

            // 声道数
            int ch = frame->ch_layout.nb_channels; // av_get_channel_layout_nb_channels(frame->channel_layout);

            // 实际有效的样本数
            frame->nb_samples = ret / (bytes * ch);
        }

        // 调用编码函数，把PCM转AAC并写入文件
        if (encode(ctx, frame, pkt, outFile) < 0) {
            goto end;
        }
    }

    // ===== 第七步：刷新编码器（处理最后一批数据） =====
    encode(ctx, nullptr, pkt, outFile);

    // ===== 结尾：释放资源 =====
end:

    // 关闭文件
    inFile.close();
    outFile.close();

    // 释放FFmpeg对象（必须释放，否则内存泄漏）
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&ctx);

    qDebug() << "编码完成！";
}

int FfmpegObject::saveToWavStart(QString filename,
                                 int     sampleRate,
                                 int     bitRate,
                                 int     channel)
{
    m_filename = filename;
    m_sampleRate = sampleRate;
    m_bitRate = bitRate;
    m_channel = channel;

    m_waveHead.bit_rate = qToLittleEndian(bitRate);
    m_waveHead.bits_per_sample = qToLittleEndian(bitRate);
    m_waveHead.channel = qToLittleEndian(channel);
    m_waveHead.sample_rate = qToLittleEndian(sampleRate);
    m_waveHead.byte_rate =
        qToLittleEndian((int)((sampleRate * (bitRate / 8) * channel)));
    m_waveHead.block_align = qToLittleEndian((short)((bitRate / 8) * channel));

    m_file.setFileName(filename);

    if (!m_file.open(QIODevice::ReadWrite)) {
        qDebug() << m_file.errorString();
        return -1;
    }

    m_file.seek(sizeof(wave_t));

    // m_file.write((char *)&m_waveHead, sizeof(wave_t));

    return 0;
}

qint64 FfmpegObject::saveToWav(const QByteArray& data)
{
    if (!m_file.isOpen()) return -1;

    return m_file.write(data);
}

int FfmpegObject::saveToWavEnd()
{
    if (!m_file.isOpen()) return -1;

    m_waveHead.data_length = m_file.size() - sizeof(wave_t);
    m_waveHead.wave_length = m_file.size() - 8;

    m_file.seek(0);
    int cnt = m_file.write((char *)&m_waveHead, sizeof(wave_t));

    m_file.close();

    if (cnt != sizeof(wave_t)) {
        return -1;
    }
    return 0;
}
