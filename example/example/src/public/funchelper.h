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

    // 现代 CPU 的 sqrtss / rsqrtss 指令（SSE）已经非常快，且精度更高，
    // 因此在大多数场景下，直接使用标准库函数是最佳选择
    // 快速逆平方根 1/sqrt(x) , number为非负数
    static float Q_rsqrt(float number) {
        uint32_t i;
        float    x2, y;
        const float threehalfs = 1.5F;

        x2 = number * 0.5F;
        y = number;

        // IEEE 754浮点数标准 , 对数近似值
        i = *(uint32_t  *)&y;

        // double : 0x5fe6eb50c7b537a9
        // float : 0x5f3759df
        i = 0x5f3759df - (i >> 1);
        y = *(float *)&i;

        // 使用牛顿-拉弗森方法（y = y*(1.5 - 0.5*x*y*y)）
        // 迭代一次即可获得相当高的精度（相对误差约 0.17%），
        // 迭代两次可达到约 1‰ 的精度
        y = y * (threehalfs - (x2 * y * y)); // 1st iteration
        // y  = y * (threehalfs - (x2 * y * y)); // 2nd iteration, can be
        // removed
        return y;
    }

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

    bool   getScreenImage(HWND    wind,
                          QImage& image,
                          bool    hasBorder = true);
    QImage getWindowScreenImageFromDXGI(HWND hWnd);
    bool   getWindowScreenImageFromDXGI(QImage     & image,
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
