#include "memorypool.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "printFunction.h"

MemoryPoolObj *initMemoryPool(unsigned long blockNumberStep,
                              unsigned long blockNumberSize,
                              unsigned long maxMemoryCount)
{
    MemoryPoolObj *obj = (MemoryPoolObj *)malloc(sizeof(MemoryPoolObj));
    if(!obj){
        FATAL_PRINT_LOG("malloc:%s\n",strerror(errno));
        return NULL;
    }
    obj->blockNumberStep = blockNumberStep;
    obj->blockNumberSize = blockNumberSize;

    obj->usedBlockNumber = 0;
    obj->blockNumber = obj->blockNumberStep;

    obj->freeNodeNumber = 0;
    obj->freeNodeArrayNumber = maxMemoryCount;

    obj->func.malloc = memoryPoolMalloc;
    obj->func.free = memoryPoolFree;
    obj->func.clear = memoryPoolClear;
    obj->func.destroy = destroyMemoryPool;

    obj->memoryPoolData = (char*)malloc(obj->blockNumber*obj->blockNumberSize);
    if(!obj->memoryPoolData){
        FATAL_PRINT_LOG("malloc:%s\n",strerror(errno));
        goto end1;
    }

    obj->memoryMlockStatus = (char*)malloc(obj->blockNumber);
    if(!obj->memoryMlockStatus){
        FATAL_PRINT_LOG("malloc:%s\n",strerror(errno));
        goto end2;
    }
    memset(obj->memoryMlockStatus,0,obj->blockNumber);

    unsigned long fnasize = sizeof(MemoryNode)*obj->freeNodeArrayNumber;
    obj->freeNodeArray = (MemoryNode *)malloc(fnasize);
    if(!obj->freeNodeArray){
        FATAL_PRINT_LOG("malloc:%s\n",strerror(errno));
        goto end3;
    }
    memset(obj->freeNodeArray,0,fnasize);

    INFO_PRINT_LOG("init memory pool success\n");

    return obj;

    end3:
    free(obj->memoryMlockStatus);
    end2:
    free(obj->memoryPoolData);
    end1:
    free(obj);
    return NULL;
}

void destroyMemoryPool(MemoryPoolObj *obj)
{
    if(!obj)return;
    for(unsigned long i=0;i<obj->freeNodeArrayNumber;i++){
        if(obj->freeNodeArray[i].isMalloc && obj->freeNodeArray[i].size > obj->blockNumberSize){
            free(obj->freeNodeArray[i].data);
        }
    }
    free(obj->freeNodeArray);
    free(obj->memoryMlockStatus);
    free(obj->memoryPoolData);
    free(obj);
}

