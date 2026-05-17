#include "lognone.h"
#include <stdio.h>

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
//#include <time.h>
#include <stdarg.h>
#include <time.h>

//#define LOG_USELIST
#ifdef LOG_USELIST
#include "mList.h"
#else
#include "mArray.h"
#endif

#ifdef _MSC_VER
// #define USE_WINDOWS_THREAD 1
#endif

#if defined(_MSC_VER)
#if USE_WINDOWS_THREAD
// 线程相关的一些宏和函数声明
#include <process.h>
#else
#include <pthread.h>
// 解决重定义struct timespec
#define HAVE_STRUCT_TIMESPEC
#pragma comment(lib,PTHREAD_USE_LIB)
//#pragma comment(lib,"./Pre-built.2/lib/x64/pthreadVC2.lib")
//#pragma comment(lib,"D:/work/QT/network/QTtest/example/example/module/log/Pre-built.2/lib/x64/pthreadVC2.lib")
#endif
#else
#include <pthread.h>
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

// #if defined(WIN32) || defined(WIN64)
// HANDLE hMutex;
// int sharedata = 0;
// DWORD WINAPI threadFunc(LPVOID lpParam) {
//     for (int i = 0; i < 5; i++) {
//         // 获取互斥量
//         WaitForSingleObject(hMutex,INFINITE);
//         sharedata++;
//         printf("Thread is running with parameter: %p\n",lpParam);
//         // 释放互斥量
//         ReleaseMutex(hMutex);
//     }
//     return 0;
// }
// // 定义一个线程函数，打印参数值
// DWORD WINAPI ThreadFunc(LPVOID lpParam) {
//     int* p = (int*)lpParam;
//     printf("Thread is running with parameter: %d\n",*p);
//     return 0;
// }
// void process(){
//     hMutex = CreateMutex(NULL,FALSE,NULL);
//     if (hMutex == NULL) {
//         printf("Create Mutex error.\n");
//         return ;
//     }
//     HANDLE hThraed1, hThread2;

//     hThraed1 = CreateThread(NULL, 0, threadFunc, NULL, 0, NULL);
//     hThread2 = CreateThread(NULL, 0, threadFunc, NULL, 0, NULL);

//     if (hThraed1 == NULL || hThread2 == NULL) {
//         printf("Create thread failed.\n");
//         return;
//     }
//     // 强制结束线程
//     //TerminateThread(hThraed1, 0);

//     //#define WM_QUIT_THREAD (WM_USER + 1)
//     // 向指定线程发送一个消息，让线程在处理这个消息时结束自己
//     // 线程函数中：
//     // DWORD WINAPI threadFunc(LPVOID lpParam) {
//     //     printf("Thread is running...\n");
//     //     MSG msg;
//     //     while (GetMessage(&msg, NULL, 0, 0)) {
//     //         if (msg.message == WM_QUIT_THREAD) {
//     //             break;
//     //         }
//     //         TranslateMessage(&msg);
//     //         DispatchMessage(&msg);
//     //     }
//     //     printf("Thread is ending...\n");
//     //     return 0;
//     // }
//     //PostThreadMessage(GetThreadId(hThraed1), WM_QUIT_THREAD, 0, 0);

//     // 等待线程结束
//     WaitForSingleObject(hThraed1,1);
//     WaitForSingleObject(hThread2,INFINITE);

//     CloseHandle(hThraed1);
//     CloseHandle(hThread2);
//     CloseHandle(hMutex);

//     printf("The final value of sharedata is :%d\n",sharedata);

//     // // 创建一个句柄
//     // HANDLE hTheard;
//     // // 线程id变量
//     // DWORD dwThreadId;
//     // int param = 123;

//     // hTheard = CreateThread(
//     //             NULL, // 安全性
//     //             0,    // 线程堆栈大小
//     //             ThreadFunc,  // 线程函数指针
//     //             &param,       // 线程函数参数
//     //             0,            // 线程创建标志，默认，立即执行
//     //             &dwThreadId); // 接收线程id

//     // // 检查是否创建成功
//     // if (hTheard == NULL) {
//     //     DWORD err = GetLastError();
//     //     printf("Failed to create Thread with code %lu.\n",err);
//     //     return;
//     // }
//     // // 等待线程结束
//     // WaitForSingleObject(hTheard,INFINITE);
//     // // 关闭线程
//     // CloseHandle(hTheard);
// }
// #endif

//#define USE_COLOR_LOG

