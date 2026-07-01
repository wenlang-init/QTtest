#include "testwidgetgl.h"
#include <QVBoxLayout>
#include <QTimer>
#include <QScreen>
#include <QPainter>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QPaintEvent>
#include <src/screen/ffmpegscreen.h>
#include <windows.h>
#include <src/public/dxgigetscreen.h>
#include <src/public/continuousscreencapture.h>
#include "regionselector.h"
#include "freehandselector.h"

#define USEDOPENGLWINDOW

TestWidgetGL::TestWidgetGL(QWidget *parent)
    : QWidget{parent}
{
    QComboBox   *combox = new QComboBox(this);
    QPushButton *pushbotton = new QPushButton(this);
    QPushButton *pushbottonrect = new QPushButton(this);
    QPushButton *pushbottonscreen = new QPushButton(this);
    QCheckBox   *checkbox = new QCheckBox(this);

    labelfps = new QLabel(this);
    checkbox->setText("使用DXGI");
    pushbotton->setText("刷新");
    pushbottonrect->setText("选择区域");
    pushbottonscreen->setText("选择任意区域");
    labelfps->setMaximumHeight(30);
    labelfps->adjustSize();
    QLabel *label = new QLabel(this);
    label->setMaximumHeight(80);
    label->setWordWrap(true); // 自动换行
    label->adjustSize();      // 自适应大小
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->addWidget(combox);
    hlayout->addWidget(pushbotton);
    hlayout->addWidget(pushbottonrect);
    hlayout->addWidget(pushbottonscreen);
    hlayout->addWidget(checkbox);
    hlayout->addWidget(labelfps);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->addWidget(label);
    vlayout->addLayout(hlayout);


#ifdef USEDOPENGLWINDOW
    m_glImageWindow = new GLImageWindow;
    m_glImageWindow->setDisplayMode(GLImageWindow::Fit);
    connect(this,
            &TestWidgetGL::loadImage,
            m_glImageWindow,
            &GLImageWindow::loadImage);
    QWidget *widget = createWindowContainer(m_glImageWindow, this);
    vlayout->addWidget(widget);
#else // ifdef USEDOPENGLWINDOW
    m_glImageWidget = new GLImageWidget(this);
    connect(this,
            &TestWidgetGL::loadImage,
            m_glImageWidget,
            &GLImageWidget::loadImage);
    m_glImageWidget->setDisplayMode(GLImageWidget::Fit);
    vlayout->addWidget(m_glImageWidget);
#endif // ifdef USEDOPENGLWINDOW

    connect(checkbox, &QCheckBox::checkStateChanged, this, [ = ]
    {
        if (m_ffmscreen) {
            m_ffmscreen->deleteLater();
            m_ffmscreen = nullptr;
        }

        if (m_capturer) {
            delete m_capturer;
            m_capturer = nullptr;
        }
    });

    connect(pushbotton, &QPushButton::clicked, this, [ = ]() {
        QList<FuncHelper::WindowInfo>list =
            FuncHelper::getInstance().getWindowInfoList();
        combox->clear();
        QVariant hwndqv = QVariant::fromValue((HWND)nullptr);
        combox->addItem("全屏", hwndqv);
        m_list.clear();

        for (int i = 0; i < list.size(); i++) {
            FuncHelper::WindowInfo& winfo = list[i];

            // if (!winfo.windowName.isEmpty())
            {
                hwndqv = QVariant::fromValue((HWND)winfo.hwnd);
                combox->addItem(winfo.windowName + " : " + winfo.className,
                                hwndqv);
                m_list.append(winfo);
            }
        }
        combox->setCurrentIndex(0);
    });

    connect(pushbottonrect, &QPushButton::clicked, this, [ = ]() {
        RegionSelector selector;
        QRect m_rect = selector.selectRegion();
        qreal scaleFactor = QGuiApplication::primaryScreen()->devicePixelRatio();
        m_rect.setRect(m_rect.x() * scaleFactor,
                       m_rect.y() * scaleFactor,
                       m_rect.width() * scaleFactor,
                       m_rect.height() * scaleFactor
                       );

        qDebug() << m_rect;
        m_isPath = false;

        if (!checkbox->isChecked()) {
            loaderImageFFmpeg(m_rect);
        } else {
            loaderImageDXGI(m_rect, nullptr, false);
        }
    });

    connect(pushbottonscreen, &QPushButton::clicked, this, [ = ]() {
        if (0) {
            FreehandSelector fselector;
            QImage image = fselector.selectShapeScreen();
            emit loadImage(image);
            return;
        }


        FreehandSelector fselector;
        m_path = fselector.selectShape();
        QRect m_rect = m_path.boundingRect().toRect();

        qreal scaleFactor = QGuiApplication::primaryScreen()->devicePixelRatio();
        m_rect.setRect(m_rect.x() * scaleFactor,
                       m_rect.y() * scaleFactor,
                       m_rect.width() * scaleFactor,
                       m_rect.height() * scaleFactor
                       );

        qDebug() << m_rect;
        m_isPath = true;

        if (!checkbox->isChecked()) {
            loaderImageFFmpeg(m_rect);
        } else {
            loaderImageDXGI(m_rect, nullptr, false);
        }
    });


    connect(combox, &QComboBox::currentIndexChanged, this, [ = ](int index) {
        if (index < 0) return;

        QVariant hwndqv = combox->itemData(index);
        HWND hwnd = hwndqv.value<HWND>();
        int width = 1920, height = 1200;
        width = QGuiApplication::primaryScreen()->geometry().width();
        height = QGuiApplication::primaryScreen()->geometry().height();
        width *= QGuiApplication::primaryScreen()->devicePixelRatio();
        height *= QGuiApplication::primaryScreen()->devicePixelRatio();
        QRect recta(0, 0, width, height), rect = recta, m_rect = recta;

        // 最小化
        // if (IsIconic(hwnd))return;
        // IsZoomed(hwnd); // 最大化
        recta = FuncHelper::getInstance().getRectFromHwnd(hwnd);
        rect = FuncHelper::getInstance().getRectNoBorderFromHwnd(hwnd);

        if (!recta.isNull()) {
            m_rect = recta;
        }
        qDebug() << hwnd
                 << FuncHelper::getInstance().getWindowProcessID(hwnd)
                 << FuncHelper::getInstance().getWindowClass(hwnd)
                 << FuncHelper::getInstance().getWindowName(hwnd)
                 << FuncHelper::getInstance().getWindowFilePathName(hwnd);

        if (index > 0) {
            index--;
            QString s1, str;
            s1 =
                QString(" rectA(%1,%2,%3x%4)")
                .arg(recta.x()).arg(recta.y())
                .arg(recta.width()).arg(recta.height());
            s1 +=
                QString(",rect(%1,%2,%3x%4)")
                .arg(rect.x()).arg(rect.y())
                .arg(rect.width()).arg(rect.height());

            str += "HWND:" + QString::number((quintptr)m_list.at(index).hwnd, 16)
                   + " PROCESS ID:"  +
                   QString::number((quintptr)m_list.at(index).processID, 16)
                   + s1 + "\n";
            str += "class name:" + m_list.at(index).className + "\n";
            str += "window name:" + m_list.at(index).windowName + "\n";
            str += "file:" + m_list.at(index).filePathName + "\n";

            label->setText(str);
        } else {
            label->clear();
        }

        m_isPath = false;

        if (!checkbox->isChecked()) {
            loaderImageFFmpeg(m_rect);
        } else {
            loaderImageDXGI(QRect(),
                            hwnd,
                            true);
        }
    });
}

