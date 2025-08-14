#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVideoWidget>
#include <QStyle>
#include <QFileDialog>
#include <QStandardPaths>
#include <QAudioOutput>
#include <QMediaMetaData>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_mediaPlayer = new QMediaPlayer(this);
    m_mediaPlayer->setVideoOutput(ui->widget);

    QAudioOutput *audioOutput = new QAudioOutput(this);
    m_mediaPlayer->setAudioOutput(audioOutput);

    ui->horizontalSlidervolume->setRange(0, 100);
    ui->horizontalSlidervolume->setValue(ui->horizontalSlidervolume->maximum());

    // m_mediaPlayer->audioOutput()->setVolume(1);

    // connect(ui->horizontalSlidervolume, &QAbstractSlider::sliderMoved, this,
    //         [ = ](int position) {
    //     m_mediaPlayer->audioOutput()->setVolume(position * 0.01);
    // });
    connect(ui->horizontalSlidervolume, &QAbstractSlider::valueChanged, this,
            [ = ](int position) {
        m_mediaPlayer->audioOutput()->setVolume(position * 0.01);
    });

    ui->horizontalSlider->setRange(0, 0);
    connect(ui->horizontalSlider, &QAbstractSlider::sliderMoved, this,
            [ = ](int position) {
        m_mediaPlayer->setPosition(position);
    });

    // connect(ui->horizontalSlider, &QAbstractSlider::sliderReleased, this,
    //         [ = ]() {
    //     m_mediaPlayer->setPosition(ui->horizontalSlider->value());
    // });

    connect(ui->openButton, &QAbstractButton::clicked, this, [ = ]() {
        QFileDialog fileDialog(this);
        fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
        fileDialog.setWindowTitle(tr("Open Movie"));
        fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::
                                                                  MoviesLocation)
                                .value(0, QDir::homePath()));

        if (fileDialog.exec() == QDialog::Accepted) {
            const QUrl url = fileDialog.selectedUrls().constFirst();
            seturl(url);
        }
    });

    ui->m_playButton->setEnabled(false);
    ui->m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(ui->m_playButton, &QAbstractButton::clicked, this, [ = ]() {
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
            ui->m_playButton->setIcon(style()->standardIcon(
                                          QStyle::SP_MediaPause));
            break;

        default:
            ui->m_playButton->setIcon(style()->standardIcon(
                                          QStyle::SP_MediaPlay));
            break;
        }
    });
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this,
            [ = ](qint64 position) {
        ui->horizontalSlider->setValue(position);
        QString str(QTime::fromMSecsSinceStartOfDay(position).toString(
                        "hh:mm:ss"));
        QString strmax;
        strmax.append(QTime::fromMSecsSinceStartOfDay(m_mediaPlayer->duration()).
                      toString("hh:mm:ss"));
        ui->label->setText(str + "/" + strmax);
    });
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this,
            [ = ](qint64 duration) {
        ui->horizontalSlider->setRange(0, duration);
        QString str("0/");
        str.append(QTime::fromMSecsSinceStartOfDay(duration).toString(
                       "hh:mm:ss"));
        ui->label->setText(str);
    });
    connect(m_mediaPlayer, &QMediaPlayer::errorChanged, this,
            [ = ]() {
        if (m_mediaPlayer->error() == QMediaPlayer::NoError) return;

        ui->m_playButton->setEnabled(false);
        const QString errorString = m_mediaPlayer->errorString();
        QString message = "Error: ";

        if (errorString.isEmpty()) message += " #" +
                                              QString::number(int(m_mediaPlayer->
                                                                  error()));
        else message += errorString;
        qDebug() << message;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::seturl(const QUrl& url)
{
    m_mediaPlayer->setSource(url);
    ui->m_playButton->setEnabled(true);

    qDebug() << m_mediaPlayer->activeAudioTrack()
             << m_mediaPlayer->activeVideoTrack()
             << m_mediaPlayer->activeSubtitleTrack();
    qDebug() << m_mediaPlayer->audioTracks().size()
             << m_mediaPlayer->videoTracks().size()
             << m_mediaPlayer->subtitleTracks().size();
    qDebug() << m_mediaPlayer->duration()
             << m_mediaPlayer->position()
             << m_mediaPlayer->playbackState();
}
