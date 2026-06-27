#include "mainwindow.h"
#include <QDebug>
#include <QApplication>
#include <QLocale>
#include <QString>
#include <QTranslator>
#include <QtWebView>

#include <QOpenGLWidget>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#if defined(Q_OS_WINDOWS) || defined(Q_OS_LINUX)
# include "src/fftw/widegtfft.h"
#endif // if defined(Q_OS_WINDOWS) || defined(Q_OS_LINUX)
#include "src/messageWidget/listmessageview.h"
#include "src/video/videowidget.h"
#include "src/graphics/graphicswidget.h"
#include "src/qmlList/listw.h"
#include "src/audio/widget.h"
#include "src/layout/wlayout.h"
#include "src/souyin/shouyinw.h"
#include "homewidget.h"

// #include "input_method_widget.h"
#ifdef _MSC_VER
# include "paddle_inference_api.h"
#endif // ifdef _MSC_VER

// #include "lognone.h"
#if defined(Q_OS_WINDOWS)
# include "QBreakpadHandler.h"
#endif // if defined(Q_OS_WINDOWS)
#include "cxxlog.h"

void paddletest()
{
#ifdef _MSC_VER

    // 创建默认配置对象
    paddle_infer::Config config;
    QString prog_file = "D:/software/PaddleOCR/resnet50/inference.pdmodel";
    QString params_file =
        "D:/software/PaddleOCR/resnet50/inference.pdiparams";

    // 设置推理模型路径，即为本小节第2步中下载的模型
    config.SetModel(prog_file.toStdString(), params_file.toStdString());

    if (0) {
        QByteArray prog_str, params_str;
        QFile f1(prog_file), f2(params_file);

        if (f1.open(QIODevice::ReadOnly)) {
            prog_str = f1.readAll();
            f1.close();
        }

        if (f2.open(QIODevice::ReadOnly)) {
            params_str = f2.readAll();
            f2.close();
        }

        // 从内存中加载模型
        config.SetModelBuffer(prog_str.data(), prog_str.size(),
                              params_str.data(), params_str.size());
    }

    if (config.model_from_memory()) {
        // 判断是否从内存中加载模型
        qDebug() << "Load model from is memory";
    }

    // 启用 GPU 和 MKLDNN 推理
    // config.EnableUseGpu(100, 0);
    config.EnableMKLDNN();

    // 设置 CPU 加速库线程数为 10
    config.SetCpuMathLibraryNumThreads(16);

    // 通过 API 获取 CPU 信息
    int num_thread = config.cpu_math_library_num_threads();
    qDebug() << "CPU thread number is: " << num_thread;

    // 开启 内存/显存 复用
    config.EnableMemoryOptim();

    ////////////////////////////////////////////////////////////
    // 根据 Confi 对象创建预测器对象
    auto predictor = paddle_infer::CreatePredictor(config);

    ///////////////////////////////////////////////////////////
    // 获取输入 Tensor
    auto input_names = predictor->GetInputNames();
    auto input_tensor = predictor->GetInputHandle(input_names[0]);

    // 设置输入 Tensor 的维度信息
    std::vector<int> INPUT_SHAPE = { 1, 3, 224, 224 };
    input_tensor->Reshape(INPUT_SHAPE);

    // 准备输入数据
    int input_size = 1 * 3 * 224 * 224;
    std::vector<float> input_data(input_size, 1);

    // 设置输入 Tensor 数据
    input_tensor->CopyFromCpu(input_data.data());

    ///////////////////////////////////////////////////////////
    // 执行推理
    predictor->Run();

    ///////////////////////////////////////////////////////////
    // 获取 Output Tensor
    auto output_names = predictor->GetOutputNames();
    auto output_tensor = predictor->GetOutputHandle(output_names[0]);

    // 获取 Output Tensor 的维度信息
    std::vector<int> output_shape = output_tensor->shape();
    int output_size = std::accumulate(output_shape.begin(),
                                      output_shape.end(),
                                      1,
                                      std::multiplies<int>());

    // 获取 Output Tensor 的数据
    std::vector<float> output_data;
    output_data.resize(output_size);
    output_tensor->CopyToCpu(output_data.data());
#endif // ifdef _MSC_VER
}

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
    INFO_LOG_CXX("wait log finish. systemsig=%d\n", sig);

    // destinyLog();
    // INFO_PRINT_LOG("write log finish. while exit\n");
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
#if (defined(_WIN32) || defined(_WIN64)) && defined(QT_DEBUG)
    system("color 0");
#endif // if (defined(_WIN32) || defined(_WIN64)) && defined(QT_DEBUG)

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

    // 使用 OpenGL ES 2.0 渲染（适用于移动设备和某些桌面环境）
    // QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);

    // 强制使用桌面 OpenGL（适用于大多数桌面环境，但在某些平台上可能不兼容）
    // QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    // qDebug() << QCoreApplication::testAttribute(Qt::AA_UseDesktopOpenGL);

    // 强制使用软件OpenGL（适用于没有硬件加速的环境，但性能较差）
    // QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    // QGuiApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    // Ensures that siblings of native widgets stay non-native unless
    // specifically set by the Qt::WA_NativeWindow attribute.
    QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    QApplication a(argc, argv);

    QtWebView::initialize();

#if defined(Q_OS_WINDOWS)

    // dump 路径
    QBreakpadInstance.setDumpPath("./qbreakpad_dump");
