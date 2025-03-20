#ifndef FINALLYRECOVERY_H
#define FINALLYRECOVERY_H

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 计算结构体成员的偏移量
 * @param type 结构体类型
 * @param member 结构体成员名
 * @return 结构体成员的偏移量
 */
#define GETVARIATEOFFSET(type, member) (size_t)(&((type*)0)->member)

/**
 * @brief 获取结构体成员的类型
 * @param type 结构体类型
 * @param member 结构体成员名
 * @return 结构体成员的类型
 */
#define GETVARIATETYPE(type, member) (typeof(((type*)0)->member))

/**
 * @brief 获取结构体成员的大小
 * @param type 结构体类型
 * @param member 结构体成员名
 * @return 结构体成员的大小
 */
#define GETVARIATESIZE(type, member) (sizeof(((type*)0)->member))

/**
 * @brief 获取结构体大小
 * @param type 结构体类型
 * @return 结构体的大小
 */
#define GETSTRUCTSIZE(type) (sizeof(type))

/**
 * @brief 获取数组个数
 * @param type 数组类型
 * @return 数组的元素个数
 */
#define GETSTRUCTCNT(type) (sizeof(type)/sizeof(type[0]))

/**
 * @brief 清理函数，用于释放通过malloc分配的内存
 * 
 * GNU中 C语言 实现RALL资源自动释放,适用于gcc,clang,不适用语msvc,
*  当变量的生命周期结束时，会自动调用函数，实现内存释放。
* 
*  注解:__attribute__((cleanup(cleanup_func_free))) void *name ,
*  这里传入的是变量的地址(&name)作为参数给cleanup_func_free函数
 * @param data 指向需要释放的内存的指针的指针
 */
static void cleanup_func_free(void **data)
{
    if (*data) {
        //printf("cleanup_func_free:%p\n", *data);
        free(*data);
    }
}

/**
 * @brief 自动分配内存并在变量生命周期结束时自动释放
 * @param name 变量名
 * @param size 分配的内存大小
 */
#define AUTOPTRNEW(name, size) __attribute__((cleanup(cleanup_func_free))) void *name = malloc(size)

/**
 * @brief 自动分配指定类型和大小的内存并在变量生命周期结束时自动释放
 * @param type 分配的内存类型
 * @param name 变量名
 * @param size 分配的内存大小
 */
#define AUTOPTRTYPENEWSIZE(type, name, size) __attribute__((cleanup(cleanup_func_free))) void *void##_name = malloc(size); type *name = (type *)void##_name

/**
 * @brief 自动分配指定类型和数量的内存并在变量生命周期结束时自动释放
 * @param type 分配的内存类型
 * @param name 变量名
 * @param cnt 分配的元素数量
 */
#define AUTOPTRTYPENEWCNT(type, name, cnt) __attribute__((cleanup(cleanup_func_free))) void *void##_name = malloc(cnt * sizeof(type)); type *name = (type *)void##_name

/**
 * @brief 定义一个自动释放的指针类型
 * @param type 指针指向的类型
 */
#define AUTOPTRTYPE(type) __attribute__((cleanup(cleanup_func_free))) typeof(type) *

/**
 * @brief 定义一个自动释放的void指针
 */
#define AUTOPTR __attribute__((cleanup(cleanup_func_free))) void *

#if 1
#include <pthread.h>

/**
 * @brief 清理函数，用于解锁互斥锁
 * @param mute 指向互斥锁的指针的指针
 */
static void cleanup_func_mutex(void **mute)
{
    if (*mute) {
        //printf("cleanup_func_mute:%p\n", *mute);
        pthread_mutex_unlock((pthread_mutex_t *)*mute);
    }
}

/**
 * @brief 自动锁定互斥锁并在变量生命周期结束时自动解锁
 * @param mutex 已经初始化的互斥锁
 */
#define AUTOPTR_MUTEX(mutex) __attribute__((cleanup(cleanup_func_mutex))) void *_mutex = &mutex; pthread_mutex_lock((pthread_mutex_t*)_mutex)
#endif

#ifdef __cplusplus
}
#endif

#endif // FINALLYRECOVERY_H