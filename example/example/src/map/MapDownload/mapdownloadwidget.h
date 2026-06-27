#ifndef MAPDOWNLOADWIDGET_H
#define MAPDOWNLOADWIDGET_H

#include <QWidget>
#include "downloadthread.h"
#include "downloadthreads.h"
#include <QElapsedTimer>
#include "mapStruct1.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MapDownLoadWidget; }
QT_END_NAMESPACE

class MapDownLoadWidget : public QWidget {
    Q_OBJECT

public:

    MapDownLoadWidget(QWidget *parent = nullptr);
    ~MapDownLoadWidget();

private slots:

    void on_but_savePath_clicked();

    void on_but_thread_clicked(bool checked);

    void on_but_threads_clicked(bool checked);

private:

    void finished(ImageInfo1 info); // 通知下载完成的索引

private:

    Ui::MapDownLoadWidget *ui;
    DownloadThread *m_dThread = nullptr;   // 单线程下载
    DownloadThreads *m_dThreads = nullptr; // 多线程下载
    QElapsedTimer m_timer;
};
#endif // MAPDOWNLOADWIDGET_H
