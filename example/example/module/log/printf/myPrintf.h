#pragma once

#define CLEAR "\e[2j"            // 清屏
#define CLRLINE "\r\e[K"         // or "\e[K\r" 清除从光标到行尾的内容
#define RESET       "\033[0m"    // 恢复终端默认属性
#define BLACK       "\033[30m"   /* Black 黑色 */
#define RED         "\033[31m"   /* Red 红色 */
#define GREEN       "\033[32m"   /* Green 绿色 */
#define YELLOW      "\033[33m"   /* Yellow 黄的 */
#define BLUE        "\033[34m"   /* Blue 蓝色 */
#define MAGENTA     "\033[35m"   /* Magenta 品红 */
#define CYAN        "\033[36m"   /* Cyan 青色 */
#define WHITE       "\033[37m"   /* White 白色 */
#define BOLDBLACK   "\033[1;30m" /* Bold Black 粗黑 */
#define BOLDRED     "\033[1;31m" /* Bold Red 粗红 */
#define BOLDGREEN   "\033[1;32m" /* Bold Green */
#define BOLDYELLOW  "\033[1;33m" /* Bold Yellow */
#define BOLDBLUE    "\033[1;34m" /* Bold Blue */
#define BOLDMAGENTA "\033[1;35m" /* Bold Magenta */
#define BOLDCYAN    "\033[1;36m" /* Bold Cyan */
#define BOLDWHITE   "\033[1;37m" /* Bold White */

#define PRINT_BUFFER_SIZE 4096
char printf_info[PRINT_BUFFER_SIZE];

// 返回写入的字符数
int printFormat(const char *format,
                ...);

// 返回写入的字符数
int printFormatBuffer(char        *buffer,
                      unsigned int bufferSize,
                      const char  *format,
                      ...);

#define MY_PRINTF(__format, ...) printFormat(__format, ## __VA_ARGS__)
#define MY_PRINTF_INFO() MY_PRINTF(GREEN "%s:%d %s" RED "---" RESET, \
                                   __FUNCTION__,                     \
                                   __LINE__,                         \
                                   __FILE__)

#define MY_PRINTF_BUFFER(buffer, bufferSize, __format, ...) printFormatBuffer( \
        buffer,                                                                \
        bufferSize,                                                            \
        __format,                                                              \
        ## __VA_ARGS__)
#define MY_PRINTF_BUFFER_INFO(buffer, bufferSize)      \
    MY_PRINTF_BUFFER(buffer,                           \
                     bufferSize,                       \
                     GREEN "%s:%d %s" RED "---" RESET, \
                     __FUNCTION__,                     \
                     __LINE__,                         \
                     __FILE__)

#define MY_PRINTF_BUFFER_DEBUG(buffer, bufferSize, __format, ...)  \
    do {                                                           \
        int len = MY_PRINTF_BUFFER_INFO(buffer, bufferSize);       \
        if (len < 0) {                                             \
            len = 0;                                               \
        }                                                          \
        if (len > bufferSize) {                                    \
            len = bufferSize;                                      \
        }                                                          \
        MY_PRINTF_BUFFER(buffer + len, bufferSize - len, __format, \
                         ## __VA_ARGS__);                          \
    } while (0)

#define MY_PRINTF_DEBUG(__format, ...)            \
    do {                                          \
        int len = MY_PRINTF_INFO();               \
        if (len < 0) {                            \
            len = 0;                              \
        }                                         \
        if (len > PRINT_BUFFER_SIZE) {            \
            len = PRINT_BUFFER_SIZE;              \
        }                                         \
        MY_PRINTF_BUFFER(printf_info + len,       \
                         PRINT_BUFFER_SIZE - len, \
                         __format,                \
                         ## __VA_ARGS__);         \
    } while (0)

// #define MY_PRINTF_DEBUG(__format, ...) \
// do { \
//     MY_PRINTF(GREEN "%s:%d %s" RED "---"
// RESET,__FUNCTION__,__LINE__,__FILE__); \
//     MY_PRINTF(__format, ## __VA_ARGS__); \
// } while(0)
