#ifndef FINALLYRECOVERY_H
#define FINALLYRECOVERY_H

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// 计算变量大小，同sizeof
// int a,b[2];sizeof_T(a);sizeof_V(a);sizeof_V(b);
#define sizeof_T(type) ((szie_t)(type*)0 + 1)
#define sizeof_V(type) ((szie_t)(&type+1) - (szie_t)(&type))

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
#define AUTOPTRTYPENEWSIZE(type, name, size) __attribute__((cleanup(cleanup_func_free))) void *void_##name = malloc(size); type *name = (type *)void_##name

/**
 * @brief 自动分配指定类型和数量的内存并在变量生命周期结束时自动释放
 * @param type 分配的内存类型
 * @param name 变量名
 * @param cnt 分配的元素数量
 */
#define AUTOPTRTYPENEWCNT(type, name, cnt) __attribute__((cleanup(cleanup_func_free))) void *void_##name = malloc(cnt * sizeof(type)); type *name = (type *)void_##name

/**
 * @brief 定义一个自动释放的指针类型
 * @param type 指针指向的类型
 */
#define AUTOPTRTYPE(type) __attribute__((cleanup(cleanup_func_free))) typeof(type) *

/**
 * @brief 定义一个自动释放的void指针
 */
#define AUTOPTR __attribute__((cleanup(cleanup_func_free))) void *

#if defined(__GNUC__)
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

/**
 * @brief 在main前执行的函数
 * 会在ELF文件中添加段.ctors。当动态库或程序在加载时，检查是否存在这个段，如果存在执行对应的代码
 * @param functionName 函数名
 * @param priority 优先级，数值越小优先执行
 * 使用方法:BEFOREMAIN(test,100){printf("This is a test function that runs before main.\n");}
 */
#define BEFOREMAIN(functionName,priority)__attribute((constructor(priority))) void functionName()

/**
 * @brief 在main后执行的函数. atexit(functionname)注册的函数在main结束后执行,先注册的后执行
 * 会在ELF文件中添加段.dtors。当动态库或程序在加载时，检查是否存在这个段，如果存在执行对应的代码
 * @param functionName 函数名
 * @param priority 优先级，数值越大优先执行
 * 使用方法:AFTERMAIN(test,101){printf("This is a test function that runs after main.\n");}
 */
#define AFTERMAIN(functionName,priority)__attribute((destructor(priority))) void functionName()

/**
 * @brief 将变量放在指定的段中
 * @param type 变量类型
 * @param name 变量名
 * @param sectionname 段名
 * 使用方法:VARIABLERESERVESECTION(int,g_b,".data")  //将int的g_b指定放在.data段
 * VARIABLERESERVESECTION(int,g_b,".bss")  //将int的g_b指定放在.bss段
 * 注:在Linux中，.data段是用来存储初始化的全局变量和静态变量的，
 * 而.bss段是用来存储未初始化的全局变量和静态变量的。
 * 通过将变量放在特定的段中，可以更好地管理内存和优化程序性能。
 */
#define VARIABLERESERVESECTION(type,name,sectionname) type name __attribute__((section(sectionname)));

#endif

#ifdef __cplusplus
}
#endif

#endif // FINALLYRECOVERY_H
