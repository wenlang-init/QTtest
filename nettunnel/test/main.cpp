#include "widget.h"

// #include "printFunction.h"
#include "lognone.h"
#include <QApplication>

#if defined(_WIN32) || defined(_WIN64)
# include <windows.h>

BOOL WINAPI HandlerRoutine(DWORD dwCtrlType) {
    DEBUG_PRINT_LOG("vvvvvvvvvvvxxxxxxxxxxxxxx %d\n", dwCtrlType);

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

#else // if defined(_WIN32) || defined(_WIN64)

# include <stdio.h>
# include <signal.h>
# include <stdlib.h>

// #include <unistd.h>
// _exit(0)

void handle_signal(int sig)
{
    INFO_LOG("wait log finish. systemsig=%d\n", sig);
    destinyLog();
    INFO_PRINT_LOG("write log finish. while exit\n");
    exit(0);
}

void initexitDetection() {
    signal(SIGHUP,  handle_signal); // 1
    signal(SIGINT,  handle_signal); // 2 interrupt,在Linux中体现为CTRL+C
    signal(SIGQUIT, handle_signal); // 3
    // signal(SIGKILL,handle_signal); // 9
    signal(SIGTERM, handle_signal); // 15
}

#endif // if defined(_WIN32) || defined(_WIN64)

void ExitRoutine1(void) {
    DEBUG_PRINT_LOG("while exit\n");
}

void ExitRoutine2(void) {
    DEBUG_PRINT_LOG("exit\n");
}

void exitAT() {
    // 注册顺序和执行顺序相反
    // 对于ctrl+c之类的不起作用，只有exit()或关闭窗口时才会起作用
    atexit(ExitRoutine2);
    atexit(ExitRoutine1);

    // exit(0);
}

int main(int argc, char *argv[])
{
    // REDIRECT_QTMESSAGE(nullptr);
    REDIRECT_QTMESSAGE_LOG(nullptr);

#if defined(_WIN32) || defined(_WIN64)

    // 第二个参数FALSE为卸载钩子
    SetConsoleCtrlHandler(HandlerRoutine, TRUE);
#else // if defined(_WIN32) || defined(_WIN64)
    initexitDetection();
#endif // if defined(_WIN32) || defined(_WIN64)

    // exitAT();
    QApplication a(argc, argv);

    QString logdir = QCoreApplication::applicationDirPath() + "/log";
    initLog(logdir.toLocal8Bit().data(), 600, 1024 * 1024 * 10);
    setLogLevel(LOG_TYPE_ENUM_DEBUG);

    Widget w;
    w.show();
    int ret = a.exec();

    INFO_LOG("stop status=%d\n", ret);
    destinyLog();

    return ret;
}
