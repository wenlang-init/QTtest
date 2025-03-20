#include "printFunction.h"
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

#if defined(__GNUC__)
#include <unistd.h> // getpid();
#endif

#if defined(__unix__)
#include <sys/syscall.h>
// gettid() // 线程ID
// syscall(SYS_gettid)
#endif

//#if defined(__ANDROID__)

double __get_printfTime_d()
{
#if defined(__GNUC__)
    // //#include <sys/time.h>
    // struct timeval tv;
    // gettimeofday(&tv, NULL);
    // double t = tv.tv_sec + (double)tv.tv_usec / (1000 * 1000);
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    double t = ts.tv_sec + ts.tv_nsec*(double)1.0/(1000*1000*1000);
#else
    SYSTEMTIME st;
    //GetSystemTime(&st);
    GetLocalTime(&st);
    struct tm temptm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth - 1, st.wYear - 1900, st.wDayOfWeek, 0, 0};
    time_t timer = mktime(&temptm);
    double t = timer*1.0 + st.wMilliseconds*(double)1.0/1000;
#endif
    return t;
}

const char *__get_printfTime(char *cur_time,int size)
{
    if(!cur_time){
        return cur_time;
    }
    //static char cur_time[64];

    // struct tm
    // {
    //     int tm_sec;         /* 秒，范围从 0 到 59      */
    //     int tm_min;         /* 分，范围从 0 到 59      */
    //     int tm_hour;        /* 小时，范围从 0 到 23     */
    //     int tm_mday;        /* 一月中的第几天，范围从 1 到 31    */
    //     int tm_mon;         /* 月，范围从 0 到 11(注意)  */
    //     int tm_year;        /* 自 1900 年起的年数      */
    //     int tm_wday;        /* 一周中的第几天，范围从 0 到 6 */
    //     int tm_yday;        /* 一年中的第几天，范围从 0 到 365   */
    //     int tm_isdst;       /* 夏令时               */
    // };
    time_t timer = time(0);
    struct tm Now;
#if defined(__GNUC__) && defined(__unix__)
    if(localtime_r(&timer,&Now)){
        snprintf(cur_time, size,"%d-%02d-%02d %02d:%02d:%02d", 1900+Now.tm_year, Now.tm_mon+1, Now.tm_mday, Now.tm_hour, Now.tm_min, Now.tm_sec);
    }else{
        snprintf(cur_time, size,"get localtime failed");
    }
#else
    if(0 > localtime_s(&Now,&timer)){
        snprintf(cur_time, size,"get localtime failed");
    } else {
        snprintf(cur_time, size,"%d-%02d-%02d %02d:%02d:%02d", 1900+Now.tm_year, Now.tm_mon+1, Now.tm_mday, Now.tm_hour, Now.tm_min, Now.tm_sec);
    }
#endif
    return cur_time;
}

void printColour(const char *color,const char *data)
{
    if(color == NULL || data == NULL)return;
    fprintf(stdout,"%s%s" RESET,color,data);
    fflush(stdout);
}

void printDebugMsg(LOG_TYPE_ENUM type,const char* function,const char *file,const int line,const char* format, ...)
{
    #define ADTA_MAXSIZE 4096
    char buffer[ADTA_MAXSIZE + 1]={0};
    va_list arg;
    va_start (arg, format);
    vsnprintf(buffer, ADTA_MAXSIZE, format, arg);
    va_end (arg);
    static const char *typemsg = "";
    switch (type) {
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

    FILE *out;
    if(type == LOG_TYPE_ENUM_DEBUG || type == LOG_TYPE_ENUM_INFO){
        out = stdout;
    } else {
        out = stderr;
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

    char cur_time[64];
    fprintf(out,YELLOW "%s:" GREEN "time:%s(%.6lf)" BOLDBLACK "|" RESET BOLDYELLOW "PId:%lu" BOLDBLUE "Tid:%lu" BOLDBLACK "|" RESET CYAN "%s:%d" BLUE "(%s)" MAGENTA "---" RESET "%s\n",typemsg,__get_printfTime(cur_time,sizeof(cur_time)),__get_printfTime_d(), ProcessId,ThreadId,file, line,function,buffer);
    fflush(out);
}
