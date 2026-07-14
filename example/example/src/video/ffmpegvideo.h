#ifndef FFMPEGVIDEO_H
#define FFMPEGVIDEO_H

#include <QThread>
#include <QImage>

class ffmpegVideo : public QThread {
    Q_OBJECT

public:

    explicit ffmpegVideo(QObject *parent = nullptr);
    ~ffmpegVideo();
    void restartwork(QString url);
    void stopwork();
    void pausework(bool ipause) {
        pause = ipause;
    }

protected:

    bool init();
    void run() override;

signals:

    void gotFrame(QImage image);
    void videoInfo(QString info);
    void errorInfo(QString error);

private:

    QString m_url;
    bool isrunning = false;
    bool pause = false;
};

#endif // FFMPEGVIDEO_H