TestWidgetGL::~TestWidgetGL()
{
    if (m_ffmscreen) {
        m_ffmscreen->deleteLater();
    }

    if (m_capturer) {
        delete m_capturer;
    }

    if (m_glImageWindow) delete m_glImageWindow;
}

void TestWidgetGL::loaderImageFFmpeg(const QRect& m_rect)
{
    int width = 1920, height = 1200;

    width = QGuiApplication::primaryScreen()->geometry().width();
    height = QGuiApplication::primaryScreen()->geometry().height();
    width *= QGuiApplication::primaryScreen()->devicePixelRatio();
    height *= QGuiApplication::primaryScreen()->devicePixelRatio();

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

    if (m_ffmscreen) {
        delete m_ffmscreen;
        m_ffmscreen = nullptr;
    }

    if (!m_ffmscreen) {
        m_ffmscreen = new ffmpegScreen;
        connect(m_ffmscreen, &ffmpegScreen::gotFrame, this,
                [ = ](QImage image) {
            if (m_isPath) {
                qreal scaleFactor = QGuiApplication::primaryScreen()->devicePixelRatio();
                image = FreehandSelector::fromPath(image, m_path, scaleFactor);
            }
            setImageInfo(image, false);
            emit loadImage(image);

            // update();
        });
    }

    // QString hwndstr = "hwnd=" + QString::number((quintptr)hwnd, 10);
    m_ffmscreen->stopwork();
    m_ffmscreen->initparam("desktop",
                           "25",
                           QString::number(x),
                           QString::number(y),
                           QString("%1x%2").arg(w).arg(h)
                           );
    m_ffmscreen->startwork();
}