typedef struct _LogObj{
    FILE *fpid; // 写日志文件描述符
#ifdef USE_WINDOWS_THREAD // _MSC_VER
    HANDLE threadId;
    HANDLE mutex;
#else
    pthread_t threadId; // 线程ID
    pthread_mutex_t mutex; // 线程锁
#endif
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
                mkdir(dirname, 0755);
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

#ifdef USE_WINDOWS_THREAD
DWORD WINAPI handlefunc(LPVOID logObj)
#else
void *handlefunc(void *logObj)
#endif
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
#ifdef USE_WINDOWS_THREAD
        WaitForSingleObject(m_logObj->mutex,INFINITE);
        //ReleaseMutex(m_logObj->mutex);
#else
        pthread_mutex_lock(&m_logObj->mutex);
#endif
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
#ifdef USE_WINDOWS_THREAD
        ReleaseMutex(m_logObj->mutex);
#else
        pthread_mutex_unlock(&m_logObj->mutex);
#endif
        double currentTime = __get_printfTime_d();
        if((currentTime - time)*1000 > m_logObj->flushMs){
            time = currentTime;
            if(m_logObj->fpid){
                fflush(m_logObj->fpid);
            }
        }
    }
#ifdef USE_WINDOWS_THREAD
        WaitForSingleObject(m_logObj->mutex,INFINITE);
#else
    pthread_mutex_lock(&m_logObj->mutex);
#endif
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
#ifdef USE_WINDOWS_THREAD
    ReleaseMutex(m_logObj->mutex);
    return 0;
#else
    pthread_mutex_unlock(&m_logObj->mutex);
    // 退出当前线程,参数与线程回调函数返回值的作用相同,回值给pthread_join()的第二个参数
    // pthread_exit(NULL);
    return NULL; // 返回值给pthread_join()的第二个参数
#endif
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

