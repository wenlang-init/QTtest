#include "lognone.h"
#include <stdio.h>
#ifdef _MSC_VER
// 解决重定义struct timespec
#define HAVE_STRUCT_TIMESPEC
//#pragma comment(lib,"${PWD}/Pre-built.2/lib/x64/WpthreadVC2.lib")
#pragma comment(lib,"D:/work/QT/network/nettunnel/library/printFunction/Pre-built.2/lib/x64/pthreadVC2.lib")
//#pragma comment(lib,"D:/work/QT/network/nettunnel/library/printFunction/Pre-built.2/lib/x64/libpthreadGC2.a")
#endif

#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
//#include <time.h>
#include <stdarg.h>

#define LOG_USELIST
#ifdef LOG_USELIST
#include "mList.h"
#else
#include "mArray.h"
#endif

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#include <io.h>
#include <direct.h>
#define sleep(sec)   Sleep(sec * 1000)
#define msleep(msec) Sleep(msec)
#else
#define msleep(msec) usleep(msec * 1000)
#endif

#if defined(__GNUC__)
#include <unistd.h> // getpid();
#endif

#if defined(__unix__)
#include <errno.h>
#include <sys/syscall.h>
// gettid() // 线程ID
// syscall(SYS_gettid)

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

typedef struct _LogObj{
    FILE *fpid; // 写日志文件描述符
    pthread_t threadId; // 线程ID
    pthread_mutex_t mutex; // 线程锁
#ifdef LOG_USELIST
    List *logList; // 缓存日志数据
#else
    Array *logList; // 缓存日志数据
#endif
    char* logDir; // 日志路径
    char* logFilePNameCurrent; // 当前日志文件名
    LOG_TYPE_ENUM level; // 日志保存级别
    unsigned int flushMs; // 日志保存刷新周期
    unsigned int fileLogMaxSize; // 日志文件大小
    //unsigned int fileLogMaxcount; // 日志文件最大数量

    unsigned int haveWrittenSize; // 已经写入的日志大小
    int runstatus; // 线程运行状态
}LogObj;

LogObj *m_logObj = NULL;
static int m_printStdout = 1;

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
static void usleep(unsigned long usec)
{
    HANDLE timer;
    LARGE_INTEGER interval;
    interval.QuadPart = (-10 * usec);

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &interval, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}
#endif

int create_dir(const char *path)
{
    if (path == NULL)
        return -1;
    umask(0);
    int i, len,relen;
    len = strlen(path);
    char *dirname;
    if(path[len-1] != '/'){
        relen = len + 2;
        dirname = (char *)malloc(relen);
        memcpy(dirname, path, len);
        dirname[len] = '/';
        dirname[len+1] = 0;
    } else {
        relen = len + 1;
        dirname = (char *)malloc(relen);
        memcpy(dirname, path, len);
        dirname[len] = 0;
    }
    for (i = 0; i < relen; i++)
    {
        if ('/' == dirname[i])
        {
            dirname[i] = 0;
            if (access(dirname, 0) != 0) // F_OK = 0
            {
#ifdef __unix__
                mkdir(dirname, 0666);
#else
                _mkdir(dirname);
#endif
            }
            dirname[i] = '/';
        }
    }
    free(dirname);
    return 0;
}

