#include "widget.h"

#include <QApplication>
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
    INFO_LOG("wait log finish. systemsig=%d\n",sig);
    destinyLog();
    INFO_PRINT_LOG("write log finish. while exit\n");
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
        //writeLognone(LOG_TYPE_ENUM_DEBUG,tempMsg.toLocal8Bit().constData());
        //DEBUG_LOG_FFL(context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());
        writeLogformat(LOG_TYPE_ENUM_DEBUG,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());
        break;
    case QtInfoMsg:
        writeLogformat(LOG_TYPE_ENUM_INFO,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());
        break;
    case QtWarningMsg:
        writeLogformat(LOG_TYPE_ENUM_WARRING,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());
        break;
    case QtCriticalMsg:
        writeLogformat(LOG_TYPE_ENUM_CRITICAL,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());
        break;
    case QtFatalMsg:
        writeLogformat(LOG_TYPE_ENUM_FATAL,context.function,context.file, context.line,"%s\n", tempMsg.toLocal8Bit().constData());
    default:
        break;
    }
}

int main(int argc, char *argv[])
{
    //REDIRECT_QTMESSAGE(nullptr);
    REDIRECT_QTMESSAGE(customMsgHandler);

#if defined(_WIN32) || defined(_WIN64)
    //第二个参数FALSE为卸载钩子
    SetConsoleCtrlHandler(HandlerRoutine, TRUE);
#else
    initexitDetection();
#endif
    //exitAT();

    QApplication a(argc, argv);
    printColour(YELLOW,"hahahahahahaha\n");

    QString logdir = QCoreApplication::applicationDirPath() + "/log";
    qDebug()<<logdir<<logdir.toLocal8Bit().data();
    initLog(logdir.toLocal8Bit().data(),600,1024*1024*10);
    setLogLevel(LOG_TYPE_ENUM_DEBUG);
    //setLogPrint(0);

    Widget w;
    w.show();
    int ret = a.exec();

    INFO_LOG("stop status=%d\n",ret);
    destinyLog();
    INFO_PRINT_LOG("while exit\n");

    return ret;
}