#ifdef USE_WINDOWS_THREAD
    if(NULL == (m_logObj->mutex = CreateMutex(NULL,FALSE,NULL))){
#else
    if(0 != pthread_mutex_init(&m_logObj->mutex,NULL)){
#endif
        FATAL_PRINT_LOG("pthread_mutex_init:%s\n",strerror(errno));
        fclose(m_logObj->fpid);
        m_logObj->logList->func.destroy_d(m_logObj->logList);
        free(m_logObj->logDir);
        free(m_logObj->logFilePNameCurrent);
        free(m_logObj);
        m_logObj = NULL;
        return -1;
    }
#ifdef USE_WINDOWS_THREAD
    if(NULL == (m_logObj->threadId = CreateThread(NULL, 0, handlefunc, m_logObj, 0, NULL))){
#else
    if(pthread_create(&m_logObj->threadId,NULL,handlefunc,m_logObj) != 0){
#endif
        FATAL_PRINT_LOG("pthread_create:%s\n",strerror(errno));
        fclose(m_logObj->fpid);
#ifdef USE_WINDOWS_THREAD
        CloseHandle(m_logObj->mutex);
#else
        pthread_mutex_destroy(&m_logObj->mutex);
#endif
        m_logObj->logList->func.destroy_d(m_logObj->logList);
        free(m_logObj->logDir);
        free(m_logObj->logFilePNameCurrent);
        free(m_logObj);
        m_logObj = NULL;
        return -1;
    }
    // 将线程置为分离状态,子线程结束时会自动释放资源，主线程无需 pthread_join
    // 一旦分离，无法再 pthread_join 获取返回值（EINVAL 错误）
    // pthread_detach(&m_logObj->threadId);
    // 发送取消请求给指定的线程，要求该线程立即终止执行
    // 线程取消是一个异步操作，调用 pthread_cancel 后，目标线程会在某些特定的取消点
    //（如 sleep、wait 等）检查是否有取消请求，并根据取消状态决定是否终止执行
    // 线程可以通过 pthread_setcancelstate 和 pthread_setcanceltype 函数来设置自己的
    // 取消状态和取消类型。这些设置决定了线程是否可以被取消，以及在接收到取消请求时如何响应。
    //  PTHREAD_CANCEL_ENABLE：线程可以被取消，这是默认状态。
    //  PTHREAD_CANCEL_DISABLE：线程不可被取消，如果收到取消请求，请求会被挂起直到线程的取消状态变为 PTHREAD_CANCEL_ENABLE。
    // 取消类型决定了线程在何时检查取消请求：
    //  PTHREAD_CANCEL_DEFERRED：线程在达到某个取消点时检查取消请求，这是默认类型。
    //  PTHREAD_CANCEL_ASYNCHRONOUS：线程可以在任何时间点被取消。
    // 如果线程在没有取消点的循环中执行，它将不会响应取消请求，除非它调用了 pthread_testcancel 函数，
    //  这个函数会创建一个取消点，允许线程检查并响应挂起的取消请求
    // pthread_cancel(&m_logObj->threadId);

    INFO_PRINT_LOG("init log success:%s\n",m_logObj->logDir);

    return 0;
}

void destinyLog()
{
    if(!m_logObj) return;
    m_logObj->runstatus = 0;
    // 等待线程结束
#ifdef USE_WINDOWS_THREAD
    int ret = WaitForSingleObject(m_logObj->threadId,INFINITE);
    // WAIT_OBJECT_0：对象已进入有信号状态。WAIT_TIMEOUT：等待超时。
    // WAIT_ABANDONED：互斥体被放弃（持有线程终止但未释放）。WAIT_FAILED：调用失败，可用 GetLastError() 获取错误码
    if(ret != WAIT_OBJECT_0){
        FATAL_PRINT_LOG("WaitForSingleObject:%d\n",GetLastError());
        // DWORD  errorCode = GetLastError();
        // LPWSTR messageBuffer = nullptr;
        // DWORD  size = FormatMessageW(
        //     FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        //     NULL,
        //     errorCode,
        //     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        //     (LPWSTR)&messageBuffer,
        //     0,
        //     NULL
        //     );
        // if (size) {
        //     FATAL_PRINT_LOG("22222222222222222 %d:%s\n",
        //                     errorCode,
        //                     QString::fromUtf16(
        //                         (const char16_t *)messageBuffer).toLocal8Bit().constData());
        //     LocalFree(messageBuffer);
        // } else {
        //     FATAL_PRINT_LOG("33333333333333333 %d\n", errorCode);
        // }
    }
#else
    int ret = pthread_join(m_logObj->threadId,NULL);
    if(ret != 0){
        FATAL_PRINT_LOG("pthread_join:%s\n",strerror(errno));
    }
#endif

#ifdef USE_WINDOWS_THREAD
    ret = CloseHandle(m_logObj->threadId);
    if(!ret)
        FATAL_PRINT_LOG("CloseHandle %d\n",GetLastError());
    ret = CloseHandle(m_logObj->mutex); // bool
    if(!ret)
        FATAL_PRINT_LOG("CloseHandle %d\n",GetLastError());
#else
    ret = pthread_mutex_destroy(&m_logObj->mutex);
    if(ret != 0){
        FATAL_PRINT_LOG("pthread_mutex_destroy:%s\n",strerror(errno));
    }
#endif

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
    free(m_logObj);
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
    const char *colorHead = YELLOW;
    switch (level) {
    case LOG_TYPE_ENUM_DEBUG:
        typemsg = "Debug   ";
        colorHead = YELLOW;
        break;
    case LOG_TYPE_ENUM_WARRING:
        typemsg = "Warring ";
        colorHead = CYAN;
        break;
    case LOG_TYPE_ENUM_CRITICAL:
        typemsg = "Critical";
        colorHead = RED;
        break;
    case LOG_TYPE_ENUM_FATAL:
        typemsg = "Fatal   ";
        colorHead = MAGENTA;
        break;
    case LOG_TYPE_ENUM_INFO:
        typemsg = "Info    ";
        colorHead = GREEN;
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
    FILE *out= stdout;
    if(m_printStdout == 2){
        out= stderr;
    }
    fprintf(out,"%s%s:" GREEN "time:%s(%.6lf)" BOLDBLACK "|" RESET BOLDYELLOW "PId:%lu" BOLDBLUE "Tid:%lu" BOLDBLACK "|" RESET CYAN "%s:%d" BLUE "(%s)" MAGENTA "---" RESET "%s",colorHead,typemsg,timechr,timed, ProcessId,ThreadId,file, line,function,buffer);
    fflush(out);

    if(m_logObj){
        if(m_logObj->level <= level){
            int buffer_len = strlen(buffer);
            char tmpbuf[1024];
            char _filename[512]={0};
            TO__FILENAME__(_filename,file);
#ifdef USE_COLOR_LOG
            int len = snprintf(tmpbuf,sizeof(tmpbuf),GREEN "time:%s(%.6lf)" BOLDBLACK "|" RESET BOLDYELLOW "(%lu" BOLDBLUE ":%lu)" CYAN " %s:%d" BLUE "(%s)" MAGENTA "---" RESET,timechr,timed, ProcessId,ThreadId,_filename,line,function);
#else
            int len = snprintf(tmpbuf,sizeof(tmpbuf)," %s(%.6lf)|(%lu:%lu) %s:%d(%s)---",timechr,timed, ProcessId,ThreadId,_filename,line,function);
#endif
            if(len < 0){
                return;
            }
            int typemsg_len = strlen(typemsg);
            char *data;
            if(buffer[buffer_len-1] != '\n'){
                data = (char *)malloc(buffer_len + len + typemsg_len + 2 + 10);
            } else {
                data = (char *)malloc(buffer_len + len + typemsg_len + 1 + 10);
            }
            if(data){
#ifdef USE_COLOR_LOG
                memcpy(data,colorHead,strlen(colorHead)+1);
                strcat(data,typemsg);
#else
                memcpy(data,typemsg,typemsg_len+1);
#endif
                strcat(data,tmpbuf);
                strcat(data,buffer);
                if(buffer[buffer_len-1] != '\n'){
                    strcat(data,"\n");
                }
#ifdef USE_WINDOWS_THREAD
                WaitForSingleObject(m_logObj->mutex,INFINITE);
#else
                pthread_mutex_lock(&m_logObj->mutex);
#endif
                m_logObj->logList->func.append(m_logObj->logList,data);
#ifdef USE_WINDOWS_THREAD
                ReleaseMutex(m_logObj->mutex);
#else
                pthread_mutex_unlock(&m_logObj->mutex);
#endif
            }
        }
    }
}

void writeLogformat(LOG_TYPE_ENUM level,const char* function,const char *file,const int line,const char* format, ...)
{
    va_list arg;
    va_start (arg, format);
    int size = vsnprintf(NULL,0,format,arg);
    va_end (arg);
    if (size < 0) {
        CRITICAL_PRINT_LOG("vsnprintf failed:%s\n", strerror(errno));
        return;
    }
#if 0
    if(size >= 4096){
        char *buffer = (char *)malloc(size+1);
        if(!buffer){
            printf("malloc:%s\n",strerror(errno));
            return;
        }
        va_start (arg, format);
        vsnprintf(buffer,size+1,format,arg);
        va_end (arg);
        writeLog(level,function,file,line,buffer);
        free(buffer);
    } else {
        char buffer[4096];
        va_start (arg, format);
        vsnprintf(buffer,sizeof(buffer),format,arg);
        va_end (arg);
        writeLog(level,function,file,line,buffer);
    }
#else
    if(size > 0){
        char *buffer = (char *)malloc(size+1);
        if(!buffer){
            printf("malloc:%s\n",strerror(errno));
            return;
        }
        va_start (arg, format);
        vsnprintf(buffer,size+1,format,arg);
        va_end (arg);
        writeLog(level,function,file,line,buffer);
        free(buffer);
    }
#endif
}

void writeLogdata(LOG_TYPE_ENUM level,const char* function,const char *file,const int line,const char* buffer)
{
    const char *colorHead = YELLOW;
    if(m_printStdout>0){
        const char *typemsg;
        switch (level) {
        case LOG_TYPE_ENUM_DEBUG:
            typemsg = "Debug   ";
            colorHead = YELLOW;
            break;
        case LOG_TYPE_ENUM_WARRING:
            typemsg = "Warring ";
            colorHead = CYAN;
            break;
        case LOG_TYPE_ENUM_CRITICAL:
            typemsg = "Critical";
            colorHead = RED;
            break;
        case LOG_TYPE_ENUM_FATAL:
            typemsg = "Fatal   ";
            colorHead = MAGENTA;
            break;
        case LOG_TYPE_ENUM_INFO:
            typemsg = "Info    ";
            colorHead = GREEN;
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
        FILE *out = stdout;
        if(m_printStdout == 2){
            out = stderr;
        }
        fprintf(out,"%s%s:" GREEN "time:%s(%.6lf)" BOLDBLACK "|" RESET BOLDYELLOW "PId:%lu" BOLDBLUE "Tid:%lu" BOLDBLACK "|" RESET CYAN "%s:%d" BLUE "(%s)" MAGENTA "---" RESET "%s",colorHead,typemsg,timechr,timed, ProcessId,ThreadId,file, line,function,buffer);
        fflush(out);
    }

    if(m_logObj){
        if(m_logObj->level <= level){
          int len = strlen(buffer)+1;
          char *data = (char *)malloc(len);
          memcpy(data,buffer,len);
#ifdef USE_WINDOWS_THREAD
            WaitForSingleObject(m_logObj->mutex,INFINITE);
#else
            pthread_mutex_lock(&m_logObj->mutex);
#endif
          m_logObj->logList->func.append(m_logObj->logList,data);
#ifdef USE_WINDOWS_THREAD
            ReleaseMutex(m_logObj->mutex);
#else
            pthread_mutex_unlock(&m_logObj->mutex);
#endif
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
