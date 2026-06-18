#ifndef FUNCHELPER_H
#define FUNCHELPER_H

#include <QObject>
#include <QCryptographicHash>
#include <QPixmap>
#if defined(Q_OS_WINDOWS)
# include <qt_windows.h>
#endif // if defined(Q_OS_WINDOWS)

class FuncHelper : public QObject {
    Q_OBJECT

public:

    static FuncHelper& getInstance();
    void               getFileHashSig(QString filePath,
                                      int     hash);

protected:

    explicit FuncHelper(QObject *parent = nullptr);

public slots:

    void slotGetFileHash(QString filePath,
                         int     hash);
    void slotGetFileMD5(QString filePath);

private:

    bool isruningGetFileHASH = true;

public:

    void stopGetFileHASH() {
        if (isruningGetFileHASH) isruningGetFileHASH = false;
    }

    QString    getFileHash(const QString& filePath,
                           int            hash);
    QString    getDataHash(const QByteArray& data,
                           int               hash);
    QString    getFileMD5(const QString& filePath);
    QString    getDataMD5(const QByteArray& data);

    void       getRandomString(QString& sOut,
                               int      iNeedStrLong = 64);
    QByteArray executeCmd(const QString    & cmd,
                          const QStringList& args);
    void       execCmd(const QString    & cmd,
                       const QString    & workDir,
                       const QStringList& args);
    QByteArray executeBashCmd(const QString& strCmd);

    bool       killExec(const QString& sExename);

#if defined(Q_OS_WINDOWS)
    struct WindowInfo {
        HWND    hwnd;
        DWORD   processID;
        QString className;
        QString windowName;
        QString filePathName;
    };

    // 根据类名 获取HWND,窗口
    HWND getWindowHWNDFromClassname(QString className,
                                    QRect & rect);

    // 根据窗口名 获取HWND,窗口
    HWND getWindowHWNDFromTitle(QString title,
                                QRect & rect);

    // 获取窗口rect
    QRect                        getRectFromHwnd(HWND wind);

    // 获取窗口rect,无边框
    QRect                        getRectNoBorderFromHwnd(HWND wind);

    // 根据HWND 获取进程ID
    DWORD                        getWindowProcessID(HWND wind);

    // 根据HWND 获取进程文件名(含路径)
    QString                      getWindowFilePathName(HWND wind);

    // 根据HWND 获取进程窗口名
    QString                      getWindowName(HWND wind);

    // 根据HWND 获取进程窗口类名
    QString                      getWindowClass(HWND wind);

    // 根据进程ID 获取显示的窗口的hwnd
    HWND                         getWindowByPID(DWORD pid);

    // 根据进程ID 获取显示的窗口的hwnd等信息
    QList<HWND>                  getWindowHwndList();

    // 根据进程ID 获取显示的窗口的hwnd等信息
    QList<FuncHelper::WindowInfo>getWindowInfoList();

    // 获取系统缩放比,系统放大倍数(在设置中的缩放比率)
    double                       getSystemZoom();

    // 物理分辨率与系统分辨率的比例()
    double                       getSystemPixZoom();

    // 获取wind的窗口到image
    bool                         getScreenImage(HWND    wind,
                                                QImage& image,
                                                bool    hasBorder = true);
    bool                         getWindowScreenImageFromDXGI(QImage     & image,
                                                              const QRect& rect);
#endif // if defined(Q_OS_WINDOWS)

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)

    // 设置当前线程为最高优先级(pthread)
    bool setThreadPriorityToTopPriority(const QString& sThreadName);
    int  setProcessCPU(int pid,
                       int cpu);
    int  getProcessCPU(int pid);
    int  setCurrentProcessCPU(int cpu);
    int  getCurrentProcessCPU();
#endif // if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    int  setThreadCPU(const void *tid,
                      int         cpu);
    int  getThreadCPU(const void *tid);
    int  setCurrentThreadCPU(int cpu);
    int  getCurrentThreadCPU();

    bool getLinkInfo(const QString& sfilename,
                     QString      & filename,
                     QPixmap      & pixmap);

signals:

    void sig_hashinfo(int value);
    void sig_hashfinished(const QString& md5);

    void sigGetFileHashPrivate(QString filePath,
                               int     hash);
};

#endif // FUNCHELPER_H
