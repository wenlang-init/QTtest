#include "ffmpegtest.h"
#include "lognone.h"
#ifdef __cplusplus
extern "C" {
# include <libavutil/avutil.h>
# include <libavcodec/avcodec.h>
}
#endif // ifdef __cplusplus
#include <QDebug>
#define qdebug qDebug().noquote() << __FILE__ << __LINE__ << __FUNCTION__

ffmpegTest::ffmpegTest(QObject *parent)
    : QObject{parent}
{
    DEBUG_LOG("avutil_version: %d\n",   avutil_version());
    DEBUG_LOG("avutil_license: %s\n",   avutil_license());
    DEBUG_LOG("av_version: %s\n",       av_version_info());
    DEBUG_LOG("av_license: %s\n",       avutil_license());
    DEBUG_LOG("av_configuration: %s\n", avutil_configuration());

    DEBUG_LOG("codec list:\n");

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
    printf("\n"); fflush(stdout);
    DEBUG_LOG("-----------------------");
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
}