static int memoryPoolExtension(MemoryPoolObj *obj)
{
    if(!obj){
        return -1;
    }
    // 当内存扩展(减少内存)后，外部得到的指针P未变，内部改变了，无法告诉外部，造成无法free
    // 所以需要实现不改变内存地址的办法
    // 所以内存池不能改变,也可以这样：
    // 内存池可以改变，但是node不能改变，结果保存到node，外部得到的也必须为node
    if(0){
        unsigned long needSizeFNA = obj->freeNodeArrayNumber;
        if(needSizeFNA <= obj->freeNodeNumber){
            needSizeFNA += obj->blockNumberStep;
            // 扩容
            MemoryNode *p = realloc(obj->freeNodeArray,needSizeFNA*sizeof(MemoryNode));
            if(!p){
                FATAL_PRINT_LOG("realloc:%s\n",strerror(errno));
                return -11;
            }
            memset(p+obj->freeNodeArrayNumber,0,obj->blockNumberStep*sizeof(MemoryNode));
            obj->freeNodeArray = p;
            obj->freeNodeArrayNumber = needSizeFNA;
        } else {
            // 减容
            needSizeFNA = (obj->freeNodeArrayNumber - obj->freeNodeNumber)/obj->blockNumberStep;
            if(needSizeFNA > 0 && obj->freeNodeArrayNumber > obj->blockNumberStep){
                needSizeFNA = obj->freeNodeArrayNumber - obj->blockNumberStep*needSizeFNA;

                // 整理内存数据(移动，使未使用的被消除)
                unsigned long _cnt = 0;
                for(unsigned long i=0;i<obj->freeNodeArrayNumber;i++){
                    // 已经确定处理了所有数据，直接结束循环
                    if(_cnt >= obj->freeNodeNumber)break;
                    if(obj->freeNodeArray[i].isMalloc){
                        _cnt++;
                        continue;
                    }
                    for(unsigned long j=i+1;i<obj->freeNodeArrayNumber;j++){
                        if(obj->freeNodeArray[j].isMalloc){
                            memcpy(&obj->freeNodeArray[i],
                                   &obj->freeNodeArray[j],
                                   sizeof(MemoryNode));
                            obj->freeNodeArray[j].isMalloc = 0;
                            _cnt++;
                            break;
                        }
                    }
                }

                MemoryNode *p = realloc(obj->freeNodeArray,needSizeFNA*sizeof(MemoryNode));
                if(!p){
                    FATAL_PRINT_LOG("realloc:%s,%lu,%lu\n",strerror(errno),needSizeFNA,sizeof(MemoryNode));
                    return -12;
                }
                obj->freeNodeArray = p;
                obj->freeNodeArrayNumber = needSizeFNA;
            }
        }
    }
    unsigned long needSize = obj->blockNumber;
    if(needSize <= obj->usedBlockNumber){
        needSize = obj->blockNumber + obj->blockNumberStep;
        // 扩大内存
        char *p = realloc(obj->memoryPoolData,needSize*obj->blockNumberSize);
        if(!p){
            FATAL_PRINT_LOG("realloc:%s,%lu,%lu\n",strerror(errno),needSize,obj->blockNumberSize);
            return -1;
        }
        obj->memoryPoolData = p;
        char *q = realloc(obj->memoryMlockStatus,needSize);
        if(!q){
            // 这里失败后obj->blockNumber未增加，obj->memoryPoolData不影响后续处理
            FATAL_PRINT_LOG("realloc:%s\n",strerror(errno));
            return -2;
        }
        memset(q+obj->blockNumber,0,obj->blockNumberStep);
        obj->memoryMlockStatus = q;
        obj->blockNumber = needSize;
    } else {
        needSize = (obj->blockNumber - obj->usedBlockNumber)/obj->blockNumberStep;
        if(needSize > 0 && obj->blockNumber > obj->blockNumberStep){
            needSize = obj->blockNumber - needSize*obj->blockNumberStep;
            // 整理内存数据(移动，使未使用的被消除)
            unsigned long _cnt = 0;
            for(unsigned long i=0;i<obj->blockNumber;i++){
                if(_cnt >= obj->usedBlockNumber)break;
                if(obj->memoryMlockStatus[i]){
                    _cnt++;
                    continue;
                }
                for(unsigned long j=i+1;i<obj->blockNumber;j++){
                    if(obj->memoryMlockStatus[j]){
                        // 拷贝到i
                        unsigned long index1 = i*obj->blockNumberSize;
                        unsigned long index2 = j*obj->blockNumberSize;
                        memcpy(obj->memoryPoolData+index1,
                               obj->memoryPoolData+index2,
                               obj->blockNumberSize);
                        obj->memoryMlockStatus[i] = 1;
                        obj->memoryMlockStatus[j] = 0;
                        // freeNodeArray数据修改
                        for(unsigned long k=0;k<obj->freeNodeArrayNumber;k++){
                            if(obj->freeNodeArray[k].isMalloc
                                    &&obj->freeNodeArray[k].size<= obj->blockNumberSize
                                    && obj->freeNodeArray[k].index == j){
                                obj->freeNodeArray[k].index = i;
                                obj->freeNodeArray[k].data = obj->memoryPoolData+index1;
                                break;
                            }
                        }
                        _cnt++;
                        break;
                    }
                }
            }
            char *p = realloc(obj->memoryPoolData,needSize*obj->blockNumberSize);
            if(!p){
                FATAL_PRINT_LOG("realloc:%s\n",strerror(errno));
                return -3;
            }
            obj->memoryPoolData = p;
            obj->blockNumber = needSize;
            char *q = realloc(obj->memoryMlockStatus,needSize);
            if(!q){
                // 这里失败后obj->blockNumber已经减少，不影响obj->memoryMlockStatus后续处理
                FATAL_PRINT_LOG("realloc:%s\n",strerror(errno));
                return -4;
            }
            obj->memoryMlockStatus = q;
        }
    }

    return 0;
}