#endif // if defined(Q_OS_WINDOWS)

#if 0

    // QFile styleFile(":/QtTheme/theme/Flat/Dark/Blue/LightGreen.qss");
    QFile styleFile(":/MacOS/MacOS.qss");

    if (styleFile.open(QIODevice::ReadOnly)) {
        QString style = styleFile.readAll();
        a.setStyleSheet(style);
        INFO_LOG_CXX("%s,%d\n",
                     styleFile.fileName().toLocal8Bit().data(),
                     style.size());
        styleFile.close();
    }
#endif // if 0

    // MainWindow *aaa = nullptr; aaa->show();

    QString logdir = QCoreApplication::applicationDirPath() + "/log";
    qDebug() << logdir << logdir.toLocal8Bit().data();

    CxxLog::getInstance().initLog(logdir.toLocal8Bit().toStdString(),
                                  1000,
                                  1024 * 1024 * 10);
    CxxLog::getInstance().setLogLevel(CxxLog::LOG_TYPE_DEBUG);
    CxxLog::getInstance().setFileLogLevel(CxxLog::LOG_TYPE_DEBUG);
    CxxLog::getInstance().setPrint(true);
    CxxLog::getInstance().setColorLog(false);

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

    qInfo().noquote() << qApp->applicationFilePath()
                      << "\nPID:" << qApp->applicationPid()
                      << "\nversion:" << qApp->applicationVersion()
                      << "\narg:" << qApp->arguments()
                      << "\norganization:" << qApp->organizationDomain()
                      << ";" << qApp->organizationName();

    // SETAUTOSHOW_INPUT_METHOD_WIDGET();

    if (argc >= 2) {
        QString str = argv[1];

        if (str == "1") {
        #if defined(Q_OS_WINDOWS) || defined(Q_OS_LINUX)
            widegtFFT *w = new widegtFFT;
            w->resize(600, 600);
            w->show();
            w->setAttribute(Qt::WA_DeleteOnClose, true);
            QObject::connect(w, &widegtFFT::destroyed, [&]() {
                qDebug() << "widegtFFT -------" << w;
            });
        #endif // if defined(Q_OS_WINDOWS) || defined(Q_OS_LINUX)
        } else if (str == "2") {
            videoWidget *w = new videoWidget;
            w->resize(600, 600);
            w->show();
            w->setAttribute(Qt::WA_DeleteOnClose, true);
            QObject::connect(w, &videoWidget::destroyed, [&]() {
                qDebug() << "videoWidget -------" << w;
            });
        } else if (str == "3") {
            ListMessageView *w = new ListMessageView;
            w->resize(600, 600);
            w->show();
            w->setAttribute(Qt::WA_DeleteOnClose, true);
            QObject::connect(w, &ListMessageView::destroyed, [&]() {
                qDebug() << "ListMessageView -------" << w;
            });
        } else if (str == "4") {
            GraphicsWidget *w = new GraphicsWidget;
            w->resize(600, 600);
            w->show();
            w->setAttribute(Qt::WA_DeleteOnClose, true);
            QObject::connect(w, &GraphicsWidget::destroyed, [&]() {
                qDebug() << "GraphicsWidget -------" << w;
            });
        } else if (str == "5") {
            ListW *w = new ListW;
            w->resize(600, 600);
            w->show();
            w->setAttribute(Qt::WA_DeleteOnClose, true);
            QObject::connect(w, &ListW::destroyed, [&]() {
                qDebug() << "ListW -------" << w;
            });
        } else if (str == "6") {
            Widget *w = new Widget;
            w->resize(600, 600);
            w->show();
            w->setAttribute(Qt::WA_DeleteOnClose, true);
            QObject::connect(w, &Widget::destroyed, [&]() {
                qDebug() << "Widget -------" << w;
            });
        } else if (str == "7") {
            wLayout *w = new wLayout;
            w->resize(600, 600);
            w->show();
            w->setAttribute(Qt::WA_DeleteOnClose, true);
            QObject::connect(w, &wLayout::destroyed, [&]() {
                qDebug() << "wLayout -------" << w;
            });
        } else if (str == "8") {
            ShouYinW *w = new ShouYinW;
            w->resize(600, 600);
            w->show();
            w->setAttribute(Qt::WA_DeleteOnClose, true);
            QObject::connect(w, &ShouYinW::destroyed, [&]() {
                qDebug() << "ShouYinW -------" << w;
            });
        } else {
            MainWindow *w = new MainWindow;
            w->show();
            w->setAttribute(Qt::WA_DeleteOnClose, true);
            QObject::connect(w, &MainWindow::destroyed, [&]() {
                qDebug() << "MainWindow -------" << w;
            });
        }
    } else {
        homewidget *w = new homewidget;
        w->show();
        w->resize(600, 600);
        w->setAttribute(Qt::WA_DeleteOnClose, true);
        QObject::connect(w, &homewidget::destroyed, [&]() {
            qDebug() << "homewidget -------" << w;
        });
    }

    paddletest();

    int ret = a.exec();

    // INFO_LOG("stop status=%d\n", ret);
    // destinyLog();
    // INFO_PRINT_LOG("while exit\n");

    INFO_LOG_CXX("stop status=%d\n", ret);

    return ret; // QCoreApplication::exec();
}
