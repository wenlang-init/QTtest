#ifndef FFMPEGSCREEN_H
#define FFMPEGSCREEN_H
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QImage>
#ifdef __cplusplus
extern "C" {
# include <libavutil/avutil.h>
# include <libswscale/swscale.h>
# include <libavcodec/avcodec.h>
# include <libavformat/avformat.h>
# include <libavdevice/avdevice.h>
# include <libavutil/imgutils.h>
# include <libavutil/opt.h>
}
#endif // ifdef __cplusplus

class ffmpegScreen : public QThread {
    Q_OBJECT

public:

    static ffmpegScreen& instance() {
        static ffmpegScreen instance;

        return instance;
    }

    explicit ffmpegScreen(QObject *parent = nullptr);
    void info();
    virtual ~ffmpegScreen();

    // url:desktop表示捕获桌面,文件名表示捕获视频文件，rtsp://实时流协议，rtmp://消息传输协议
    // 使用 ffmpeg 命令行捕获桌面（dxgi）
    // ddagrab=0：捕获第一个显示器。hwdownload：将硬件帧下载到内存。format=bgra：将像素格式转换为 BGRA。
    // ffmpeg -filter_complex "ddagrab=0,hwdownload,format=bgra"
    //  -c:v libx264 output.mp4
    // 使用 h264_nvenc (NVIDIA GPU) 的命令行参考如下
    // ffmpeg -f lavfi -i ddagrab -c:v h264_nvenc -cq 18 output.mp4

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

// 将QImage编码为av1
class AV1Encoder {
public:

    // 测试函数
    static int test();

    /**
     * @param outputFile 输出文件路径（如 "video.mp4"）
     * @param width      视频宽度
     * @param height     视频高度
     * @param fps        帧率
     * @param crf        质量因子（0~63，推荐 20~35），0 表示无损（仅 libsvtav1 支持）
     * @param preset     编码速度预设（0~13，数值越大越快，质量略降）
     */
    AV1Encoder(const QString& outputFile,
               int            width,
               int            height,
               int            fps = 30,
               int            crf = 30,
               int            preset = 8);
    ~AV1Encoder();

    /**
     * 编码单帧图像
     * @param image 输入的 QImage（推荐格式为 Format_ARGB32 或 Format_RGB32）
     * @return 成功返回 true
     */
    bool encodeFrame(const QImage& image);

    /** 冲刷编码器，写入剩余帧并关闭文件 */
    bool flush();

    // 关闭
    void close();

private:

    /** 初始化编码器、输出容器和转换上下文 */
    bool init();

private:

    QString m_outputFile;
    int m_width, m_height, m_fps;
    int m_crf, m_preset;

    AVFormatContext *m_formatCtx;
    AVCodecContext *m_codecCtx;
    AVStream *m_stream;
    SwsContext *m_swsCtx;
    AVFrame *m_frame;
    AVPacket *m_packet;

    int64_t m_frameIndex;
    bool m_isInitialized;

    char errbuf[AV_ERROR_MAX_STRING_SIZE] = { 0 };
};

class SaveAV1FromQImage : public QThread {
    Q_OBJECT

private:

    ~SaveAV1FromQImage() {
        qDebug();
    }

public:

    // 停止线程，并且会在编码完成后会发出finish信号，根据此信号可实现异步delete
    void stop() {
        m_isRunning.store(false);
    }

    void usedUpdate(bool usedUpdate) {
        m_usedUpdate = usedUpdate;
    }

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

    /**
     * @param outputFile 输出文件路径（如 "video.mp4"）
     * @param width      视频宽度
     * @param height     视频高度
     * @param fps        帧率
     * @param crf        质量因子（0~63，推荐 20~35），0 表示无损（仅 libsvtav1 支持）
     * @param preset     编码速度预设（0~13，数值越大越快，质量略降）
     */
    explicit SaveAV1FromQImage(const QString& outputFile,
                               int            width,
                               int            height,
                               int            fps = 30,
                               int            crf = 30,
                               int            preset = 8) : QThread{nullptr} {
        m_outputFile = outputFile;
        m_width = width;
        m_height = height;
        m_fps = fps;
        m_crf = crf;
        m_preset = preset;

        // connect(this, &SaveAV1FromQImage::finish, this,
        // &SaveAV1FromQImage::quit);
        // connect(this,
        //         &SaveAV1FromQImage::finished,
        //         this,
        //         &SaveAV1FromQImage::deleteLater);
        // start();
    }

protected:

    void run() override
    {
        AV1Encoder encoder(m_outputFile, m_width, m_height, m_fps, m_crf,
                           m_preset);

        while (m_isRunning.load()) {
            if (!m_usedUpdate && (m_imageList.size() < 1)) {
                QThread::msleep(1);
                continue;
            }

            if (m_usedUpdate) {
                QMutexLocker locker(&m_mutex);

                if (m_countTemp == m_count) {
                    QThread::msleep(1);
                    continue;
                } else {
                    m_countTemp = m_count;
                }
            }

            QMutexLocker locker(&m_mutex);
            QImage image;

            if (!m_usedUpdate) {
                image = m_imageList.first();
                m_imageList.removeFirst();
            } else {
                image = m_image;
            }
            locker.unlock();

            if (!encoder.encodeFrame(image)) {
                stop();
                emit error("Encode failed");
                break;
            }
        }

        if (!m_usedUpdate) {
            for (int i = 0; i < m_imageList.size(); i++) {
                const QImage& image = m_imageList.first();

                if (!encoder.encodeFrame(image)) {
                    emit error("Encode failed");
                    break;
                }
            }
            m_imageList.clear();
        }

        encoder.flush();
        emit finish();
    }

signals:

    void finish();
    void error(const QString& msg);

public:

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

private:

    QMutex m_mutex;

    std::atomic_bool m_isRunning{ true }; // 初始化
    // m_isRunning.store(false); // 设置值
    // if(m_isRunning.load()); // 加载值

    bool m_usedUpdate = true; // 性能不足，编码太慢，只取最新帧

    QString m_outputFile;
    int m_width;
    int m_height;
    int m_fps;
    int m_crf;
    int m_preset;
    QList<QImage>m_imageList;
    QImage m_image;
    int m_count = 0;     // 总帧数
    int m_countTemp = 0; // 判断是否有新帧
};

#endif // FFMPEGSCREEN_H
