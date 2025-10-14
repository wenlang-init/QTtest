#ifndef FUNCHELPER_H
#define FUNCHELPER_H

#include <QObject>

class FuncHelper : public QObject {
    Q_OBJECT

public:

    static FuncHelper& getInstance();

protected:

    explicit FuncHelper(QObject *parent = nullptr);

public:

    QString    getFileMD5(const QString& filePath);
    QString    getDataMD5(const QByteArray& data);

    void       getRandomString(QString& sOut,
                               int      iNeedStrLong = 64);
    QByteArray executeCmd(const QString    & cmd,
                          const QStringList& args);
    QByteArray executeBashCmd(const QString& strCmd);

    bool       killExec(const QString& sExename);

#ifdef Q_OS_LINUX
    int        setProcessCPU(int pid,
                             int cpu);
    int        getProcessCPU(int pid);
    int        setCurrentProcessCPU(int cpu);
    int        getCurrentProcessCPU();
#endif // ifdef Q_OS_LINUX
    int        setThreadCPU(const void *tid,
                            int         cpu);
    int        getThreadCPU(const void *tid);
    int        setCurrentThreadCPU(int cpu);
    int        getCurrentThreadCPU();

signals:

    void sig_md5info(int value);
};

#endif // FUNCHELPER_H
