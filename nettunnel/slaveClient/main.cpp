#include "widget.h"
#include <QApplication>
#include "baseMacroDefine.h"
#include "printFunction.h"
#include "lognone.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

BOOL WINAPI HandlerRoutine(DWORD dwCtrlType) {
    DEBUG_PRINT_LOG("vvvvvvvvvvvxxxxxxxxxxxxxx %d\n",dwCtrlType);
    // switch (dwCtrlType)
    // {
    // case CTRL_C_EVENT: //CTRL + C
    //     MessageBox(NULL, L"CTRL + C", L"提示", MB_OK);
    //     break;
    // case CTRL_BREAK_EVENT: //CTRL + BREAK
    //     break;
    // case CTRL_CLOSE_EVENT: //关闭
    //     MessageBox(NULL, L"关闭事件", L"提示", MB_OK);
    //     break;
    // case CTRL_LOGOFF_EVENT: //用户退出
    //     MessageBox(NULL, L"用户退出，系统注销", L"提示", MB_OK);
    //     break;
    // case CTRL_SHUTDOWN_EVENT: //系统被关闭时.
    //     MessageBox(NULL, L"系统关闭", L"提示", MB_OK);
    //     break;
    // }

    return 0;
}

#else

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
//#include <unistd.h>
// _exit(0)

void handle_signal(int sig)
{
    DEBUG_PRINT_LOG("vvvvvvvvvvvxxxxxxxxxxxxxx %d\n",sig);
    fflush(stdout);
    exit(0);
}

void initexitDetection() {
    signal(SIGHUP,handle_signal); // 1
    signal(SIGINT,handle_signal); // 2 interrupt,在Linux中体现为CTRL+C
    signal(SIGQUIT,handle_signal); // 3
    //signal(SIGKILL,handle_signal); // 9
    signal(SIGTERM,handle_signal); // 15
}
#endif

void ExitRoutine1(void) {
    DEBUG_PRINT_LOG("while exit\n");
}
void ExitRoutine2(void) {
    DEBUG_PRINT_LOG("exit\n");
}

void exitAT(){
    //注册顺序和执行顺序相反
    // 对于ctrl+c之类的不起作用，只有exit()或关闭窗口时才会起作用
    atexit(ExitRoutine2);
    atexit(ExitRoutine1);
    //exit(0);
}

void customMsgHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString tempMsg;
    if (context.file && QString::fromStdString(context.file).contains("qrc:/"))
    {
        tempMsg += "qmlLog ";
    }

    if (msg.contains("TypeError") && msg.contains("sRowHeight"))
    {
        return;
    }

    //tempMsg += QString("%1:%2:%3 %4").arg(context.file, context.function, QString::number(context.line), msg.toLocal8Bit().constData());
    tempMsg += msg;

    switch (type)
    {
    case QtDebugMsg:
        printDebugMsg(LOG_TYPE_ENUM_DEBUG,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());
        break;
    case QtInfoMsg:
        printDebugMsg(LOG_TYPE_ENUM_INFO,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());
        break;
    case QtWarningMsg:
        printDebugMsg(LOG_TYPE_ENUM_WARRING,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());
        break;
    case QtCriticalMsg:
        printDebugMsg(LOG_TYPE_ENUM_CRITICAL,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());
        break;
    case QtFatalMsg:
        printDebugMsg(LOG_TYPE_ENUM_FATAL,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());
    default:// printDebugMsg(LOG_TYPE_ENUM_DEBUG,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());
        break;
    }
}

int main(int argc, char *argv[])
{
#if defined(_WIN32) || defined(_WIN64)
    //第二个参数FALSE为卸载钩子
    SetConsoleCtrlHandler(HandlerRoutine, TRUE);
#else
    initexitDetection();
#endif
    exitAT();

    char aa[100];
    DEBUG_PRINT_LOG("%d\n",sprintf(aa,"1"));
    // 重定向 qDebug() 等的log输出, 包括qml的log输出一起重定向.
    //qInstallMessageHandler(customMsgHandler);
    //REDIRECT_QTMESSAGE(customMsgHandler);
    REDIRECT_QTMESSAGE(nullptr);
    qDebug() << "\e[36m" TESTSTRING "\e[0m";
    //printColour(BOLDRED,"ffffffffffff");
    //WRITE_LOG(LOG_TYPE_ENUM_WARRING,"%s","cccccccccccccccccccccccc");
    QApplication a(argc, argv);

    QString logdir = QCoreApplication::applicationDirPath() + "/log";
    qDebug()<<logdir<<logdir.toLocal8Bit().data();
    initLog(logdir.toLocal8Bit().data(),600,1024*1024);
    setLogLevel(LOG_TYPE_ENUM_DEBUG);
    setLogPrint(0);

    // for(int i=0;i<10000;i++){
    //     DEBUG_LOG("a %d\n",i);
    //     WARRING_LOG("b %d\n",i);
    //     CRITICAL_LOG("c %d\n",i);
    //     FATAL_LOG("d %d\n",i);
    //     INFO_LOG("e %d\n",i);
    // }

    Widget w;
    w.show();
    int ret = a.exec();
    DEBUG_PRINT_LOG("faaaaa %d\n",ret);
    destinyLog();
    //DEBUG_PRINT_LOG("%s\n",_pgmptr);
    return ret;
}
