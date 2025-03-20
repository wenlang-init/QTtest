#ifndef MLOG_H
#define MLOG_H

#include <list>
#include <thread>
#include <mutex>
#include "printFunction.h"
using namespace std;

class MLog
{
private:
    MLog();
    virtual ~MLog();
public:
    static MLog& getInstance(){
        static MLog mlog;
        return mlog;
    }
    void start(const string &dir,unsigned int flushMs,unsigned int fileLogMaxSize);
    void writeLog(LOG_TYPE_ENUM level,const char* function,const char *file,const int line,const char* data);

protected:
    static void workthread(MLog *mlog);
protected:
    thread *m_thread;
    mutex m_mutex;
    bool isrunning;

    string logDir; // 日志路径
    string logDirFileName; // 当前日志文件名
    LOG_TYPE_ENUM level; // 日志保存级别
    unsigned int flushMs; // 日志保存刷新周期
    unsigned int fileLogMaxSize; // 日志文件大小
    //unsigned int fileLogMaxcount; // 日志文件最大数量

    unsigned int haveWrittenSize; // 已经写入的日志大小
    bool isprintStdout; // 是否输出显示
    bool isAddLineFeed; // 在日志后添加换行

    list<string> m_list; // 日志数据
};

#define ADTA_MAXSIZE 4096
#define WRITE_LOG(level,__format, ...) \
do{ \
        char buffer[ADTA_MAXSIZE + 1]; \
        snprintf(buffer,ADTA_MAXSIZE,__format,##__VA_ARGS__); \
        writeLog(level,__FUNCTION__,__FILE__,__LINE__,buffer); \
}while(0)

#define DEBUG_LOG(__format, ...)    WRITE_LOG(LOG_TYPE_ENUM_DEBUG,__format,##__VA_ARGS__)
#define WARRING_LOG(__format, ...)  WRITE_LOG(LOG_TYPE_ENUM_WARRING,__format,##__VA_ARGS__)
#define CRITICAL_LOG(__format, ...) WRITE_LOG(LOG_TYPE_ENUM_CRITICAL,__format,##__VA_ARGS__)
#define FATAL_LOG(__format, ...)    WRITE_LOG(LOG_TYPE_ENUM_FATAL,__format,##__VA_ARGS__)
#define INFO_LOG(__format, ...)     WRITE_LOG(LOG_TYPE_ENUM_INFO,__format,##__VA_ARGS__)

#endif // MLOG_H
