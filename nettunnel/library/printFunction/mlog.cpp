#include "mlog.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>

#if defined(WIN32) || defined(WIN64)
#include <processthreadsapi.h>
#endif

#if defined(__GNUC__)
#include <unistd.h> // getpid();
#endif

#if defined(__unix__)
#include <sys/syscall.h>
#endif

MLog::MLog() {
    isrunning = false;
    isAddLineFeed = false;
    m_thread = new thread(workthread,this);
}

MLog::~MLog()
{
    isrunning = false;
    //if(m_thread->joinable())
    m_thread->join();
    delete m_thread;
}

void MLog::start(const string &dir,unsigned int nflushMs,unsigned int nfileLogMaxSize)
{
    if(!isrunning){
        logDir = dir;
        flushMs = nflushMs;
        fileLogMaxSize = nfileLogMaxSize;
        isrunning = true;
    }
}

void MLog::writeLog(LOG_TYPE_ENUM nlevel, const char *function, const char *file, const int line, const char *data)
{
    const char *typemsg;
    switch (nlevel) {
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
    unsigned long ThreadId = 0; // this_thread::get_id();
#if defined(WIN32) || defined(WIN64)
    ProcessId = GetCurrentProcessId();
    ThreadId = GetCurrentThreadId();
#endif
#if defined(__unix__)
    ProcessId = getpid();
    //ThreadId = gettid();
    ThreadId = syscall(SYS_gettid);
#endif

    // 获取当前时间点
    //auto now = std::chrono::system_clock::now();now.time_since_epoch();
    // 转换为time_t类型
    //std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    // 输出当前时间
    //std::cout << "Current time: " << std::ctime(&now_c) << std::endl;

    char timechr[64];
    __get_printfTime(timechr,sizeof(timechr));
    double timed = __get_printfTime_d();
    if(isprintStdout){
        fprintf(stdout,YELLOW "%s:" GREEN "time:%s(%.6lf)" BOLDBLACK "|" RESET BOLDYELLOW "PId:%lu" BOLDBLUE "Tid:%lu" BOLDBLACK "|" RESET CYAN "%s:%d" BLUE "(%s)" MAGENTA "---" RESET "%s\n",typemsg,timechr,timed, ProcessId,ThreadId,file, line,function,data);
        fflush(stdout);
    }

    if(level <= nlevel){
        char tmpbuf[1024];
        snprintf(tmpbuf,sizeof(tmpbuf)," %s(%.6lf)|(%lu:%lu) %s:%d(%s) ",timechr,timed, ProcessId,ThreadId,file,line,function);
        string str;
        str.append(typemsg);
        str.append(tmpbuf);
        str.append(data);
        if(isAddLineFeed && (str.at(str.length()-1) != '\n')){
            str.append("/n");
        }

        m_mutex.lock();
        m_list.push_back(str);
        m_mutex.unlock();
    }
}

void MLog::workthread(MLog *mlog)
{
    double time = __get_printfTime_d();
    unsigned int filelogsize = 0;
    while(mlog->isrunning){
        if(mlog->m_list.size() == 0) {
            chrono::milliseconds(10);
            //continue;
        }
        ofstream outstream;
        outstream.open(mlog->logDirFileName,ios::out);
        if(outstream.is_open() && mlog->m_list.size() > 0){
            string str =  mlog->m_list.front();
            mlog->m_list.pop_front();
            //outstream << str;
            outstream.write(str.data(),str.size());
            outstream.flush();
            outstream.close();
        }
    }
    //mlog.m_thread.get_id();
    //mlog.m_thread.detach(); // 调用 detach 函数之后：this 不再代表任何的线程执行实例;joinable() == false;get_id() == std::thread::id()
}