MemoryNode *memoryPoolMalloc(MemoryPoolObj *obj,unsigned long size)
{
    if(!obj)return NULL;

    obj->freeNodeNumber++;
    if(obj->freeNodeNumber > obj->freeNodeArrayNumber){
        FATAL_PRINT_LOG("Exceeding the maximum application limit\n");
        obj->freeNodeNumber--;
        return NULL;
    }

    if(size > obj->blockNumberSize){
        void *p = malloc(size);
        if(!p){
            FATAL_PRINT_LOG("malloc:%s\n",strerror(errno));
            return NULL;
        }

        for(unsigned long i=0;i<obj->freeNodeArrayNumber;i++){
            if(!obj->freeNodeArray[i].isMalloc){
                obj->freeNodeArray[i].isMalloc = 1;
                obj->freeNodeArray[i].data = p;
                obj->freeNodeArray[i].size = size;
                return &obj->freeNodeArray[i];
            }
        }
        FATAL_PRINT_LOG("error !!!!!\n");
        free(p);
        obj->freeNodeNumber--;
        return NULL;
    } else {
        obj->usedBlockNumber++;
        if(0!=memoryPoolExtension(obj)){
            obj->usedBlockNumber--;
            obj->freeNodeNumber--;
            return NULL;
        }
        MemoryNode *node = NULL;
        for(unsigned long i=0;i<obj->freeNodeArrayNumber;i++){
            if(!obj->freeNodeArray[i].isMalloc){
                node = &obj->freeNodeArray[i];
                break;
            }
        }
        if(NULL == node){
            FATAL_PRINT_LOG("failed !!!!!\n");
            obj->usedBlockNumber--;
            obj->freeNodeNumber--;
            return NULL;
        }
        for(unsigned long i=0;i<obj->blockNumber;i++){
            if(!obj->memoryMlockStatus[i]){
                obj->memoryMlockStatus[i] = 1;
                node->isMalloc = 1;
                node->index = i;
                node->data = &obj->memoryPoolData[i*obj->blockNumberSize];
                node->size = size;
                return node;
            }
        }
        FATAL_PRINT_LOG("error !!!!!\n");
        obj->usedBlockNumber--;
        obj->freeNodeNumber--;
        return NULL;
    }
}

void memoryPoolFree(MemoryPoolObj *obj,MemoryNode *p)
{
    if(!obj)return;
    for(unsigned long i=0;i<obj->freeNodeArrayNumber;i++){
        // FATAL_PRINT_LOG("(%lu,%lu)%lu,%lu,%p,%p\n",obj->usedBlockNumber,obj->freeNodeNumber,
        //                 i,obj->freeNodeArray[i].isMalloc
        //                 ,&obj->freeNodeArray[i]
        //                 ,p);
        if(obj->freeNodeArray[i].isMalloc && &obj->freeNodeArray[i] == p){
            if(obj->freeNodeArray[i].size > obj->blockNumberSize){
                obj->freeNodeArray[i].isMalloc = 0;
                free(p->data);
                obj->freeNodeNumber--;
                //memoryPoolExtension(obj);
            } else {
                obj->freeNodeArray[i].isMalloc = 0;
                obj->usedBlockNumber--;
                obj->freeNodeNumber--;
                obj->memoryMlockStatus[i] = 0;
                memoryPoolExtension(obj);
            }
            break;
        }
    }
}

MemoryPoolObj* memoryPoolClear(MemoryPoolObj *obj)
{
    if(!obj)return NULL;
    unsigned long blockNumberStep = obj->blockNumberStep;
    unsigned long blockNumberSize = obj->blockNumberSize;
    unsigned long freeNodeArrayNumber = obj->freeNodeArrayNumber;
    destroyMemoryPool(obj);
    return initMemoryPool(blockNumberStep,blockNumberSize,freeNodeArrayNumber);
}
