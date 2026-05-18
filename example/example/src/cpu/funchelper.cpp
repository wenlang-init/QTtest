#include "funchelper.h"
#include <QFile>
#include <QCryptographicHash>
#include <QDateTime>
#include <QProcess>
#include <QTime>

FuncHelper& FuncHelper::getInstance()
{
    static FuncHelper funcHelper;

    return funcHelper;
}

FuncHelper::FuncHelper(QObject *parent)
    : QObject{parent}
{}

void FuncHelper::slotGetFileHash(QString filePath, int bhash) {
    QString result;
    QFile   file(filePath);

    if (!file.exists()) {
        emit sig_hashfinished(result + " : " + file.errorString());
        return;
    }
    qint64 fileSize = file.size();
    qint64 fsize = fileSize;
    const qint64 bufferSize = 1024 * 500;

    if (file.open(QIODevice::ReadOnly))
    {
        char buffer[bufferSize];
        int  bytesRead = 0;
        int  readSize = qMin(fileSize, bufferSize);

        QCryptographicHash hash((QCryptographicHash::Algorithm)bhash);
        QTime time = QTime::currentTime();
        isruningGetFileHASH = true;

        QTime btime = time;

        // emit sig_md5info("");
        while (readSize > 0 && (bytesRead = file.read(buffer, readSize)) > 0)
        {
            if (isruningGetFileHASH == false)
            {
                file.close();
                isruningGetFileHASH = true;
                emit sig_hashfinished(result + " : 取消计算");
                return;
            }
            fileSize -= bytesRead;
            hash.addData(QByteArrayView(buffer, bytesRead));

            // hash.addData(buffer, bytesRead);
            readSize = qMin(fileSize, bufferSize);

            if (time.msecsTo(QTime::currentTime()) > 1000) {
                time = QTime::currentTime();
                int  infobl = (fsize - fileSize) * 100.0 / fsize;
                emit sig_hashinfo(infobl);
            }
        }

        file.close();
        result = QString(hash.result().toHex());
        emit sig_hashfinished(result + " 用时:" +
                              QString::number(btime.secsTo(QTime::currentTime()))
                              + "s");
    } else {
        emit sig_hashfinished(result + " : " + file.errorString());
        return;
    }
}

void FuncHelper::slotGetFileMD5(QString filePath)
{
    slotGetFileHash(filePath, QCryptographicHash::Md5);
}

QString FuncHelper::getFileHash(const QString& filePath, int bhash) {
    QString result;
    QFile   file(filePath);

    if (!file.exists()) {
        return result;
    }
    qint64 fileSize = file.size();
    const qint64 bufferSize = 1024 * 500;

    if (file.open(QIODevice::ReadOnly))
    {
        char buffer[bufferSize];
        int  bytesRead = 0;
        int  readSize = qMin(fileSize, bufferSize);

        QCryptographicHash hash((QCryptographicHash::Algorithm)bhash);

        while (readSize > 0 && (bytesRead = file.read(buffer, readSize)) > 0)
        {
            fileSize -= bytesRead;
            hash.addData(QByteArrayView(buffer, bytesRead));

            // hash.addData(buffer, bytesRead);
            readSize = qMin(fileSize, bufferSize);
        }

        file.close();
        result = QString(hash.result().toHex());
    }
    return result;
}

QString FuncHelper::getDataHash(const QByteArray& data, int bhash) {
    QCryptographicHash hash((QCryptographicHash::Algorithm)bhash);

    hash.addData(QByteArrayView(data));
    return hash.result().toHex();
}

QString FuncHelper::getFileMD5(const QString& filePath)
{
    return getFileHash(filePath, QCryptographicHash::Md5);
}

QString FuncHelper::getDataMD5(const QByteArray& data)
{
    return getDataHash(data, QCryptographicHash::Md5);
}

