#ifndef _BINDKERNEL_H
#define _BINDKERNEL_H

#ifdef __cplusplus
extern "C" {
#endif

int setProcessCPU(int pid, int cpu);
int getProcessCPU(int pid);
int setCurrentProcessCPU(int cpu);
int getCurrentProcessCPU();

int setThreadCPU(const void *tid, int cpu);
int getThreadCPU(const void *tid);
int setCurrentThreadCPU(int cpu);
int getCurrentThreadCPU();

#ifdef __cplusplus
}
#endif

#endif 