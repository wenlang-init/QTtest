#ifndef PRINTFUNCTION_H
#define PRINTFUNCTION_H

// ubuntu
// \033 == \e == 0x1B

// 格式：\e[显示方式;前景色;背景色m输出字符串\e[0m

// 显示方式：
// 0默认；
// 1粗体；22非粗体
// 4单条下划线；24无下划线
// 5闪烁；25无闪烁
// 7反显（翻转前景背景色）；27无反显
// 8隐藏

// 颜色：30+前景色；40+背景色
// 0黑色；1红色；2黄色；4蓝色；5洋红色；6青色；7白色

// 部分控制：
#define CLEAR "\e[2j"			// 清屏
#define CLRLINE "\r\e[K"		// or "\e[K\r" 清除从光标到行尾的内容

#define RESET       "\033[0m"	// 恢复终端默认属性
#define BLACK       "\033[30m"             /* Black 黑色 */
#define RED         "\033[31m"             /* Red 红色 */
#define GREEN       "\033[32m"             /* Green 绿色 */
#define YELLOW      "\033[33m"             /* Yellow 黄的 */
#define BLUE        "\033[34m"             /* Blue 蓝色 */
#define MAGENTA     "\033[35m"             /* Magenta 品红 */
#define CYAN        "\033[36m"             /* Cyan 青色 */
#define WHITE       "\033[37m"             /* White 白色 */
#define BOLDBLACK   "\033[1;30m"      /* Bold Black 粗黑 */
#define BOLDRED     "\033[1;31m"      /* Bold Red 粗红 */
#define BOLDGREEN   "\033[1;32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1;33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1;34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1;35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1;36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1;37m"      /* Bold White */

typedef enum LOG_TYPE_ENUM {
    LOG_TYPE_ENUM_DEBUG=0,
    LOG_TYPE_ENUM_WARRING,
    LOG_TYPE_ENUM_CRITICAL,
    LOG_TYPE_ENUM_FATAL,
    LOG_TYPE_ENUM_INFO
}LOG_TYPE_ENUM;

#ifdef __cplusplus
extern "C" {
#endif

double __get_printfTime_d();
// 要求内存不小于20byte
const char *__get_printfTime(char *cur_time,int size);

void printColour(const char *color,const char *data);
void printDebugMsg(LOG_TYPE_ENUM type,const char* function,const char *file,const int line,const char* format, ...);

/**
 @brief 打印输出
 @param type-为对应的级别
 @param more-参考printf函数
 */
#define PRINT_LOG(type,__format, ...) \
do \
    { \
        printDebugMsg(type,__FUNCTION__,__FILE__, __LINE__,__format,##__VA_ARGS__); \
    }while (0)

#define DEBUG_PRINT_LOG(__format, ...)    PRINT_LOG(LOG_TYPE_ENUM_DEBUG,__format,##__VA_ARGS__)
#define WARRING_PRINT_LOG(__format, ...)  PRINT_LOG(LOG_TYPE_ENUM_WARRING,__format,##__VA_ARGS__)
#define CRITICAL_PRINT_LOG(__format, ...) PRINT_LOG(LOG_TYPE_ENUM_CRITICAL,__format,##__VA_ARGS__)
#define FATAL_PRINT_LOG(__format, ...)    PRINT_LOG(LOG_TYPE_ENUM_FATAL,__format,##__VA_ARGS__)
#define INFO_PRINT_LOG(__format, ...)     PRINT_LOG(LOG_TYPE_ENUM_INFO,__format,##__VA_ARGS__)


#define CUSTOMMSGHANDLER \
[](QtMsgType type, const QMessageLogContext &context, const QString &msg)\
{\
    QString tempMsg;\
    if (context.file && QString::fromStdString(context.file).contains("qrc:/"))\
    {\
        tempMsg += "qmlLog ";\
    }\
\
    if (msg.contains("TypeError") && msg.contains("sRowHeight"))\
    {\
        return;\
    }\
\
    tempMsg += msg;\
\
    switch (type)\
    {\
    case QtDebugMsg:\
        printDebugMsg(LOG_TYPE_ENUM_DEBUG,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());\
        break;\
    case QtInfoMsg:\
        printDebugMsg(LOG_TYPE_ENUM_INFO,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());\
        break;\
    case QtWarningMsg:\
        printDebugMsg(LOG_TYPE_ENUM_WARRING,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());\
        break;\
    case QtCriticalMsg:\
        printDebugMsg(LOG_TYPE_ENUM_CRITICAL,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());\
        break;\
    case QtFatalMsg:\
        printDebugMsg(LOG_TYPE_ENUM_FATAL,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());\
    default:\
        break;\
    }\
}

#define REDIRECT_QTMESSAGE(handler) \
do{\
    if(handler){\
        qInstallMessageHandler(handler);\
    } else {\
        qInstallMessageHandler(CUSTOMMSGHANDLER);\
    }\
}while(0)


#ifdef __cplusplus
}
#endif

#endif // PRINTFUNCTION_H