void FuncHelper::getRandomString(QString& sOut, int iNeedStrLong)
{
    srand(QDateTime::currentMSecsSinceEpoch()); // 为随机值设定一个seed

    static const QString sChrs =
        "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

    for (int i(0); i < iNeedStrLong; i++)
    {
        auto randomx = rand() % (sizeof(sChrs) - 1);
        sOut.append(sChrs[(int)randomx]);
    }
}

QByteArray FuncHelper::executeCmd(const QString& cmd, const QStringList& args)
{
    QByteArray strResult;
    QProcess   p;

    // connect(&p,SIGNAL(readyReadStandardOutput()),this,SLOT(readstanderoutput()));
    // connect(&p,SIGNAL(readyReadStandardError()),this,SLOT(readstandererror()));
    // connect(&p,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(errorOccurred(QProcess::ProcessError)));
    // connect(&p,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finished(int,QProcess::ExitStatus)));
    // connect(&p,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(stateChanged(QProcess::ProcessState)));


    p.setProgram(cmd);
    p.setArguments(args);
    p.start();

    if (!p.waitForFinished()) {
        // p.readAllStandardError();
        return strResult;
    }

    strResult = p.readAllStandardOutput();
    return strResult;
}

QByteArray FuncHelper::executeBashCmd(const QString& strCmd)
{
    QByteArray strResult;
    QProcess   p;

    p.start("bash", QStringList() << "-c" << strCmd);

    if (!p.waitForFinished()) {
        // p.readAllStandardError();
        return strResult;
    }

    strResult = p.readAllStandardOutput();

    return strResult;
}

bool FuncHelper::killExec(const QString& sExename)
{
#ifdef __WIN32
    QStringList strlist;
    strlist << "/FI" << QString("imagename eq %1").arg(sExename);

    // tasklist /FI "imagename eq a.exe"
    // taskkill -im a.exe -f
    // taskkill /PID 12345 -f
    QProcess p;
    p.start("tasklist", strlist);

    if (!p.waitForFinished()) {
        return false;
    }
    QByteArray rst = p.readAllStandardOutput();

    if (rst.isEmpty()) {
        return false;
    }
    QString rstlocal = QString::fromLocal8Bit(rst);

    if (rstlocal.contains(sExename)) {
        // taskkill -im a.exe -f
        QString cmd = "taskkill -im ";
        cmd += sExename + " -f";
        p.execute(cmd);

        // p.start(cmd);p.waitForFinished()
        // p.readAllStandardOutput();
        // p.readAllStandardError();
    }
    p.close();
#else // ifdef s__WIN32
    QStringList strlist;
    QProcess    p;
    strlist << sExename;
    p.start("pidof", strlist);
    p.waitForFinished();

    if (!p.waitForFinished()) {
        return false;
    }
    QByteArray rst = p.readAllStandardOutput();

    if (rst.isEmpty()) {
        return false;
    }
    QString rstlocal = QString::fromLocal8Bit(rst);

    QStringList lpid = rstlocal.split(" ");

    for (int i = 0; i < lpid.size(); i++) {
        QString cmd = "kill -9 ";
        cmd += lpid[i];
        p.execute(cmd);

        // p.readAllStandardOutput();
        // p.readAllStandardError();
    }
    p.close();
#endif // ifdef __WIN32
    return true;
}

#include <stdio.h>
#if defined(Q_OS_WINDOWS)
# include <windows.h>
# include <winbase.h>
#endif // if defined(Q_OS_WINDOWS)
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
# include <pthread.h>
# include <sched.h>
# include <unistd.h>
#endif // if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
int FuncHelper::setProcessCPU(int pid, int cpu)
{
    int cpukercount = sysconf(_SC_NPROCESSORS_ONLN); // CPU核心总数

    if (cpu >= cpukercount) cpu = cpukercount - 1;
    cpu_set_t mask;

    CPU_ZERO(&mask);
    CPU_SET(cpu, &mask);

    // pid == 0 表示当前进程
    if (sched_setaffinity(pid, sizeof(cpu_set_t), &mask) < 0) {
        perror("sched_setaffinity");
        return -1;
    }
    return 0;
}

