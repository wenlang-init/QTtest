#pragma once

#ifndef _MEMORYPOOL_H
#define _MEMORYPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _memoryPoolObj MemoryPoolObj;
typedef struct _memoryNode MemoryNode;

struct _memoryNode {
    unsigned long isMalloc; // 保存当前的是否是申请内存状态
    unsigned long size; // 内存大小
    unsigned long index; // 在内存池中的位置
    void *data; // 内存数据
};

struct _memoryPoolObj {
    char *memoryPoolData; // 内存池数据
    char *memoryMlockStatus; // 内存块使用状态,0表示未使用
    unsigned long blockNumber; // 内存块数
    unsigned long usedBlockNumber; // 已经使用的内存块数

    unsigned long blockNumberStep; // 每次增加的内存块数
    unsigned long blockNumberSize; // 每个内存块的大小

    unsigned long freeNodeNumber; // 存储已经申请的内存的个数
    unsigned long freeNodeArrayNumber; // 存储申请的内存状态的个数(不可变,为最大可申请内存个数)
    MemoryNode *freeNodeArray; // 存储申请的内存状态

    struct _memoryPoolObjFunction {
        MemoryNode *(*malloc)(MemoryPoolObj *obj,unsigned long size); // 申请内存
        void (*free)(MemoryPoolObj *obj,MemoryNode *p); // 释放内存
        MemoryPoolObj * (*clear)(MemoryPoolObj *obj); // 清空线程池
        void (*destroy)(MemoryPoolObj *obj); // 销毁线程池
    }func;
};

/**
 @brief 初始化内存池。initMemoryPool(1000,1024,100000)
 @param blockNumberStep-每次增加的内存块的个数
 @param blockNumberSize-每个内存的大小
 @param maxMemoryCount-最大可同时存在的内存个数
 */
MemoryPoolObj *initMemoryPool(unsigned long blockNumberStep,
                              unsigned long blockNumberSize,
                              unsigned long maxMemoryCount);

/**
 @brief 销毁线程池
 @param obj-线程池对象
 */
void destroyMemoryPool(MemoryPoolObj *obj);

/**
 @brief 申请内存
 @param obj-线程池对象
 @param size-内存大小
 @return 成功返回内存指针,失败返回NULL
 */
MemoryNode *memoryPoolMalloc(MemoryPoolObj *obj,unsigned long size);

/**
 @brief 释放内存
 @param obj-线程池对象
 @param p-内存指针
 */
void memoryPoolFree(MemoryPoolObj *obj,MemoryNode *p);

/**
 @brief 清空线程池,返回新的线程池对象
 @param obj-线程池对象
 @return 返回新的线程池对象
 */
MemoryPoolObj* memoryPoolClear(MemoryPoolObj *obj);

#ifdef __cplusplus
}
#endif

#endif // ifndef _MEMORYPOOL_H
