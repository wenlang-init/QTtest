#include "mediaplayerwidget.h"
#include <QVideoWidget>
#include <QStyle>
#include <QFileDialog>
#include <QStandardPaths>
#include <QAudioOutput>
#include <QMediaMetaData>
#include <QMediaPlayer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QTime>
#include <QImage>
#include <QGraphicsBlurEffect>
#include <QVideoSink>
#include <QMetaEnum>
#include <QVideoFrame>
#include <QTimeEdit>
#include "popwidget.h"
#define qdebug qDebug().noquote() << "[" << __FILE__ << ":" << __LINE__ << "] " << \
        __FUNCTION__ << "() "

mediaPlayerWidget::mediaPlayerWidget(QWidget *parent)
    : QWidget{parent}
{
    init();
    installEventFilter(this);
}

mediaPlayerWidget::~mediaPlayerWidget()
{
    delete pop;
}

void mediaPlayerWidget::setUrl(QUrl url)
{
    m_mediaPlayer->setSource(url);
    btn->setEnabled(true);
    btnjump->setEnabled(true);

    this->setWindowTitle(url.toString());

    qdebug << m_mediaPlayer->activeAudioTrack()
           << m_mediaPlayer->activeVideoTrack()
           << m_mediaPlayer->activeSubtitleTrack();
    qdebug << m_mediaPlayer->audioTracks().size()
           << m_mediaPlayer->videoTracks().size()
           << m_mediaPlayer->subtitleTracks().size();
    qdebug << m_mediaPlayer->duration()
           << m_mediaPlayer->position()
           << m_mediaPlayer->playbackState();
    qdebug << videowidget->videoSink()->videoSize()
           << videowidget->videoSink()->subtitleText();
    qdebug << m_mediaPlayer->videoSink()->videoSize()
           << m_mediaPlayer->videoSink()->subtitleText()
           << m_mediaPlayer->mediaStatus();
}

void mediaPlayerWidget::setFullScreen(bool fullscreen)
{
    videowidget->setFullScreen(fullscreen);
}

void mediaPlayerWidget::setShowM(bool isshow)
{
    if (isshow) {
        widget->show();
    } else {
        widget->hide();
    }
}

bool mediaPlayerWidget::isShowM()
{
    return !widget->isHidden();
}

QMap<QString, QString>mediaPlayerWidget::getMetaData()
{
    return m_metaData;
}

