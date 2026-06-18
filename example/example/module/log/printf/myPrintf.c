#include "myPrintf.h"

#include <stdarg.h>
// int __vsnprintf(out_fct_type out, char* buffer, const unsigned int maxlen, const char* format, va_list va);
signed int rt_vsnprintf(char *buf, unsigned int size, const char *fmt, va_list args);

#if defined(WIN32) || defined(WIN64)
static int isFirst = 1;
#include <windows.h>
#endif

int printFormat(const char *format,...)
{
#if defined(WIN32) || defined(WIN64)
    if (isFirst) {
        isFirst = 0;
        system("color 0");
    }
#endif // if defined(WIN32) || defined(WIN64)
    va_list arg;
    va_start(arg, format);
    va_start (arg, format);
    int length = rt_vsnprintf(printf_info,PRINT_BUFFER_SIZE,format,arg);
    va_end (arg);
    if(length > 0){
        //printf("%s",printf_info);
        //HAL_UART_Transmit(&huart1, (uint8_t *) printf_string, len, HAL_MAX_DELAY);
    }
    if(length > PRINT_BUFFER_SIZE){
        length = PRINT_BUFFER_SIZE-1;
    }
    return length;
}

int printFormatBuffer(char *buffer, unsigned int bufferSize, const char *format,...)
{
#if defined(WIN32) || defined(WIN64)
    if (isFirst) {
        isFirst = 0;
        system("color 0");
    }
#endif // if defined(WIN32) || defined(WIN64)
    if(bufferSize < 1)return 0;
    va_list arg;
    va_start(arg, format);
    va_start (arg, format);
    int length = rt_vsnprintf(buffer, bufferSize, format, arg);
    va_end (arg);
    if(length > 0){
        //printf("%s",printf_info);
        //HAL_UART_Transmit(&huart1, (uint8_t *) printf_string, len, HAL_MAX_DELAY);
    }
    if(length > bufferSize){
        length = bufferSize-1;
    }
    return length;
}

#include <stdio.h>
void test()
{
    char buffer[4096];
    printFormatBuffer(buffer, sizeof(buffer), GREEN "%s:%d %s" RED "---" RESET,__FUNCTION__,__LINE__,__FILE__);
    printf("%s",buffer);
    printFormatBuffer(buffer, sizeof(buffer), "- hello world %d %lf\n", 123, 3.1415926);
    printf("%s",buffer);

    MY_PRINTF_BUFFER_INFO(buffer, sizeof(buffer));
    printf("%s",buffer);
    MY_PRINTF_BUFFER(buffer, sizeof(buffer),"0 hello world %d %lf\n", 123, 3.1415926);
    printf("%s",buffer);

    MY_PRINTF_BUFFER_DEBUG(buffer, sizeof(buffer), "1 hello world %d %lf\n", 123, 3.1415926);
    printf("%s",buffer);


    printFormat(GREEN "%s:%d %s" RED "---" RESET,__FUNCTION__,__LINE__,__FILE__);
    printf("%s",printf_info);
    printFormat("- hello world %d %lf\n", 123, 3.1415926);
    printf("%s",printf_info);

    MY_PRINTF_INFO();
    printf("%s",printf_info);
    MY_PRINTF("20 hello world %d\n", 123);
    printf("%s",printf_info);

    MY_PRINTF_DEBUG("21 hello world %d %lf\n", 123, 3.1415926);
    printf("%s",printf_info);

    fflush(stdout);

    return;
}