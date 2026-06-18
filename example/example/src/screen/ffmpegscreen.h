#ifndef FFMPEGSCREEN_H
#define FFMPEGSCREEN_H

#include <QThread>
#include <QImage>
#ifdef __cplusplus
extern "C" {
// # include <libavutil/avutil.h>
# include <libswscale/swscale.h>
# include <libavcodec/avcodec.h>
# include <libavformat/avformat.h>
# include <libavdevice/avdevice.h>
# include <libavutil/imgutils.h>
}
#endif // ifdef __cplusplus

class ffmpegScreen : public QThread {
    Q_OBJECT

public:

    static ffmpegScreen& instance() {
        static ffmpegScreen instance;

        return instance;
    }

    void info();
    virtual ~ffmpegScreen();

    // url:desktop表示捕获桌面,文件名表示捕获视频文件，rtsp://实时流协议，rtmp://消息传输协议

    /*  widows:
            dshow : DirectShow输入格式，用于录制Windows上的摄像头、麦克风、屏幕、窗口等。
            gdigrab：GDI抓取输入格式，用于录制Windows上的屏幕或窗口，可以选择特定的窗口进行录制。
            vfwcap : 是一种输入格式，用于在 Windows 上录制视频设备（如摄像头）的输入源。
        linux:
            v4l2 ：Video for Linux 2输入格式，用于录制Linux上的视频设备，如USB摄像头等。
            alsa：Advanced Linux Sound Architecture输入格式，用于录制Linux上的音频设备，如麦克风、扬声器等。
            x11grab：X11抓取输入格式，用于录制X Window系统上的屏幕或窗口。
        mac:
            avfoundation：AVFoundation输入格式，用于录制Mac上的摄像头、麦克风、屏幕等。
            qtkit：QuickTime Kit输入格式，用于录制Mac上的摄像头、麦克风等。
     */
    void initparam(QString url = "desktop",
                   QString framerate = "25",
                   QString xoffset = "0",
                   QString yoffset = "0",
                   QString size = "1920x1080",
                   bool    isMouse = true
                   );
    void startwork();
    void stopwork();

protected:

    bool isinit();
    bool getImage(QImage& image);
    void run() override;

signals:

    void gotFrame(QImage image);

private:

    explicit ffmpegScreen(QObject *parent = nullptr);
    bool init();

    AVFormatContext *m_formatContext = nullptr;
    SwsContext *swsCtx;
    int videoStreamIndex = -1;
    AVCodecContext *codecCtx;
    AVPacket *packet;
    AVFrame *frame;
    AVFrame *rgbFrame;
    int rgbBufferSize;
    uint8_t *rgbBuffer;

    QString m_framerate = "1920x1080", m_size = "25", m_url = "desktop";
    QString m_xoffset = "0", m_yoffset = "0";
    bool m_isMouse = true;

    QThread *thread;
    bool isrunning = false;
};

#endif // FFMPEGSCREEN_H
