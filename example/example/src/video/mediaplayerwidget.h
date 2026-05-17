#ifndef MEDIAPLAYERWIDGET_H
#define MEDIAPLAYERWIDGET_H

#include <QWidget>
class QMediaPlayer;
class QPushButton;
class QVideoWidget;
class popWidget;
class QTimeEdit;
class mediaPlayerWidget : public QWidget {
    Q_OBJECT

public:

    explicit mediaPlayerWidget(QWidget *parent = nullptr);
    ~mediaPlayerWidget();
    void                  setUrl(QUrl url);
    void                  setFullScreen(bool fullscreen);
    void                  setShowM(bool isshow);
    bool                  isShowM();
    QMap<QString, QString>getMetaData();

protected:

    void init();

    bool eventFilter(QObject *watched,
                     QEvent  *event) override;

signals:

private:

    QMediaPlayer *m_mediaPlayer;
    QPushButton *btn;
    QTimeEdit *timeEdit;
    QPushButton *btnjump;
    QPushButton *btnperv;
    QPushButton *btnnext;

    QVideoWidget *videowidget;
    QWidget *widget;
    popWidget *pop;
    QMap<QString, QString>m_metaData;
};

#endif // MEDIAPLAYERWIDGET_H
