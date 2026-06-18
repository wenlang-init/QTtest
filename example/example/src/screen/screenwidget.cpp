#include "screenwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QScreen>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

#include "ffmpegscreen.h"
#define USE_FFMPEG

#ifndef USE_FFMPEG

// # define USE_QTSCREEN
#endif // ifndef USE_FFMPEG


#if defined(Q_OS_WINDOWS)
# include <windows.h>
# if !defined(USE_FFMPEG) && !defined(USE_QTSCREEN)
#  include "src/public/dxgigetscreen.h"
# endif // ifndef USE_QTSCREEN
#endif  // if defined(Q_OS_WINDOWS)


screenWidgetShow::screenWidgetShow(QWidget *parent) : QWidget{parent}
{
    QVBoxLayout  *vlayout =  new QVBoxLayout(this);
    titleWidget  *tw = new titleWidget(this);
    screenWidget *sw = new screenWidget(this);

    tw->setMaximumHeight(120);

    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->addWidget(tw);
    vlayout->addWidget(sw);

    connect(tw, &titleWidget::changeWindow, sw, &screenWidget::changeWindow);
}

titleWidget::titleWidget(QWidget *parent) : QWidget{parent}
{
    QVBoxLayout *vlayout =  new QVBoxLayout(this);
    QHBoxLayout *hlayout = new QHBoxLayout;
    QComboBox   *combox = new QComboBox(this);
    QPushButton *pushbotton = new QPushButton(this);
    QLabel *label = new QLabel(this);

    pushbotton->setText("刷新");

    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->addWidget(    combox);
    hlayout->addWidget(pushbotton);
    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->addWidget(label);
    vlayout->addLayout(hlayout);

    connect(pushbotton, &QPushButton::clicked, this, [ = ]() {
        QList<FuncHelper::WindowInfo>list =
            FuncHelper::getInstance().getWindowInfoList();
        qDebug() << list.size();
        combox->clear();
        QVariant hwndqv = QVariant::fromValue((HWND)nullptr);
        combox->addItem("全屏", hwndqv);
        m_list.clear();

        for (int i = 0; i < list.size(); i++) {
            FuncHelper::WindowInfo& winfo = list[i];

            if (!winfo.windowName.isEmpty()) {
                hwndqv = QVariant::fromValue((HWND)winfo.hwnd);
                combox->addItem(winfo.className, hwndqv);
                m_list.append(winfo);
            }
        }
        combox->setCurrentIndex(0);
    });

    connect(combox, &QComboBox::currentIndexChanged, this, [ = ](int index) {
        if (index < 0) return;

        QVariant hwndqv = combox->itemData(index);
        HWND hwnd = hwndqv.value<HWND>();

        if (hwnd) {
            // 最小化
            if (IsIconic(hwnd)) return;

            // IsZoomed(hwnd); // 最大化
        }

        if (index > 0) {
            index--;
            QString str;
            str += "HWND:" + QString::number((quintptr)m_list.at(index).hwnd, 16)
                   + "PROCESS ID:" +
                   QString::number((quintptr)m_list.at(
                                       index).processID, 16) + "\n";
            str += "class name:" + m_list.at(index).className + "\n";
            str += "window name:" + m_list.at(index).windowName + "\n";
            str += "file:" + m_list.at(index).filePathName + "\n";

            label->setText(str);
        }
        emit changeWindow(hwnd);
    });
}

screenWidget::screenWidget(QWidget *parent)
    : QWidget{parent}
{
    setWindowTitle("screenTest");

    int width = 1920, height = 1200;
    width = QGuiApplication::primaryScreen()->geometry().width();
    height = QGuiApplication::primaryScreen()->geometry().height();
#ifndef USE_QTSCREEN
    width *= QGuiApplication::primaryScreen()->devicePixelRatio();
    height *= QGuiApplication::primaryScreen()->devicePixelRatio();
#endif // ifndef USE_QTSCREEN
    m_rect = QRect(0, 0, width, height);

#if !defined(USE_FFMPEG)
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [ = ]() {
# if defined(USE_QTSCREEN)
        getScreen(m_rect.x(), m_rect.y(), m_rect.width(), m_rect.height(), 0);
# else // if 0
        QImage  image;

        // QDateTime tm = QDateTime::currentDateTime();
        DXGIGetScreen::getScreen(image);

        // qDebug() << tm.msecsTo(QDateTime::currentDateTime());
        // tm = QDateTime::currentDateTime();
        pixmap = QPixmap::fromImage(image.copy(m_rect));

        // qDebug() << tm.msecsTo(QDateTime::currentDateTime());
# endif // if 0
        update();
    });
    timer->start(1000 / 25); // 每秒重绘一次
#else // if !defined(USE_FFMPEG)
    // ffmpegScreen::instance().info();
    ffmpegScreen::instance().initparam("desktop",
                                       "25",
                                       QString::number(m_rect.x()),
                                       QString::number(m_rect.y()),
                                       QString("%1x%2").arg(m_rect.width()).
                                       arg(m_rect.height()));
    ffmpegScreen::instance().startwork();
    connect(&ffmpegScreen::instance(), &ffmpegScreen::gotFrame, this,
            [ = ](QImage image) {
        pixmap = QPixmap::fromImage(image);

        // QPixmap::fromImage(image.scaled(rect().size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        update();
    });
