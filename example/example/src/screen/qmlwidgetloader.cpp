#include "qmlwidgetloader.h"
#include <QHBoxLayout>
#include <QQuickWidget>
#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QMetaObject>
#include <QQuickItem>
#include <QVariant>
#include "ffmpegscreen.h"
#ifdef Q_OS_WINDOWS
# include <qt_windows.h>

// #include <windows.h>
# include <opencv2/opencv.hpp>
#endif // ifdef Q_OS_WINDOWS

cv::Mat QImageToCvMat(const QImage& image) {
    switch (image.format()) {
    case QImage::Format_RGB32: // 或 Format_ARGB32, Format_ARGB32_Premultiplied
    case QImage::Format_ARGB32: {
        // QImage 是 RGBA 格式，对应 OpenCV 的 4 通道 CV_8UC4
        // 使用 constBits() 进行浅拷贝，或 bits() 后进行深拷贝
        cv::Mat mat(image.height(), image.width(), CV_8UC4,
                    const_cast<uchar *>(image.bits()), image.bytesPerLine());
        return mat.clone(); // 推荐深拷贝
    }

    case QImage::Format_RGB888: {
        // QImage 是 RGB 格式，对应 OpenCV 的 3 通道 CV_8UC3
        // 但 OpenCV 默认通道顺序是 BGR，需要转换
        cv::Mat mat(image.height(), image.width(), CV_8UC3,
                    const_cast<uchar *>(image.bits()), image.bytesPerLine());
        cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR); // RGB -> BGR
        return mat.clone();
    }

    case QImage::Format_RGB16: {
        // QImage 是 RGB 格式，对应 OpenCV 的 2 通道 CV_8UC2
        // 但 OpenCV 默认通道顺序是 BGR，需要转换
        cv::Mat mat(image.height(), image.width(), CV_8UC2,
                    const_cast<uchar *>(image.bits()), image.bytesPerLine());
        cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR); // RGB -> BGR
        return mat.clone();
    }

    case QImage::Format_Indexed8: // 或 Format_Grayscale8
    case QImage::Format_Grayscale8: {
        // 8位灰度图，对应 OpenCV 的单通道 CV_8UC1
        cv::Mat mat(image.height(), image.width(), CV_8UC1,
                    const_cast<uchar *>(image.bits()), image.bytesPerLine());
        return mat.clone();
    }

    default: {
        // 对于不支持的格式，可以尝试转换
        QImage converted = image.convertToFormat(QImage::Format_RGB888);
        return QImageToCvMat(converted);
    }
    }
}

QImage CvMatToQImage(const cv::Mat& mat) {
    switch (mat.type()) {
    case CV_8UC1: {
        // OpenCV 单通道灰度图 -> QImage 灰度图
        QImage image(mat.data,
                     mat.cols,
                     mat.rows,
                     mat.step,
                     QImage::Format_Grayscale8);
        return image.copy(); // 推荐深拷贝
    }

    case CV_8UC2: {
        // OpenCV 2通道 BGR 图 -> QImage RGB 图
        cv::Mat rgbMat;
        cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB); // BGR -> RGB
        QImage image(rgbMat.data,
                     rgbMat.cols,
                     rgbMat.rows,
                     rgbMat.step,
                     QImage::Format_RGB16);
        return image.copy(); // 推荐深拷贝
    }

    case CV_8UC3: {
        // OpenCV 3通道 BGR 图 -> QImage RGB 图
        cv::Mat rgbMat;
        cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB); // BGR -> RGB
        QImage image(rgbMat.data,
                     rgbMat.cols,
                     rgbMat.rows,
                     rgbMat.step,
                     QImage::Format_RGB888);
        return image.copy();
    }

    case CV_8UC4: {
        // OpenCV 4通道 BGRA 图 -> QImage ARGB 图
        QImage image(mat.data, mat.cols, mat.rows, mat.step,
                     QImage::Format_ARGB32);
        return image.copy();
    }

    default: {
        qWarning() << "CvMatToQImage() - Unsupported cv::Mat type:" << mat.type();
        return QImage();
    }
    }
}