void mediaPlayerWidget::init()
{
    videowidget = new QVideoWidget(this);
    widget = new QWidget(this);
    QSlider *sliderVolumn = new QSlider(widget);
    QSlider *slider = new QSlider(widget);
    QLabel  *label = new QLabel(widget);

    btn = new QPushButton(widget);

    widget->setMaximumHeight(50);
    btn->setFixedSize(20, 20);
    btn->setEnabled(false);
    btn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    sliderVolumn->setOrientation(Qt::Orientation::Horizontal);
    sliderVolumn->setFixedHeight(20);
    sliderVolumn->setRange(0, 100);
    sliderVolumn->setValue(sliderVolumn->maximum());

    timeEdit = new QTimeEdit(widget);
    timeEdit->setDisplayFormat("hh:mm:ss.zzz");
    timeEdit->setDate(QDate::currentDate());
    timeEdit->setMinimumWidth(115); // timeEdit->setMaximumWidth(115);
    btnjump = new QPushButton(widget);
    btnjump->setText("跳转");
    btnjump->setFixedSize(50, 20);
    btnjump->setEnabled(false);
    btnperv = new QPushButton(widget);
    btnperv->setText("上一帧");

    // btnperv->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    btnperv->setFixedSize(60, 20);
    btnnext = new QPushButton(widget);
    btnnext->setText("下一帧");

    // btnnext->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    btnnext->setFixedSize(60, 20);

    slider->setOrientation(Qt::Orientation::Horizontal);
    slider->setFixedHeight(20);
    slider->setRange(0, 0);
    label->setFixedHeight(20);
    label->setMaximumWidth(300);
    label->setText("0/0");

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0, 0, 0, 0);

    vlayout->addWidget(videowidget);
    vlayout->addWidget(widget);

    QVBoxLayout *vlayout0 = new QVBoxLayout(widget);
    QHBoxLayout *hlayout0 = new QHBoxLayout;
    QHBoxLayout *hlayout1 = new QHBoxLayout;
    vlayout0->setContentsMargins(0, 0, 0, 0);
    hlayout0->setContentsMargins(0, 0, 0, 0);
    hlayout1->setContentsMargins(0, 0, 0, 0);
    hlayout0->addWidget(slider);
    hlayout0->addWidget(label);
    hlayout1->addWidget(sliderVolumn);
    hlayout1->addWidget(btn);
    hlayout1->addWidget(timeEdit);
    hlayout1->addWidget(btnjump);
    hlayout1->addWidget(btnperv);
    hlayout1->addWidget(btnnext);
    vlayout0->addLayout(hlayout0);
    vlayout0->addLayout(hlayout1);

    pop = new popWidget("");
    pop->hide();
    pop->move((width() - pop->width()) / 2, (height() - pop->height()) / 2);


    // 创建模糊效果
    // QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect(this);
    // blurEffect->setBlurRadius(10); // 设置模糊半径
    // // 将模糊效果应用到 QVideoWidget
    // videowidget->setGraphicsEffect(blurEffect);

    m_mediaPlayer = new QMediaPlayer(this);
    m_mediaPlayer->setVideoOutput(videowidget);

    QAudioOutput *audioOutput = new QAudioOutput(this);
    m_mediaPlayer->setAudioOutput(audioOutput);

    connect(btnperv, &QPushButton::clicked, this, [ = ]() {
        qint64 currentPos = m_mediaPlayer->position();

        double frameRate =
            m_mediaPlayer->metaData().
            value(QMediaMetaData::VideoFrameRate).toDouble();

        // 防止帧率为0或无效，可设置一个默认值，如25
        if (frameRate <= 0) {
            frameRate = 25.0;
        }
        int stepMs = static_cast<int>(1000.0 / frameRate);

        // 计算上一帧位置
        qint64 Pos = currentPos - stepMs;

        if (Pos <= 0) {
            Pos = 0;
        }

        // 执行跳转
        m_mediaPlayer->setPosition(Pos);
    });
    connect(btnnext, &QPushButton::clicked, this, [ = ]() {
        qint64 currentPos = m_mediaPlayer->position();
        qint64 duration = m_mediaPlayer->duration();

        double frameRate =
            m_mediaPlayer->metaData().
            value(QMediaMetaData::VideoFrameRate).toDouble();

        // 防止帧率为0或无效，可设置一个默认值，如25
        if (frameRate <= 0) {
            frameRate = 25.0;
        }
        int stepMs = static_cast<int>(1000.0 / frameRate);

        // 计算下一帧位置，确保不超过总时长
        qint64 nextPos = currentPos + stepMs;

        if (nextPos >= duration) {
            nextPos = duration; // 或者设为0，实现循环播放
        }

        // 执行跳转
        m_mediaPlayer->setPosition(nextPos);
    });
    connect(btnjump, &QPushButton::clicked, this, [ = ]() {
        m_mediaPlayer->setPosition(timeEdit->time().msecsSinceStartOfDay());
    });
    connect(sliderVolumn, &QAbstractSlider::valueChanged, this,
            [ = ](int position) {
        m_mediaPlayer->audioOutput()->setVolume(position * 0.01);
    });
    connect(slider, &QAbstractSlider::sliderReleased, this,
            [ = ]() {
        m_mediaPlayer->setPosition(slider->value());

        pop->hide();
    });
    connect(slider, &QAbstractSlider::sliderPressed, this,
            [ = ]() {
        QPoint point = mapToGlobal(pos());
        pop->move(point.x() + (width() - pop->width()) / 2,
                  point.y() + (height() - pop->height()) / 2);
        pop->show();
    });

    connect(slider, &QAbstractSlider::sliderMoved, this,
            [ = ](int position) {
        QPoint point = mapToGlobal(pos());
        pop->move(point.x() + (width() - pop->width()) / 2,
                  point.y() + (height() - pop->height()) / 2);
        pop->show();
        QString str(QTime::fromMSecsSinceStartOfDay(position).toString(
                        "hh:mm:ss"));
        QString strmax;
        strmax.append(QTime::fromMSecsSinceStartOfDay(m_mediaPlayer->duration()).
                      toString("hh:mm:ss"));
        QString text = str + "/" + strmax;

        pop->settext(text);
    });
    connect(btn, &QAbstractButton::clicked, this, [ = ]() {
        switch (m_mediaPlayer->playbackState()) {
        case QMediaPlayer::PlayingState:
            m_mediaPlayer->pause();
            break;

        default:
            m_mediaPlayer->play();
            break;
        }
    });
    connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged, this,
            [ = ](QMediaPlayer::PlaybackState state) {
        switch (state) {
        case QMediaPlayer::PlayingState:
            btn->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
            break;

        default:
            btn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            break;
        }
    });
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this,
            [ = ](qint64 position) {
        if (!slider->isSliderDown()) {
            slider->setValue(position);
        }
        QString str(QTime::fromMSecsSinceStartOfDay(position).toString(
                        "hh:mm:ss.zzz"));
        QString strmax;
        strmax.append(QTime::fromMSecsSinceStartOfDay(m_mediaPlayer->duration()).
                      toString("hh:mm:ss.zzz"));
        label->setText(str + "/" + strmax);
    });
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this,
            [ = ](qint64 duration) {
        timeEdit->setMinimumTime(QTime(0, 0));
        timeEdit->setMaximumTime(QTime::fromMSecsSinceStartOfDay(duration));
        slider->setRange(0, duration);
        QString str("0/");
        str.append(QTime::fromMSecsSinceStartOfDay(duration).toString(
                       "hh:mm:ss.zzz"));
        label->setText(str);
    });
    connect(m_mediaPlayer, &QMediaPlayer::errorChanged, this,
            [ = ]() {
        if (m_mediaPlayer->error() == QMediaPlayer::NoError) return;

        btnjump->setEnabled(false);
        btn->setEnabled(false);
        const QString errorString = m_mediaPlayer->errorString();
        QString message = "Error: ";

        if (errorString.isEmpty()) message += " #" +
                                              QString::number(int(m_mediaPlayer->
                                                                  error()));
        else message += errorString;
        qdebug << message;
    });

    // 媒体信息
    connect(m_mediaPlayer, &QMediaPlayer::metaDataChanged, this, [ = ]() {
        // QMediaPlayer对象将媒体文件准备好了
        if (m_mediaPlayer->isAvailable())
        {
            // 读取信息
            auto data = m_mediaPlayer->metaData();

            // qdebug << data.value(QMediaMetaData::Title).toString()
            //        << data
            //        << data.value(QMediaMetaData::VideoFrameRate).toDouble()
            //        << data.value(QMediaMetaData::VideoCodec).toString()
            //        << data.value(QMediaMetaData::Resolution).toSize()
            //        << data.value(QMediaMetaData::Duration).toLongLong()
            //        << data.value(QMediaMetaData::FileFormat).toString()
            //        << data.value(QMediaMetaData::AudioBitRate).toInt()
            //        << data.value(QMediaMetaData::HasHdrContent).toBool()
            //        << data.value(QMediaMetaData::AudioCodec).toString()
            //        << data.value(QMediaMetaData::VideoBitRate).toInt();
            QList<QMediaMetaData::Key>keys = data.keys();
            QMetaEnum metaEnum = QMetaEnum::fromType<QMediaMetaData::Key>();

            for (int i = 0; i < keys.size(); i++) {
// qdebug << QString(metaEnum.key(keys[i])) << data.value(keys[i]);
                m_metaData[QString(metaEnum.key(keys[i]))] = data.value(
                    keys[i]).toString();
            }
        }
    });

    // 当前帧
    connect(m_mediaPlayer->videoSink(),
            &QVideoSink::videoFrameChanged,
            this,
            [ = ](const QVideoFrame& frame) {
        // frame.toImage();
        // qdebug << m_mediaPlayer->videoSink()->videoFrame() << frame;
    });

    // 当前视频大小
    connect(m_mediaPlayer->videoSink(),
            &QVideoSink::videoSizeChanged,
            this,
            [ = ]() {
        // qdebug << m_mediaPlayer->videoSink()->videoSize();
    });

    // 当前字幕
    connect(m_mediaPlayer->videoSink(),
            &QVideoSink::subtitleTextChanged,
            this,
            [ = ](const QString& subtitleText) {
        // qdebug << m_mediaPlayer->videoSink()->subtitleText() << subtitleText;
    });
}

bool mediaPlayerWidget::eventFilter(QObject *watched, QEvent *event)
{
    return QWidget::eventFilter(watched, event);
}
