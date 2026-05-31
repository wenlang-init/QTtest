#ifndef FFMPEGOBJECT_H
#define FFMPEGOBJECT_H

#include <QObject>
#include <QFile>
#include <QtEndian>

extern "C" {
#include <libavformat/avformat.h>
}


class FfmpegObject : public QObject {
    Q_OBJECT

private:

    // 定义PCM输入参数的结构体（把参数打包，方便传递）
    typedef struct {
        const char    *filename;   // PCM文件路径
        int            sampleRate; // 采样率
        AVSampleFormat sampleFmt;  // 采样格式
        int            chLayout;   // 声道布局
    } AudioEncodeSpec;
#pragma pack(1)
    typedef struct _wave_header_t
    {
        char  wave_header[4];  // WAVE 头1
        int   wave_length;     // 音频数据的长度 +44 -8
        char  format[8];       // WAVE+fmt
        int   bit_rate;        // 一个采样点占几个bit
        short pcm;             // 音频数据编码方式
        short channel;         // 通道数
        int   sample_rate;     // 采样率
        int   byte_rate;       // 采样率×每次采样大小
        short block_align;     // 每次采样的大小 = 采样精度*声道数/8(单位是字节) 譬如 16bit 立体声在这里的值是
                               // 4 字节
        short bits_per_sample; // 每个声道的采样精度; 譬如 16bit 在这里的值就是16
        char  fix_data[4];     // "data"
        int   data_length;     // 音频数据的长度
        _wave_header_t() {
            memcpy(wave_header, "RIFF",     4);
            wave_length = qToLittleEndian((int) - 1);
            memcpy(format,      "WAVEfmt ", 8);
            bit_rate = qToLittleEndian(16);
            pcm = qToLittleEndian(1);
            channel = qToLittleEndian(2);
            sample_rate = qToLittleEndian(44100);
            byte_rate = qToLittleEndian(44100 * (16 / 8) * 2);
            block_align = qToLittleEndian((short)((16 / 8) * 2));
            bits_per_sample = qToLittleEndian((short) 16);
            memcpy(fix_data, "data", 4);
            data_length = qToLittleEndian((int) - 1);
        }
    } wave_t;
#pragma pack()

public:

    explicit FfmpegObject(QObject *parent = nullptr);
    static void getInfo();
    static void aacEncode(const QString      & filename,
                          const int            sampleRate,
                          const AVSampleFormat sampleFmt,
                          const int            channels,
                          const QString      & outFilename);
    static void aacEncode(AudioEncodeSpec & in, const char *outFilename);

    int saveToWavStart(QString filename,
                       int     sampleRate = 44100,
                       int     bitRate = 16,
                       int     channel = 2);
    qint64 saveToWav(const QByteArray& data);
    int    saveToWavEnd();

signals:

private:

    QFile m_file;
    QString m_filename;
    int m_sampleRate = 44100;
    int m_bitRate = 16;
    int m_channel = 2;

    wave_t m_waveHead;
};

#endif // FFMPEGOBJECT_H
