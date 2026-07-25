#include "testwidgetgl.h"
#include <QVBoxLayout>
#include <QTimer>
#include <QScreen>
#include <QPainter>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QPaintEvent>
#include <QFileDialog>
#include <QMediaDevices>
#include <src/screen/ffmpegscreen.h>
#include <windows.h>
#include <src/public/dxgigetscreen.h>
#include <src/public/continuousscreencapture.h>
#include "regionselector.h"
#include "freehandselector.h"

// #include <src/public/AVEncoder.h>

// #define USEDOPENGLWINDOW

TestWidgetGL::TestWidgetGL(QWidget *parent)
    : QWidget{parent}
{
#ifndef USEDOPENGLWINDOW

    // setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
#endif // ifndef USEDOPENGLWINDOW
    QComboBox   *combox = new QComboBox(this);
    QPushButton *pushbotton = new QPushButton(this);
    QPushButton *pushbottonrect = new QPushButton(this);
    QPushButton *pushbottonscreen = new QPushButton(this);
    checkbox = new QCheckBox(this);
    QPushButton *pushbottonSaveAv1 = new QPushButton(this);
    QCheckBox   *checkboxUsedUpdate = new QCheckBox(this);
    QSpinBox    *spinBox = new QSpinBox(this);
    QCheckBox   *checkboxStart = new QCheckBox("开始采集", this);
    checkboxshow = new QCheckBox(this);
    spinBoxFps = new QSpinBox(this);
    spinBox->setToolTip("保存帧率");
    spinBox->setMaximum(10000);
    spinBox->setMinimum(1);
    spinBox->setValue(30);
    spinBoxFps->setToolTip("最大帧率");
    spinBoxFps->setMaximum(1000);
    spinBoxFps->setMinimum(1);
    spinBoxFps->setValue(1000);
    checkboxshow->setText("显示");
    checkboxshow->setChecked(true);

    labelfps = new QLabel(this);
    checkbox->setText("使用DXGI");
    pushbottonSaveAv1->setText("保存为视频(AV1)");
    checkboxUsedUpdate->setText("仅用最新");
    checkboxUsedUpdate->setChecked(true);
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
    QHBoxLayout *hlayout1 = new QHBoxLayout;
    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout1->setContentsMargins(0, 0, 0, 0);
    hlayout->addWidget(combox);
    hlayout->addWidget(pushbotton);
    hlayout->addWidget(pushbottonrect);
    hlayout->addWidget(pushbottonscreen);
    hlayout->addWidget(checkboxStart);
    hlayout->addWidget(labelfps);

    hlayout1->addWidget(checkboxshow);
    hlayout1->addWidget(spinBoxFps);
    hlayout1->addWidget(checkbox);
    hlayout1->addWidget(pushbottonSaveAv1);
    hlayout1->addWidget(checkboxUsedUpdate);
    hlayout1->addWidget(spinBox);


    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->addWidget(label);
    vlayout->addLayout(hlayout);
    vlayout->addLayout(hlayout1);

    // AVEncoder::test();
    // AV1Encoder::test();

#ifdef USEDOPENGLWINDOW
    m_glImageWindow = new GLImageWindow;
    m_glImageWindow->setDisplayMode(GLImageWindow::Fit);
    connect(this,
            &TestWidgetGL::loadImage,
            m_glImageWindow,
            &GLImageWindow::loadImage);

    // windows 嵌入 widget 透明不能有效设置
    // m_glImageWindow->resize(600, 600); m_glImageWindow->show();
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

    m_systemAudioSource = new systemAudioSource;
    connect(m_systemAudioSource, &systemAudioSource::audioSourceStatus, this,
            [ = ](QAudio::State state) {
        if ((state == QAudio::SuspendedState) ||
            (state == QAudio::StoppedState)) {
            qDebug() << state;
        }
    });
    connect(m_systemAudioSource, &systemAudioSource::readData, this,
            [ = ](QByteArray data) {
        if (m_AVEncoderThread) m_AVEncoderThread->addAudio(data);
    });
    connect(this, &TestWidgetGL::saveImage,
            this, [ = ](const QImage& image) {
        if (m_AVEncoderThread) m_AVEncoderThread->addImage(image);
    });
    m_thread = new QThread(this);
    m_systemAudioSource->moveToThread(m_thread);
    m_thread->start();

    connect(checkbox, &QCheckBox::checkStateChanged, this, [ = ]
    {
        if (m_ffmscreen) {
            delete m_ffmscreen;
            m_ffmscreen = nullptr;
        }

        if (m_capturer) {
            delete m_capturer;
            m_capturer = nullptr;
        }
        resetSaveObject();

        if (checkboxStart->isChecked()) {
            startCollect();
        }
    });
    connect(pushbottonSaveAv1, &QPushButton::clicked, this, [ = ]() {
        QString fileNames = QFileDialog::getSaveFileName(this,
                                                         QStringLiteral("保存"),
                                                         "./",
                                                         QStringLiteral(
                                                             "(*.mp4)"));

        if (fileNames.isEmpty()) return;

        if (!checkboxStart->isChecked()) return;

        if (QFile::exists(fileNames)) {
            // 如果还在处理，这里不允许覆盖
            for (auto it = m_saveAV1Map.constBegin();
                 it != m_saveAV1Map.constEnd();
                 ++it) {
                if (it.value() == fileNames) {
                    return;
                }
            }
        }

        if (m_AVEncoderThread) {
            m_AVEncoderThread->stop();
            m_AVEncoderThread = nullptr;
            m_systemAudioSource->colseSource();
        }

        if ((m_imageWidth > 0) && (m_imageHeight > 0)) {
            int sampleRate = 48000;
            int channelCount = 2;

            m_AVEncoderThread = new AVEncoderThread(fileNames,
                                                    m_imageWidth,
                                                    m_imageHeight,
                                                    spinBox->value(),
                                                    sampleRate,
                                                    channelCount
                                                    );
            m_saveAV1Map.insert(m_AVEncoderThread, fileNames);
            m_AVEncoderThread->usedUpdate(checkboxUsedUpdate->isChecked());
            connect(m_AVEncoderThread, &AVEncoderThread::finish, this,
                    [ = ]() {
                AVEncoderThread *sam =
                    qobject_cast<AVEncoderThread *>(sender());

                if (m_saveAV1Map.contains(sam)) {
                    m_saveAV1Map.remove(sam);
                }
                sam->quit();

                // sam->wait();
                // sam->deleteLater();
            });
            connect(m_AVEncoderThread, &AVEncoderThread::finished,
                    m_AVEncoderThread, &AVEncoderThread::deleteLater);

            /////////////////////////////////////////
            QAudioFormat format;
            format.setSampleRate(sampleRate);
            format.setSampleFormat(QAudioFormat::Int16);
            format.setChannelCount(channelCount);

            // int bufferSize = m_AVEncoderThread->getAudioOneFrameSize() * 2;
            int bufferSize = 1024 * 2 * 2;

            // format.setChannelConfig(QAudioFormat::ChannelConfigStereo);

            // 1. 获取所有音频输入设备
            const QList<QAudioDevice>inputDevices = QMediaDevices::audioInputs();
            QAudioDevice stereoMixDevice;

            // 2. 遍历查找立体声混音设备
            for (const QAudioDevice& device : inputDevices) {
                if (device.description().contains("立体声混音",
                                                  Qt::CaseInsensitive) ||
                    device.description().contains("Stereo Mix",
                                                  Qt::CaseInsensitive))
                {
                    stereoMixDevice = device;
                    break;
                }
            }

            if (stereoMixDevice.isNull()) {
                return;
            }

            if (!stereoMixDevice.isFormatSupported(format)) return;

            m_systemAudioSource->openSource(format, stereoMixDevice, bufferSize);

            m_AVEncoderThread->start();
            qInfo() << fileNames << m_imageWidth << m_imageHeight << sampleRate << channelCount;
        }
    });
    connect(checkboxUsedUpdate, &QCheckBox::checkStateChanged, this, [ = ]
    {
        if (m_AVEncoderThread) {
            m_AVEncoderThread->usedUpdate(checkboxUsedUpdate->isChecked());
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

        if (m_rect.isEmpty() || (m_rect.width() < 1) || (m_rect.height() < 1)) return;

        qreal scaleFactor = QGuiApplication::primaryScreen()->devicePixelRatio();
        m_rect.setRect(m_rect.x() * scaleFactor,
                       m_rect.y() * scaleFactor,
                       m_rect.width() * scaleFactor,
                       m_rect.height() * scaleFactor
                       );
        m_screenRect = m_rect;
        isHwnd = false;
        m_isPath = false;

        if (checkboxStart->isChecked()) {
            startCollect();
        }
    });

    connect(pushbottonscreen, &QPushButton::clicked, this, [ = ]() {
        if (0) {
            FreehandSelector fselector;
            QImage image = fselector.selectShapeScreen();
            emit loadImage(image);
            image.save("./test.png");
            return;
        }


        FreehandSelector fselector;
        m_path = fselector.selectShape();
        QRect m_rect = m_path.boundingRect().toRect();

        if (m_rect.isEmpty() || (m_rect.width() < 1) || (m_rect.height() < 1)) return;

        qreal scaleFactor = QGuiApplication::primaryScreen()->devicePixelRatio();
        m_rect.setRect(m_rect.x() * scaleFactor,
                       m_rect.y() * scaleFactor,
                       m_rect.width() * scaleFactor,
                       m_rect.height() * scaleFactor
                       );
        m_screenRect = m_rect;
        isHwnd = false;

        m_isPath = true;

        if (checkboxStart->isChecked()) {
            startCollect();
        }
    });

    connect(checkboxStart, &QCheckBox::checkStateChanged, this, [ = ]() {
        if (checkboxStart->isChecked()) {
            startCollect();
        } else {
            if (m_ffmscreen) {
                delete m_ffmscreen;
                m_ffmscreen = nullptr;
            }

            if (m_capturer) {
                delete m_capturer;
                m_capturer = nullptr;
            }
            resetSaveObject();
        }
    });


    connect(combox, &QComboBox::currentIndexChanged, this, [ = ](int index) {
        if (index < 0) return;

        QVariant hwndqv = combox->itemData(index);
        HWND hwnd = hwndqv.value<HWND>();
        m_hwnd = hwnd;
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
        m_screenRect = m_rect;

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
            isHwnd = false;
        } else {
            isHwnd = true;
        }

        if (checkboxStart->isChecked()) {
            startCollect();
        }
    });
}