int createFile(LogObj *m_logObj)
{
    if(!m_logObj){
        return -1;
    }

    if(0 != create_dir(m_logObj->logDir)){
        FATAL_PRINT_LOG("create_dir failed\n");
        free(m_logObj->logFilePNameCurrent);
        m_logObj->logFilePNameCurrent = NULL;
        return -1;
    }

    if(m_logObj->fpid){
        fclose(m_logObj->fpid);
    }

    static char filename[64];
    double dtime = __get_printfTime_d();
    long long lltimes = dtime;
    dtime = dtime - lltimes;
    long long lltime = dtime * 1000 * 1000;
    time_t timer = time(0);
    struct tm Now;
#if defined(__GNUC__) && defined(__unix__)
    if(localtime_r(&timer,&Now)){
        snprintf(filename, sizeof(filename),"%d%02d%02d%02d%02d%02d.%lld", 1900+Now.tm_year, Now.tm_mon+1, Now.tm_mday, Now.tm_hour, Now.tm_min, Now.tm_sec,lltime);
    }else{
        snprintf(filename, sizeof(filename),"get localtime failed");
    }
#else
    if(0 > localtime_s(&Now,&timer)){
        _snprintf_s(filename, sizeof(filename),sizeof(filename),"get localtime failed");
    } else {
        _snprintf_s(filename, sizeof(filename),sizeof(filename),"%d%02d%02d%02d%02d%02d.%lld", 1900+Now.tm_year, Now.tm_mon+1, Now.tm_mday, Now.tm_hour, Now.tm_min, Now.tm_sec,lltime);
    }
#endif
    if(m_logObj->logFilePNameCurrent){
        free(m_logObj->logFilePNameCurrent);
    }
    size_t dirlen = strlen(m_logObj->logDir);
    size_t filenamelen = strlen(filename);
    size_t size = dirlen + filenamelen + 1 + 5;
    m_logObj->logFilePNameCurrent = (char *)malloc(size);
    if(!m_logObj->logFilePNameCurrent){
        FATAL_PRINT_LOG("malloc:%s\n",strerror(errno));
        return -1;
    }
    memcpy(m_logObj->logFilePNameCurrent,m_logObj->logDir,dirlen+1);
    strcat(m_logObj->logFilePNameCurrent,"/");
    strcat(m_logObj->logFilePNameCurrent,filename);
    strcat(m_logObj->logFilePNameCurrent,".log");

    m_logObj->fpid = fopen(m_logObj->logFilePNameCurrent,"w");
    if(!m_logObj->fpid){
        FATAL_PRINT_LOG("fopen %s:%s\n",m_logObj->logFilePNameCurrent,strerror(errno));
        free(m_logObj->logFilePNameCurrent);
        m_logObj->logFilePNameCurrent = NULL;
        return -1;
    }

    INFO_PRINT_LOG("create log file:%s\n",m_logObj->logFilePNameCurrent);

    return 0;
}

void *handlefunc(void *logObj)
{
    LogObj *m_logObj = (LogObj *)logObj;
    double time = __get_printfTime_d();
    while(m_logObj->runstatus){
        if(m_logObj->logList->ro.count == 0) msleep(1);//usleep(1000);
        if(!m_logObj->fpid){
            createFile(m_logObj);
            m_logObj->haveWrittenSize = 0;
        }

        if(m_logObj->haveWrittenSize > m_logObj->fileLogMaxSize){
            if(m_logObj->fpid){
                fflush(m_logObj->fpid);
            }
            createFile(m_logObj);
            m_logObj->haveWrittenSize = 0;
        }

        pthread_mutex_lock(&m_logObj->mutex);
        if(m_logObj->logList->ro.count > 0){
#ifdef LOG_USELIST
            List_Node *node;
#else
            Array_Node *node;
#endif
            node = m_logObj->logList->func.first(m_logObj->logList);
            size_t len = strlen((char*)node->data);
            if(m_logObj->fpid){
                int wcnt = fwrite(node->data,1,len,m_logObj->fpid);
                if(0 > wcnt){
                    FATAL_PRINT_LOG("fwrite %s:%s\n",node->data,strerror(errno));
                } else {
                    m_logObj->haveWrittenSize += wcnt;
                }
            }
            m_logObj->logList->func.removeFirst_d(m_logObj->logList);
        }
        pthread_mutex_unlock(&m_logObj->mutex);

        double currentTime = __get_printfTime_d();
        if((currentTime - time)*1000 > m_logObj->flushMs){
            time = currentTime;
            if(m_logObj->fpid){
                fflush(m_logObj->fpid);
            }
        }
    }

    pthread_mutex_lock(&m_logObj->mutex);
    // 退出时写剩下的数据
    while(m_logObj->logList->ro.count > 0){
        if(m_logObj->haveWrittenSize > m_logObj->fileLogMaxSize){
            if(m_logObj->fpid){
                fflush(m_logObj->fpid);
            }
            createFile(m_logObj);
            m_logObj->haveWrittenSize = 0;
        }
#ifdef LOG_USELIST
        List_Node *node;
#else
        Array_Node *node;
#endif
        node = m_logObj->logList->func.first(m_logObj->logList);
        size_t len = strlen((char*)node->data);
        if(m_logObj->fpid){
            int wcnt = fwrite(node->data,1,len,m_logObj->fpid);
            if(0 > wcnt){
                FATAL_PRINT_LOG("fwrite %s:%s\n",node->data,strerror(errno));
            } else {
                m_logObj->haveWrittenSize += wcnt;
            }
        }
        m_logObj->logList->func.removeFirst_d(m_logObj->logList);
    }
    //m_logObj->logList->func.clear_d(m_logObj->logList);
    pthread_mutex_unlock(&m_logObj->mutex);
    return NULL; // 返回值给pthread_join()的第二个参数
}

#ifdef __cplusplus
}
#endif

