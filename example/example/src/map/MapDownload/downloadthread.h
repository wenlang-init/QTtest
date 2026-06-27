#ifndef DOWNLOADTHREAD_H
#define DOWNLOADTHREAD_H

#include <QObject>
#include <QNetworkAccessManager>
#include "mapStruct1.h"

class DownloadThread : public QObject {
    Q_OBJECT

public:

    explicit DownloadThread(QObject *parent = nullptr);
    ~DownloadThread();

    void quit(); // 退出下载

signals:

    void getImage(QList<ImageInfo1>infos);
    void finished(ImageInfo1 info); // 返回下载后的瓦片，由于QImage为共享内存，所以传递不需要考虑太多性能

private:

    void started();        // 线程启动
    void threadFinished(); // 线程退出

    void startGet(QList<ImageInfo1>infos);
    void get();
    void on_finished(QNetworkReply *reply);

private:

    QThread *m_thread = nullptr;
    QNetworkAccessManager *m_manager = nullptr;
    QNetworkRequest m_request;
    QList<ImageInfo1>m_infos; // 保存需要下载的地图信息
    bool m_quit = false;
};

#endif // DOWNLOADTHREAD_H
