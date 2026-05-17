#ifndef LOGNONE_H
#define LOGNONE_H
#include "printFunction.h"

#ifdef __cplusplus
extern "C" {
#endif

// 初始化日志路径
// logDir:路径
// flushMs:日志保存刷新周期
// fileLogMaxSize:日志文件大小
int initLog(const char* logDir,unsigned int flushMs,unsigned int fileLogMaxSize);
void destinyLog();
// 设置日志保存级别
int setLogLevel(LOG_TYPE_ENUM level);
// 设置是否在标准输出(stdout)输出
void setLogPrint(int isPrint);
// 获取当前缓存区日志数量
int getcurrentLogCount();

void writeLogformat(LOG_TYPE_ENUM level,const char* function,const char *file,const int line,const char* format, ...);
void writeLogdata(LOG_TYPE_ENUM level,const char* function,const char *file,const int line,const char* data);
void writeLog(LOG_TYPE_ENUM level,const char* function,const char *file,const int line,const char* data);

#define WRITE_LOG(level,__format, ...) \
do{ \
    writeLogformat(level,__FUNCTION__,__FILE__,__LINE__,__format,##__VA_ARGS__); \
}while(0)

#define DEBUG_LOG(__format, ...)    WRITE_LOG(LOG_TYPE_ENUM_DEBUG,__format,##__VA_ARGS__)
#define WARRING_LOG(__format, ...)  WRITE_LOG(LOG_TYPE_ENUM_WARRING,__format,##__VA_ARGS__)
#define CRITICAL_LOG(__format, ...) WRITE_LOG(LOG_TYPE_ENUM_CRITICAL,__format,##__VA_ARGS__)
#define FATAL_LOG(__format, ...)    WRITE_LOG(LOG_TYPE_ENUM_FATAL,__format,##__VA_ARGS__)
#define INFO_LOG(__format, ...)     WRITE_LOG(LOG_TYPE_ENUM_INFO,__format,##__VA_ARGS__)

#define WRITE_LOG_1(level,__format, ...) \
do{ \
        char *data = NULL; \
        int len = getFormatString(&data,__format,##__VA_ARGS__);\
        if(len < 0){\
            FATAL_PRINT_LOG("getFormatString failed\n");\
    } else {\
            writeLog(level,__FUNCTION__,__FILE__,__LINE__,data); \
            free(data);\
    }\
}while(0)

#define DEBUG_LOG_1(__format, ...)    WRITE_LOG_1(LOG_TYPE_ENUM_DEBUG,__format,##__VA_ARGS__)
#define WARRING_LOG_1(__format, ...)  WRITE_LOG_1(LOG_TYPE_ENUM_WARRING,__format,##__VA_ARGS__)
#define CRITICAL_LOG_1(__format, ...) WRITE_LOG_1(LOG_TYPE_ENUM_CRITICAL,__format,##__VA_ARGS__)
#define FATAL_LOG_1(__format, ...)    WRITE_LOG_1(LOG_TYPE_ENUM_FATAL,__format,##__VA_ARGS__)
#define INFO_LOG_1(__format, ...)     WRITE_LOG_1(LOG_TYPE_ENUM_INFO,__format,##__VA_ARGS__)

#define ADTA_MAXSIZE 4095
#define WRITE_LOG_FFL(level,func,file,line,__format, ...) \
do{ \
    char buffer[ADTA_MAXSIZE + 1]; \
    snprintf(buffer,ADTA_MAXSIZE,__format,##__VA_ARGS__); \
    writeLog(level,func,file,line,buffer); \
}while(0)

#define DEBUG_LOG_FFL(func,file,line,__format, ...)    WRITE_LOG_FFL(LOG_TYPE_ENUM_DEBUG,func,file,line,__format,##__VA_ARGS__)
#define WARRING_LOG_FFL(func,file,line,__format, ...)  WRITE_LOG_FFL(LOG_TYPE_ENUM_WARRING,func,file,line,__format,##__VA_ARGS__)
#define CRITICAL_LOG_FFL(func,file,line,__format, ...) WRITE_LOG_FFL(LOG_TYPE_ENUM_CRITICAL,func,file,line,__format,##__VA_ARGS__)
#define FATAL_LOG_FFL(func,file,line,__format, ...)    WRITE_LOG_FFL(LOG_TYPE_ENUM_FATAL,func,file,line,__format,##__VA_ARGS__)
#define INFO_LOG_FFL(func,file,line,__format, ...)     WRITE_LOG_FFL(LOG_TYPE_ENUM_INFO,func,file,line,__format,##__VA_ARGS__)

#define WRITE_LOG_RAW(level,__format, ...) \
do{ \
        char *data = NULL; \
        int len = getFormatString(&data,__format,##__VA_ARGS__);\
        if(len < 0){\
            FATAL_PRINT_LOG("getFormatString failed\n");\
    } else {\
            writeLogdata(level,__FUNCTION__,__FILE__,__LINE__,data); \
            free(data);\
    }\
}while(0)

#define DEBUG_LOG_RAW(__format, ...)    WRITE_LOG_RAW(LOG_TYPE_ENUM_DEBUG,__format,##__VA_ARGS__)
#define WARRING_LOG_RAW(__format, ...)  WRITE_LOG_RAW(LOG_TYPE_ENUM_WARRING,__format,##__VA_ARGS__)
#define CRITICAL_LOG_RAW(__format, ...) WRITE_LOG_RAW(LOG_TYPE_ENUM_CRITICAL,__format,##__VA_ARGS__)
#define FATAL_LOG_RAW(__format, ...)    WRITE_LOG_RAW(LOG_TYPE_ENUM_FATAL,__format,##__VA_ARGS__)
#define INFO_LOG_RAW(__format, ...)     WRITE_LOG_RAW(LOG_TYPE_ENUM_INFO,__format,##__VA_ARGS__)

#define WRITE_LOG_DATA(level,data) \
do{ \
        writeLogdata(level,__FUNCTION__,__FILE__, __LINE__,data); \
}while(0)

#define DEBUG_LOG_DATA(data)    WRITE_LOG_DATA(LOG_TYPE_ENUM_DEBUG,data)
#define WARRING_LOG_DATA(data)  WRITE_LOG_DATA(LOG_TYPE_ENUM_WARRING,data)
#define CRITICAL_LOG_DATA(data) WRITE_LOG_DATA(LOG_TYPE_ENUM_CRITICAL,data)
#define FATAL_LOG_DATA(data)    WRITE_LOG_DATA(LOG_TYPE_ENUM_FATAL,data)
#define INFO_LOG_DATA(data)     WRITE_LOG_DATA(LOG_TYPE_ENUM_INFO,data)


#define CUSTOMMSGHANDLER_LOG \
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
            writeLogformat(LOG_TYPE_ENUM_DEBUG,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());\
            break;\
        case QtInfoMsg:\
            writeLogformat(LOG_TYPE_ENUM_INFO,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());\
            break;\
        case QtWarningMsg:\
            writeLogformat(LOG_TYPE_ENUM_WARRING,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());\
            break;\
        case QtCriticalMsg:\
            writeLogformat(LOG_TYPE_ENUM_CRITICAL,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());\
            break;\
        case QtFatalMsg:\
            writeLogformat(LOG_TYPE_ENUM_FATAL,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());\
            break;\
            default:\
            break;\
    }\
}

#define REDIRECT_QTMESSAGE_LOG(handler) \
do{\
    if(handler){\
        qInstallMessageHandler(handler);\
    } else {\
        qInstallMessageHandler(CUSTOMMSGHANDLER_LOG);\
    }\
}while(0)

#ifdef __cplusplus
}
#endif

#endif // LOGNONE_H