int initLog(const char* logDir,unsigned int flushMs,unsigned int fileLogMaxSize)
{
    if(!logDir){
        INFO_PRINT_LOG("logDir is NULL\n");
        return -1;
    }

    if(m_logObj){
        INFO_PRINT_LOG("logOBj it's already initialized\n");
        return 0;
    }

    m_logObj = (LogObj*)malloc(sizeof(LogObj));
    if(!m_logObj){
        FATAL_PRINT_LOG("malloc:%s\n",strerror(errno));
        return -1;
    }

    m_logObj->level = LOG_TYPE_ENUM_DEBUG;
    m_logObj->flushMs = flushMs;
    m_logObj->fileLogMaxSize = fileLogMaxSize;
    m_logObj->haveWrittenSize = 0;
    m_logObj->runstatus = 1;
#ifdef LOG_USELIST
    m_logObj->logList = init_list();
#else
    m_logObj->logList = init_array();
#endif
    if(!m_logObj->logList){
        FATAL_PRINT_LOG("init logList\n");
        free(m_logObj);
        m_logObj = NULL;
        return -1;
    }

    int len = strlen(logDir);
    m_logObj->logDir = (char *)malloc(len + 1);
    if(!m_logObj->logDir){
        FATAL_PRINT_LOG("malloc:%s\n",strerror(errno));
        m_logObj->logList->func.destroy_d(m_logObj->logList);
        free(m_logObj);
        m_logObj = NULL;
        return -1;
    }
    memcpy(m_logObj->logDir,logDir,len+1);

    m_logObj->logFilePNameCurrent = NULL;
    m_logObj->fpid = NULL;
    if(0 != createFile(m_logObj)){
        FATAL_PRINT_LOG("createFile failed\n");
        m_logObj->logList->func.destroy_d(m_logObj->logList);
        free(m_logObj->logDir);
        free(m_logObj);
        m_logObj = NULL;
        return -1;
    }

    if(0 != pthread_mutex_init(&m_logObj->mutex,NULL)){
        FATAL_PRINT_LOG("pthread_mutex_init:%s\n",strerror(errno));
        fclose(m_logObj->fpid);
        m_logObj->logList->func.destroy_d(m_logObj->logList);
        free(m_logObj->logDir);
        free(m_logObj->logFilePNameCurrent);
        free(m_logObj);
        m_logObj = NULL;
        return -1;
    }

    if(pthread_create(&m_logObj->threadId,NULL,handlefunc,m_logObj) != 0){
        FATAL_PRINT_LOG("pthread_create:%s\n",strerror(errno));
        fclose(m_logObj->fpid);
        pthread_mutex_destroy(&m_logObj->mutex);
        m_logObj->logList->func.destroy_d(m_logObj->logList);
        free(m_logObj->logDir);
        free(m_logObj->logFilePNameCurrent);
        free(m_logObj);
        m_logObj = NULL;
        return -1;
    }

    INFO_PRINT_LOG("init log success:%s\n",m_logObj->logDir);

    return 0;
}

void destinyLog()
{
    if(!m_logObj) return;
    m_logObj->runstatus = 0;
    // 等待线程结束
    int ret = pthread_join(m_logObj->threadId,NULL);
    if(ret != 0){
        FATAL_PRINT_LOG("pthread_join:%s\n",strerror(errno));
    }
    ret = pthread_mutex_destroy(&m_logObj->mutex);
    if(ret != 0){
        FATAL_PRINT_LOG("pthread_mutex_destroy:%s\n",strerror(errno));
    }
    if(m_logObj->fpid){
        fclose(m_logObj->fpid);
    }
    if(m_logObj->logList){
        m_logObj->logList->func.destroy_d(m_logObj->logList);
    }
    if(m_logObj->logDir){
        free(m_logObj->logDir);
    }
    if(m_logObj->logFilePNameCurrent){
        free(m_logObj->logFilePNameCurrent);
    }
    m_logObj = NULL;
}

int setLogLevel(LOG_TYPE_ENUM level)
{
    if(!m_logObj) return -1;
    m_logObj->level = level;
    return 0;
}