TestWidgetGL::~TestWidgetGL()
{
    if (m_ffmscreen) {
        delete m_ffmscreen;
    }

    if (m_capturer) {
        delete m_capturer;
    }

    if (m_glImageWindow) delete m_glImageWindow;

    if (m_AVEncoderThread) m_AVEncoderThread->stop();

    m_systemAudioSource->colseSource();
    m_thread->quit();
    m_thread->wait();
    delete m_thread;
    delete m_systemAudioSource;
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
    resetSaveObject();

    if (!m_ffmscreen) {
        m_ffmscreen = new ffmpegScreen;

        connect(m_ffmscreen, &ffmpegScreen::gotFrame, this,
                [ = ](QImage image) {
            m_image = image;
            m_imageWidth = m_image.width();
            m_imageHeight = m_image.height();

            if (m_isPath) {
                qreal scaleFactor = QGuiApplication::primaryScreen()->devicePixelRatio();
                m_image = FreehandSelector::fromPath(m_image, m_path,
                                                     scaleFactor);
            }
            setImageInfo(m_image, false);

            if (checkboxshow->isChecked()) emit loadImage(m_image);

            if (m_AVEncoderThread) emit saveImage(m_image);

            // update();
        });
    }

    // QString hwndstr = "hwnd=" + QString::number((quintptr)hwnd, 10);
    m_ffmscreen->stopwork();
    m_ffmscreen->initparam("desktop",
                           QString::number(spinBoxFps->value()),
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
        resetSaveObject();

        connect(m_capturer, &ContinuousScreenCapture::frameCaptured,
                this, [this](const QImage& img) {
            m_image = img;
            m_imageWidth = m_image.width();
            m_imageHeight = m_image.height();

            if (m_isPath) {
                qreal scaleFactor = QGuiApplication::primaryScreen()->devicePixelRatio();
                m_image = FreehandSelector::fromPath(m_image, m_path,
                                                     scaleFactor);
            }
            setImageInfo(m_image, false);

            if (checkboxshow->isChecked()) emit loadImage(m_image);

            if (m_AVEncoderThread) emit saveImage(m_image);

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
    m_capturer->setLowFpsMode(true, spinBoxFps->value());
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

        {
            QPixmap pixmap = QPixmap::fromImage(m_image)
                             .scaled(rect().size(),
                                     Qt::KeepAspectRatio, // 保持比例缩放
                                     Qt::SmoothTransformation
                                     );
            int x = (rect().size().width() - pixmap.width()) / 2;
            int y = (rect().size().height() - pixmap.height()) / 2;

            // painter.setRenderHint(QPainter::Antialiasing, true);
            painter.drawPixmap(x, y, pixmap.width(), pixmap.height(), pixmap);
        }

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

void TestWidgetGL::resetSaveObject()
{
    m_imageWidth = -1;
    m_imageHeight = -1;

    if (m_AVEncoderThread) {
        m_AVEncoderThread->stop();
        m_AVEncoderThread = nullptr;
        m_systemAudioSource->colseSource();
    }
}

void TestWidgetGL::startCollect()
{
    if (!isHwnd && (m_screenRect.isEmpty() ||
                    (m_screenRect.width() < 1) ||
                    (m_screenRect.height() < 1))) return;

    if (!checkbox->isChecked()) {
        qDebug() << m_screenRect;
        loaderImageFFmpeg(m_screenRect);
    } else {
        qDebug() << m_screenRect << m_hwnd;
        loaderImageDXGI(m_screenRect, m_hwnd, isHwnd);
    }
}