qmlWidgetLoader::qmlWidgetLoader(QWidget *parent)
    : QWidget{parent}
{
    qw = new QQuickWidget(this);

    imageP = new imageProvider();

    qw->setResizeMode(QQuickWidget::SizeRootObjectToView);

    // qw->engine()->addImportPath("qrc:/");
    qw->engine()->addImageProvider(QStringLiteral("test"), imageP);
    qw->rootContext()->setContextProperty("imageProvider", imageP);
    qw->setSource(QUrl("qrc:/src/screen/imageFetcher.qml"));
    qw->showFullScreen();
    connect(qw, &QQuickWidget::sceneGraphError, this,
            [ = ](QQuickWindow::SceneGraphError error, const QString& message) {
        qDebug() << error << message;
    });
    connect(qw, &QQuickWidget::statusChanged, this,
            [ = ](QQuickWidget::Status status) {
        qDebug() << status;
    });

    QHBoxLayout *hboxlayout = new QHBoxLayout(this);
    hboxlayout->addWidget(qw);
#if 0
    QTimer *timer = new QTimer(this);
    timer->start(1000.0 / 25);
    connect(timer, &QTimer::timeout, this, [ = ]() {
        QScreen *screen = QGuiApplication::primaryScreen();

        if (screen) {
            imageP->updateImage(screen->grabWindow(0, 0, 0, -1, -1).toImage());

            // 重新加载
            QMetaObject::invokeMethod(qw->rootObject(),
                                      "reLoader");
        }
    });
#else // if 0

    // ffmpegScreen::instance().initparam("desktop", "25", "0", "0",
    // "3200x2000");

    int width = 1920, height = 1200;
    width = QGuiApplication::primaryScreen()->geometry().width()
            * QGuiApplication::primaryScreen()->devicePixelRatio();
    height = QGuiApplication::primaryScreen()->geometry().height()
             * QGuiApplication::primaryScreen()->devicePixelRatio();

    // QList<QScreen *> screens = QGuiApplication::screens();
    //     foreach(QScreen * screen, screens) {
    //         QRect screenGeometry = screen->geometry();

    //         qDebug() << screenGeometry;
    //         qDebug() << "屏幕分辨率：" << screenGeometry.width() << "x" <<
    //             screenGeometry.height();
    //     }
    // QScreen *sc = QGuiApplication::primaryScreen();
    // qDebug() << width << height
    //        << sc->geometry()
    //        << sc->logicalDotsPerInch()
    //        << sc->logicalDotsPerInchX()
    //        << sc->logicalDotsPerInchY()
    //        << sc->physicalDotsPerInch()
    //        << sc->physicalDotsPerInchX()
    //        << sc->physicalDotsPerInchY()
    //        << sc->physicalSize()
    //        << sc->size()
    //        << sc->virtualSize()
    //        << sc->depth()
    //        << sc->refreshRate()
    //        << sc->virtualGeometry()
    //        << sc->devicePixelRatio();
# if 0

    // ffmpegScreen::instance().info();
    ffmpegScreen::instance().initparam("desktop", "25", "0", "0",
                                       QString("%1x%2").arg(width).arg(height));
    ffmpegScreen::instance().startwork();
    connect(&ffmpegScreen::instance(), &ffmpegScreen::gotFrame, this,
            [ = ](QImage image) {
        showImage(image);
    });
# else // if 0
#  if defined(Q_OS_WINDOWS)
    using cv::Mat;
    double zoom = getZoom();
    int    m_width = GetSystemMetrics(SM_CXSCREEN) * zoom;
    int    m_height = GetSystemMetrics(SM_CYSCREEN) * zoom;
    m_screenshotData.resize(m_width * m_height * 4, 0);
    HDC m_screenDC = GetDC(NULL);
    HDC m_compatibleDC = CreateCompatibleDC(m_screenDC);
    HBITMAP m_hBitmap = CreateCompatibleBitmap(m_screenDC, m_width, m_height);
    SelectObject(m_compatibleDC, m_hBitmap);

    // DeleteObject(m_hBitmap);
    // DeleteDC(m_screenDC);
    // ReleaseDC(NULL,m_screenDC);

    qDebug() << zoom << m_width << m_height;

    QTimer *timer = new QTimer(this);
    timer->start(40);
    connect(timer, &QTimer::timeout, this, [ = ]() {
        // 得到位图的数据
        BitBlt(m_compatibleDC, 0, 0, m_width, m_height, m_screenDC, 0, 0,
               SRCCOPY);
        GetBitmapBits(m_hBitmap, m_screenshotData.size(),
                      m_screenshotData.data());

        // 创建图像
        Mat screenshot(m_height, m_width, CV_8UC4, m_screenshotData.data());

        // Mat screenshotRect = screenshot(cv::Rect(10, 10, 1920, 1080));

        // Mat grayImage;
        // cv::cvtColor(screenshot, grayImage, cv::COLOR_BGR2GRAY);
        // cv::Canny(grayImage, screenshot, 100, 100);

        QImage image = CvMatToQImage(screenshot);

        imageP->updateImage(image);

        qint64 nowid = QDateTime::currentMSecsSinceEpoch();

        // 重新加载
        QMetaObject::invokeMethod(qw->rootObject(),
                                  "reLoader",
                                  Q_ARG(QVariant, nowid));
    });


#  endif // if defined(Q_OS_WINDOWS)
# endif  // if 1
#endif   // if 0
}