void writeLog(LOG_TYPE_ENUM level, const char *function, const char *file, const int line, const char *buffer)
{
    const char *typemsg;
    switch (level) {
    case LOG_TYPE_ENUM_DEBUG:
        typemsg = "Debug   ";
        break;
    case LOG_TYPE_ENUM_WARRING:
        typemsg = "Warring ";
        break;
    case LOG_TYPE_ENUM_CRITICAL:
        typemsg = "Critical";
        break;
    case LOG_TYPE_ENUM_FATAL:
        typemsg = "Fatal   ";
        break;
    case LOG_TYPE_ENUM_INFO:
        typemsg = "Info    ";
        break;
    default:
        typemsg = "        ";
        break;
    }

    unsigned long ProcessId = 0;
    unsigned long ThreadId = 0;

#if defined(WIN32) || defined(WIN64)
    // system("color 0");
    ProcessId = GetCurrentProcessId();
    ThreadId = GetCurrentThreadId();
#endif

#if defined(__unix__)
    ProcessId = getpid();
    //ThreadId = gettid();
    ThreadId = syscall(SYS_gettid);
#endif

    char timechr[64];
    __get_printfTime(timechr,sizeof(timechr));
    double timed = __get_printfTime_d();

    if(m_printStdout){
        fprintf(stdout,YELLOW "%s:" GREEN "time:%s(%.6lf)" BOLDBLACK "|" RESET BOLDYELLOW "PId:%lu" BOLDBLUE "Tid:%lu" BOLDBLACK "|" RESET CYAN "%s:%d" BLUE "(%s)" MAGENTA "---" RESET "%s\n",typemsg,timechr,timed, ProcessId,ThreadId,file, line,function,buffer);
        fflush(stdout);
    }

    if(m_logObj){
        if(m_logObj->level <= level){
            int buffer_len = strlen(buffer);
            char tmpbuf[1024];
            int len = snprintf(tmpbuf,sizeof(tmpbuf)," %s(%.6lf)|(%lu:%lu) %s:%d(%s) ",timechr,timed, ProcessId,ThreadId,file,line,function);
            if(len < 0){
                return;
            }
            int typemsg_len = strlen(typemsg);
            char *data;
            if(buffer[buffer_len-1] != '\n'){
                data = (char *)malloc(buffer_len + len + typemsg_len + 2);
            } else {
                data = (char *)malloc(buffer_len + len + typemsg_len + 1);
            }
            if(data){
                memcpy(data,typemsg,typemsg_len+1);
                strcat(data,tmpbuf);
                strcat(data,buffer);
                if(buffer[buffer_len-1] != '\n'){
                    strcat(data,"\n");
                }

                pthread_mutex_lock(&m_logObj->mutex);
                m_logObj->logList->func.append(m_logObj->logList,data);
                pthread_mutex_unlock(&m_logObj->mutex);
            }
        }
    }
}

void writeLogformat(LOG_TYPE_ENUM level,const char* function,const char *file,const int line,const char* format, ...)
{
    char buffer[ADTA_MAXSIZE + 1];

    va_list arg;
    va_start (arg, format);
    vsnprintf(buffer, ADTA_MAXSIZE, format, arg);
    va_end (arg);

    writeLog(level,function,file,line,buffer);
}

void writeLognone(LOG_TYPE_ENUM level,const char* data)
{
    const char *typemsg;
    switch (level) {
    case LOG_TYPE_ENUM_DEBUG:
        typemsg = "Debug";
        break;
    case LOG_TYPE_ENUM_WARRING:
        typemsg = "Warring";
        break;
    case LOG_TYPE_ENUM_CRITICAL:
        typemsg = "Critical";
        break;
    case LOG_TYPE_ENUM_FATAL:
        typemsg = "Fatal";
        break;
    case LOG_TYPE_ENUM_INFO:
        typemsg = "Info";
        break;
    default:
        typemsg = "";
        break;
    }

    unsigned long ProcessId = 0;
    unsigned long ThreadId = 0;

#if defined(WIN32) || defined(WIN64)
    // system("color 0");
    ProcessId = GetCurrentProcessId();
    ThreadId = GetCurrentThreadId();
#endif

#if defined(__unix__)
    ProcessId = getpid();
    //ThreadId = gettid();
    ThreadId = syscall(SYS_gettid);
#endif

    if(m_printStdout){
        char cur_time[64];
        fprintf(stdout,YELLOW "%s:" GREEN "time:%s(%.6lf)" BOLDBLACK "|" RESET BOLDYELLOW "PId:%lu" BOLDBLUE "Tid:%lu" BOLDBLACK "|" RESET MAGENTA "---" RESET "%s\n",typemsg,__get_printfTime(cur_time,sizeof(cur_time)),__get_printfTime_d(), ProcessId,ThreadId,data);
        fflush(stdout);
    }

    if(m_logObj){
        if(m_logObj->level <= level){
            int data_len = strlen(data);
            int typemsg_len = strlen(typemsg);
            char *ndata = (char *)malloc(data_len + typemsg_len + 20);
            if(ndata){
                memcpy(ndata,typemsg,typemsg_len+1);
                strcat(ndata,data);

                pthread_mutex_lock(&m_logObj->mutex);
                m_logObj->logList->func.append(m_logObj->logList,ndata);
                pthread_mutex_unlock(&m_logObj->mutex);
            }
        }
    }
}

void setLogPrint(int isPrint)
{
    m_printStdout = isPrint;
}

int getcurrentLogCount()
{
    if(!m_logObj){
        return 0;
    }
    return m_logObj->logList->ro.count;
}