#endif // if !defined(USE_FFMPEG)
}

screenWidget::~screenWidget()
{
#if defined(USE_FFMPEG)
    ffmpegScreen::instance().stopwork();
#endif // if defined(USE_FFMPEG)
}

void screenWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        // close();
        showNormal();
    } else if ((event->key() == Qt::Key_Enter) ||
               (event->key() == Qt::Key_Return)) {
        showFullScreen();
    } else {
        QWidget::keyPressEvent(event); // 其他按键交给基类处理
    }
}

void screenWidget::paintEvent(QPaintEvent *event)
{
    if (event->type() == QEvent::Paint) {
        if (pixmap.isNull()) {
            QWidget::paintEvent(event); // 如果没有截图，交给基类处理
            return;
        }

        // 在这里执行绘制操作
        QPainter painter(this);

        pixmap = pixmap.scaled(rect().size(),
                               Qt::KeepAspectRatio, // 保持比例缩放
                               Qt::SmoothTransformation);
        int x = (rect().size().width() - pixmap.width()) / 2;
        int y = (rect().size().height() - pixmap.height()) / 2;

        // painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawPixmap(x, y, pixmap.width(), pixmap.height(), pixmap);

        // 设置字体和颜色
        QFont font("Arial", 30);
        painter.setFont(font);
        painter.setPen(Qt::white);
        m_allcount++;

        if (m_datetime.msecsTo(QDateTime::currentDateTime()) >= 2000) {
            m_fps = qRound(m_count * 1000.0 / m_datetime.msecsTo(
                               QDateTime::currentDateTime()));
            m_datetime = QDateTime::currentDateTime();
            m_count = 0;
        } else {
            m_count++;
        }
        QString str = QString::number(m_fps) + "fps\n" +
                      QString::number(m_allcount);

        // 绘制文本到指定位置
        painter.drawText(QPointF(50, 50), str);
    } else {
        QWidget::paintEvent(event); // 其他事件交给基类处理
    }
}

void screenWidget::getScreen(int x, int y, int w, int h, WId window)
{
    // 应用程序主屏幕
    QScreen *screen = QGuiApplication::primaryScreen();

    pixmap = screen->grabWindow(window, x, y, w, h); // 70~100ms
}

void screenWidget::changeWindow(void *phwnd)
{
    HWND hwnd = (HWND)phwnd;

#if defined(Q_OS_WINDOWS)
    int width = 1920, height = 1200;
    width = QGuiApplication::primaryScreen()->geometry().width();
    height = QGuiApplication::primaryScreen()->geometry().height();
# ifndef USE_QTSCREEN
    width *= QGuiApplication::primaryScreen()->devicePixelRatio();
    height *= QGuiApplication::primaryScreen()->devicePixelRatio();
# endif // ifndef USE_QTSCREEN

    m_rect = QRect(0, 0, width, height);

    if (hwnd) {
        m_rect = FuncHelper::getInstance().getRectNoBorderFromHwnd(hwnd);
        qDebug() << hwnd
                 << FuncHelper::getInstance().getWindowProcessID(hwnd)
                 << FuncHelper::getInstance().getWindowClass(hwnd)
                 << FuncHelper::getInstance().getWindowName(hwnd)
                 << FuncHelper::getInstance().getWindowFilePathName(hwnd);
# ifdef USE_QTSCREEN

        // double zoom = QGuiApplication::primaryScreen()->devicePixelRatio();
        double zoom = FuncHelper::getInstance().getSystemZoom();
        int    w = m_rect.width() / zoom;
        int    h = m_rect.height() / zoom;
        int    x = m_rect.x() / zoom;
        int    y = m_rect.y() / zoom;
        m_rect = QRect(x, y, w, h);
# endif // ifdef USE_QTSCREEN
    }
    qDebug() << hwnd << m_rect;

# if defined(USE_FFMPEG)
    ffmpegScreen::instance().stopwork();
    int x, y, w, h;
    w = m_rect.width();
    h = m_rect.height();
    x = m_rect.x();
    y = m_rect.y();

    if (w < 1) w = 1;

    if (w > width) w = width;

    if (h < 1) h = 1;

    if (h > height) h = height;

    if (x < 0) x = 0;

    if (x > width - 1) x = width - 1;

    if (y < 0) y = 0;

    if (y > height - 1) y = height - 1;

    // QString strtit = "title=" +
    //                  FuncHelper::getInstance().getWindowName(hwnd);
    // QRect rect;
    // FuncHelper::getInstance().getWindowHWNDFromTitle("计算器",rect);
    // x = y = 0;
    // w = rect.width();
    // h = rect.height();
    // qDebug() << rect;
    ffmpegScreen::instance().initparam("desktop",
                                       "25",
                                       QString::number(x),
                                       QString::number(y),
                                       QString("%1x%2")
                                       .arg(w)
                                       .arg(h));

    ffmpegScreen::instance().startwork();
# endif // if !defined(USE_FFMPEG)
#endif  // if defined(Q_OS_WINDOWS)
}
