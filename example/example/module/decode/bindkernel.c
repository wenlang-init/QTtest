#include <stdio.h>
#if defined(__linux__) && !defined(__ANDROID_API__)
//#if defined(__unix__)
//#define _GNU_SOURCE
#define __USE_GNU
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#endif

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

int setProcessCPU(int pid, int cpu)
{
 #if defined(__linux__) && !defined(__ANDROID_API__)
    int cpukercount = sysconf(_SC_NPROCESSORS_ONLN);   //CPU核心总数
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(cpu, &mask);
    // pid == 0 表示当前进程
    if (sched_setaffinity(pid, sizeof(cpu_set_t), &mask) != 0){
        perror("sched_setaffinity");
        return -1;
    }
    return 0;
#endif
    return -1;
}
int getProcessCPU(int pid)
{
#if defined(__linux__) && !defined(__ANDROID_API__)
    int cpukercount = sysconf(_SC_NPROCESSORS_ONLN);   //CPU核心总数
    cpu_set_t mask;
    CPU_ZERO(&mask);
    if(sched_getaffinity(pid, sizeof(mask), &mask) != 0){
        perror("sched_getaffinity");
        return -1;
    }
    for (int i = 0; i < cpukercount; i++)
        if (CPU_ISSET(i, &mask))
            return i;
    return -1;
#endif
    return -1;
}
int setCurrentProcessCPU(int cpu)
{
#if defined(__linux__) && !defined(__ANDROID_API__)
    int pid = getpid();
    return setProcessCPU(pid, cpu);
#endif
    return -1;
}
int getCurrentProcessCPU()
{
#if defined(__linux__) && !defined(__ANDROID_API__)
    int pid = 0;//getpid();
    return getProcessCPU(pid);
#endif
    return -1;
}
int setThreadCPU(const void *tid, int cpu)
{
#if defined(__linux__) && !defined(__ANDROID_API__)
    pthread_t threadid = *(pthread_t*)tid;
    int cpukercount = sysconf(_SC_NPROCESSORS_ONLN);   //CPU核心总数
    if (cpu >= cpukercount)
        return -1;
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(cpu, &mask);
    // threadid必须是pthread_create创建的线程，否则pthread_setaffinity_np会失败，
    // 并且线程必须没有结束，否则pthread_setaffinity_np会失败。甚至崩溃。
    if (pthread_setaffinity_np(threadid, sizeof(mask), &mask) != 0){
        perror("pthread_setaffinity_np");
        return -1;
    }
    return 0;
#endif
#if defined(WIN32) || defined(WIN64)
    HANDLE thread = *(HANDLE *)tid;
    if(SetThreadAffinityMask(thread, 1 << cpu) == 0){
        perror("SetThreadAffinityMask");
        return -1;
    }
    return 0;
#endif
    return -1;
}

int getThreadCPU(const void *tid)
{
#if defined(__linux__) && !defined(__ANDROID_API__)
    pthread_t threadid = *(pthread_t*)tid;
    int cpukercount = sysconf(_SC_NPROCESSORS_ONLN);   //CPU核心总数
    cpu_set_t mask;
    CPU_ZERO(&mask);
    if (pthread_getaffinity_np(threadid, sizeof(mask), &mask) != 0){
        perror("pthread_getaffinity_np");
        return -1;
    }
    for (int i = 0; i < cpukercount; i++)
        if (CPU_ISSET(i, &mask))
            return i;
    return -1;
#endif
#if 0 //defined(WIN32) || defined(WIN64)
    HANDLE thread = *(HANDLE *)tid;
    uint64_t threadAffinityMask;
    if(GetThreadAffinityMask(thread, &threadAffinityMask) != c_partyErrorSuccess){
        //GetErrorMessage(errcode, errmsg, sizeof(errmsg));
        perror("GetThreadAffinityMask");
        return -1;
    }
    for (int i = 0; i < sizeof(threadAffinityMask)*8; i++){
        if(threadAffinityMask &( 1 << i)){
            return i;
        }
    }
    return -1;
#endif
    return -1;
}
int setCurrentThreadCPU(int cpu)
{
#if defined(__unix__) && !defined(__ANDROID_API__)
    int tid = pthread_self(); // syscall(SYS_gettid);
    return setThreadCPU(&tid, cpu);
#endif
#if defined(WIN32) || defined(WIN64)
    HANDLE tid = GetCurrentThread();
    return setThreadCPU(&tid, cpu);
#endif
    return -1;
}

int getCurrentThreadCPU()
{
#if defined(__unix__) && !defined(__ANDROID_API__)
    int tid = pthread_self();
    return getThreadCPU(&tid);
#endif
#if defined(WIN32) || defined(WIN64)
    HANDLE tid = GetCurrentThread();
    return getThreadCPU(&tid);
#endif
    return -1;
}