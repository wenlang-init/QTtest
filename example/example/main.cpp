#include "mainwindow.h"
#include <QDebug>
#include <QApplication>
#include <QLocale>
#include <QString>
#include <QTranslator>

// #include "lognone.h"
#if !defined(Q_OS_ANDROID)
# include "QBreakpadHandler.h"
#endif // if !defined(Q_OS_ANDROID)
#include "cxxlog.h"

#if defined(_WIN32) || defined(_WIN64)
# include <windows.h>

BOOL WINAPI HandlerRoutine(DWORD dwCtrlType) {
    DEBUG_LOG_CXX("vvvvvvvvvvvxxxxxxxxxxxxxx %d\n", dwCtrlType);

    switch (dwCtrlType)
    {
    case CTRL_C_EVENT: // CTRL + C
        MessageBox(NULL, L"CTRL + C", L"提示", MB_OK);
        break;

    case CTRL_BREAK_EVENT: // CTRL + BREAK
        break;

    case CTRL_CLOSE_EVENT: // 关闭
        MessageBox(NULL, L"关闭事件", L"提示", MB_OK);
        break;

    case CTRL_LOGOFF_EVENT: // 用户退出
        MessageBox(NULL, L"用户退出，系统注销", L"提示", MB_OK);
        break;

    case CTRL_SHUTDOWN_EVENT: // 系统被关闭时.
        MessageBox(NULL, L"系统关闭", L"提示", MB_OK);
        break;
    }

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
    // signal(SIGKILL, handle_signal); // 9
    signal(SIGTERM, handle_signal); // 15
}

#endif // if defined(_WIN32) || defined(_WIN64)

void ExitRoutine1(void) {
    INFO_LOG_CXX("while exit\n");
}

void ExitRoutine2(void) {
    INFO_LOG_CXX("exit\n");
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
    // REDIRECT_QTMESSAGE_LOG(nullptr);
    REDIRECT_QTMESSAGE_LOG_CXX(nullptr);
#if defined(_WIN32) || defined(_WIN64)

    // 第二个参数FALSE为卸载钩子
    if (!SetConsoleCtrlHandler(HandlerRoutine, TRUE)) {
        FATAL_LOG_CXX("Error: Could not set control handler.\n");

        DWORD  errorCode = GetLastError();
        LPWSTR messageBuffer = nullptr;
        DWORD  size = FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&messageBuffer,
            0,
            NULL
            );

        if (size) {
            FATAL_LOG_CXX("22222222222222222 %d:%s\n",
                          errorCode,
                          QString::fromUtf16(
                              (const char16_t *)messageBuffer).toLocal8Bit().constData());
            LocalFree(messageBuffer);
        } else {
            FATAL_LOG_CXX("33333333333333333 %d\n", errorCode);
        }
    }

    exitAT();
#else // if defined(_WIN32) || defined(_WIN64)
    initexitDetection();
#endif // if defined(_WIN32) || defined(_WIN64)

    QApplication a(argc, argv);
#if !defined(Q_OS_ANDROID)

    // dump 路径
    QBreakpadInstance.setDumpPath("./qbreakpad_dump");
#endif // if !defined(Q_OS_ANDROID)

    // MainWindow *aaa = nullptr; aaa->show();

    QString logdir = QCoreApplication::applicationDirPath() + "/log";
    qDebug() << logdir << logdir.toLocal8Bit().data();

    CxxLog::getInstance().initLog(logdir.toLocal8Bit().toStdString(),
                                  1000,
                                  1024 * 1024 * 10);
    CxxLog::getInstance().setFileLogLevel(CxxLog::LOG_TYPE_DEBUG);
    CxxLog::getInstance().setFileLogLevel(CxxLog::LOG_TYPE_DEBUG);
    CxxLog::getInstance().setPrint(true);
    CxxLog::getInstance().setColorLog(true);

    // DEBUG_LOG_CXX_STRING(std::string("xxxxxxxxxxxx1xxxxxxxxxxx\n"));
    // WARRING_LOG_CXX_STRING(std::string("xxxxxxxxxx2xxxxxxxxxxxxx\n"));
    // CRITICAL_LOG_CXX_STRING(std::string("xxxxxxxxx3xxxxxxxxxxxxxx\n"));
    // FATAL_LOG_CXX_STRING(std::string("xxxxxxxxxxxx4xxxxxxxxxxx\n"));
    // INFO_LOG_CXX_STRING(std::string("xxxxxxxxxxxxx5xxxxxxxxxx\n"));
    // DEBUG_LOG_CXX("12345;%s\n", "sssssssss1");
    // WARRING_LOG_CXX("12345;%d\n", 1);
    // CRITICAL_LOG_CXX("222222222222d\n");
    // FATAL_LOG_CXX("12345mmmmmmmmmmmm\n");
    // INFO_LOG_CXX("12345yyyyyyyyyyyyyyyyyyyyyyyy\n");

    // char buf[1024];
    // if (GET_CURRENTPATH(buf, sizeof(buf))) {
    //     INFO_PRINT_LOG("11111111111111111111 %s\n", buf);
    // }


    // initLog(logdir.toLocal8Bit().data(), 1000, 1024 * 1024 * 10);
    // setLogLevel(LOG_TYPE_ENUM_DEBUG);

    // // setLogPrint(0);
    // PRINT_LOG(LOG_TYPE_ENUM_DEBUG, "ccccccccccccccccccccxxxxx\n");
    // INFO_PRINT_LOG("bbbbbbbbbbbbbbbbbbbb\n");
    // INFO_LOG("ttttttttttttttttttttttttt\n");

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();

    for (const QString& locale : uiLanguages) {
        const QString baseName = "example_" + QLocale(locale).name();

        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();

    int ret = a.exec();

    // INFO_LOG("stop status=%d\n", ret);
    // destinyLog();
    // INFO_PRINT_LOG("while exit\n");

    INFO_LOG_CXX("stop status=%d\n", ret);

    return ret; // QCoreApplication::exec();
}
