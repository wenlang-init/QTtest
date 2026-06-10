#ifndef FUNCHELPER_H
#define FUNCHELPER_H

#include <QObject>
#include <QCryptographicHash>

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
    QByteArray executeBashCmd(const QString& strCmd);

    bool       killExec(const QString& sExename);

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    int        setProcessCPU(int pid,
                             int cpu);
    int        getProcessCPU(int pid);
    int        setCurrentProcessCPU(int cpu);
    int        getCurrentProcessCPU();
#endif // if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    int        setThreadCPU(const void *tid,
                            int         cpu);
    int        getThreadCPU(const void *tid);
    int        setCurrentThreadCPU(int cpu);
    int        getCurrentThreadCPU();

signals:

    void sig_hashinfo(int value);
    void sig_hashfinished(const QString& md5);

    void sigGetFileHashPrivate(QString filePath,
                               int     hash);
};

#endif // FUNCHELPER_H