int FuncHelper::getProcessCPU(int pid)
{
    int cpukercount = sysconf(_SC_NPROCESSORS_ONLN); // CPU核心总数
    cpu_set_t mask;

    CPU_ZERO(&mask);

    if (sched_getaffinity(pid, sizeof(mask), &mask) < 0) {
        perror("sched_getaffinity");
        return -1;
    }

    for (int i = 0; i < cpukercount; i++)
        if (CPU_ISSET(i, &mask)) return i;

    return -1;
}

int FuncHelper::setCurrentProcessCPU(int cpu)
{
    int pid = 0; // getpid();

    return setProcessCPU(pid, cpu);
}

int FuncHelper::getCurrentProcessCPU()
{
    int pid = 0; // getpid();

    return getProcessCPU(pid);
}

#endif // if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)

int FuncHelper::setThreadCPU(const void *tid, int cpu)
{
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    int threadid = *(int *)tid;
    int cpukercount = sysconf(_SC_NPROCESSORS_ONLN); // CPU核心总数

    if (cpu >= cpukercount) return -1;

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(cpu, &mask);

    if (pthread_setaffinity_np(threadid, sizeof(mask), &mask) < 0) {
        perror("pthread_setaffinity_np");
        return -1;
    }
    return 0;

#endif // if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
#if defined Q_OS_WINDOWS
    HANDLE thread = *(HANDLE *)tid;

    if (SetThreadAffinityMask(thread, 1 << cpu) == 0) {
        perror("SetThreadAffinityMask");
        return -1;
    }
    return 0;

#endif // if defined Q_OS_WINDOWS
    Q_UNUSED(tid)
    Q_UNUSED(cpu)
    return -1;
}

int FuncHelper::getThreadCPU(const void *tid)
{
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    int threadid = *(int *)tid;
    int cpukercount = sysconf(_SC_NPROCESSORS_ONLN); // CPU核心总数
    cpu_set_t mask;
    CPU_ZERO(&mask);

    if (pthread_getaffinity_np(threadid, sizeof(mask), &mask) < 0) {
        perror("pthread_getaffinity_np");
        return -1;
    }

    for (int i = 0; i < cpukercount; i++)
        if (CPU_ISSET(i, &mask)) return i;

    return -1;

#endif // if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
#if 0 // defined Q_OS_WINDOWS
    GetCurrentProcessorNumber();
    HANDLE   thread = *(HANDLE *)tid;
    uint64_t threadAffinityMask;

    if (GetThreadAffinityMask(thread,
                              &threadAffinityMask) != c_partyErrorSuccess) {
        // GetErrorMessage(errcode, errmsg, sizeof(errmsg));
        perror("GetThreadAffinityMask");
        return -1;
    }

    for (uint64_t i = 0; i < sizeof(threadAffinityMask) * 8; i++) {
        if (threadAffinityMask & (1 << i)) {
            return i;
        }
    }
    return -1;

#endif // if defined Q_OS_WINDOWS
    Q_UNUSED(tid)
    return -1;
}

int FuncHelper::setCurrentThreadCPU(int cpu)
{
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    int tid = pthread_self(); // syscall(SYS_gettid);
    return setThreadCPU(&tid, cpu);

#endif // if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
#if defined Q_OS_WINDOWS
    HANDLE tid = GetCurrentThread();
    return setThreadCPU(&tid, cpu);

#endif // if defined Q_OS_WINDOWS
    Q_UNUSED(cpu)
    return -1;
}

int FuncHelper::getCurrentThreadCPU()
{
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    int tid = pthread_self();
    return getThreadCPU(&tid);

#endif // if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
#if defined Q_OS_WINDOWS
    return GetCurrentProcessorNumber();

    // HANDLE tid = GetCurrentThread();
    // return getThreadCPU(&tid);

#endif // if defined Q_OS_WINDOWS
    return -1;
}