qmlWidgetLoader::~qmlWidgetLoader()
{
    imageP->deleteLater();
}

double qmlWidgetLoader::getZoom()
{
    #if defined(Q_OS_WINDOWS)

    // 获取窗口当前显示的监视器
    HWND hWnd = GetDesktopWindow();
    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    // 获取监视器逻辑宽度
    MONITORINFOEX monitorInfo;

    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfo(hMonitor, &monitorInfo);
    int cxLogical = (monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left);

    // 获取监视器物理宽度
    DEVMODE dm;
    dm.dmSize = sizeof(dm);
    dm.dmDriverExtra = 0;
    EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &dm);
    int cxPhysical = dm.dmPelsWidth;

    return cxPhysical * 1.0 / cxLogical;

#endif // if defined(Q_OS_WINDOWS)
    return 1;
}

void qmlWidgetLoader::showImage(QImage& image)
{
    // qdebug << image.size();
    cv::Mat img = QImageToCvMat(image);
    cv::Mat grayImage, dimage;

    // BGR -> 灰度图
    cv::cvtColor(img, grayImage, cv::COLOR_BGR2GRAY);

    // BGR -> HSV
    // cv::cvtColor(img, grayImage, cv::COLOR_BGR2HSV);
    // BGR -> RGB
    // cv::cvtColor(img, grayImage, cv::COLOR_BGR2RGB);
    // 调整大小到 640x480
    // cv::resize(img, grayImage, cv::Size(640, 480));
    // 翻转
    // cv::rotate(img, grayImage, 1);
    // // 1. 获取旋转矩阵：绕图像中心旋转45度，不缩放
    // cv::Point2f center(img.cols / 2.0f, img.rows / 2.0f);
    // cv::Mat rot_mat = cv::getRotationMatrix2D(center, 45, 1.0);
    // // 2. 应用仿射变换
    // cv::warpAffine(img, grayImage, rot_mat, img.size());
    // 水平翻转
    // cv::flip(img, dimage, 1);
    // 边缘检测（Canny算子）
    cv::Canny(grayImage, dimage, 100, 100);

    // cv::cvtColor(dimage, cimage, cv::COLOR_GRAY2BGR);
    // cv::applyColorMap(dimage, dimage, cv::COLORMAP_JET);

    // 拼接原始图像和翻转图像，制作对称全景图
    // cv::hconcat(img, grayImage, dimage);
    // 创建一个黑色图像(全0)
    // cv::Mat mask = cv::Mat::zeros(img.size, CV_8UC1);
    // 示例1: 绘制一个圆形区域
    // cv::circle(mask, cv::Point(img.cols/2, img.rows/2), 150,
    // cv::Scalar(255), -1);
    // cv::rectangle(mask, cv::Rect(0, 0, img.cols, img.rows),
    //               cv::Scalar(255),-1);
    // 使用掩码复制图像区域
    // img.copyTo(dimage, mask);
    // 应用掩膜
    // cv::bitwise_and(img, grayImage, dimage, mask);

    image = CvMatToQImage(dimage);

    imageP->updateImage(image);

    qint64 nowid = QDateTime::currentMSecsSinceEpoch();

    // 重新加载
    QMetaObject::invokeMethod(qw->rootObject(),
                              "reLoader",
                              Q_ARG(QVariant, nowid));
}