void TestWidgetGL::loaderImageDXGI(const QRect& m_rect, HWND hwnd, bool ishwnd)
{
    if (!m_capturer) {
        m_capturer = new ContinuousScreenCapture(hwnd);

        // m_capturer->setLowFpsMode(true, 15);
        connect(m_capturer, &ContinuousScreenCapture::frameCaptured,
                this, [this](const QImage& img) {
            QImage image = img;

            if (m_isPath) {
                qreal scaleFactor = QGuiApplication::primaryScreen()->devicePixelRatio();
                image = FreehandSelector::fromPath(image, m_path, scaleFactor);
            }
            setImageInfo(image, false);
            emit loadImage(image);

            // update();
        });
        m_capturer->start(); // 开始连续捕获
    }

    if (ishwnd) {
        m_capturer->setHwnd(hwnd);
    } else {
        m_capturer->setScreenSize(m_rect.x(),
                                  m_rect.y(),
                                  m_rect.width(),
                                  m_rect.height());
    }
}

void TestWidgetGL::setImageInfo(QImage& image, bool showsy)
{
    m_allcount++;

    if (m_datetime.msecsTo(QDateTime::currentDateTime()) >= 1000) {
        m_fps = qRound(m_count * 1000.0 / m_datetime.msecsTo(
                           QDateTime::currentDateTime()));
        m_datetime = QDateTime::currentDateTime();
        m_count = 0;
    } else {
        m_count++;
    }

    QString str = QString::number(m_fps) + "fps " +
                  QString::number(m_allcount);

    labelfps->setText(str);

    if (showsy) {
        QPainter painter(&image);
        painter.setPen(Qt::red);             // 文字颜色
        painter.setFont(QFont("Arial", 30)); // 字体
        painter.drawText(QPointF(50, 50), str);
        painter.end();
    }
}

void TestWidgetGL::paintEvent(QPaintEvent *event)
{
    return QWidget::paintEvent(event);

    if (event->type() == QEvent::Paint) {
        // 在这里执行绘制操作
        QPainter painter(this);

        // 设置字体和颜色
        QFont font("Arial", 30);
        painter.setFont(font);
        painter.setPen(Qt::white);
        QString str = QString::number(m_fps) + "fps " +
                      QString::number(m_allcount);

        // 绘制文本到指定位置
        painter.drawText(QPointF(50, 50), str);
    } else {
        QWidget::paintEvent(event); // 其他事件交给基类处理
    }
}
