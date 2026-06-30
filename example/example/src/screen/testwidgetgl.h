#ifndef TESTWIDGETGL_H
#define TESTWIDGETGL_H

#include <QWidget>
#include "glimagewidget.h"
#include "glimagewindow.h"
#include <QDateTime>
#include <QImage>
#include "src/public/funchelper.h"

class ffmpegScreen;
class ContinuousScreenCapture;
class TestWidgetGL : public QWidget {
    Q_OBJECT

public:

    explicit TestWidgetGL(QWidget *parent = nullptr);
    ~TestWidgetGL();

signals:

    void loadImage(const QImage& image);

protected:

    void setImageInfo(QImage& image,
                      bool    showsy = true);
    void paintEvent(QPaintEvent *event) override;

private:

    GLImageWidget *m_glImageWidget = nullptr;
    GLImageWindow *m_glImageWindow = nullptr;
    ffmpegScreen *m_ffmscreen = nullptr;
    ContinuousScreenCapture *m_capturer = nullptr;

    qint64 m_allcount = 0, m_count = 0, m_fps = 0;

    QDateTime m_datetime = QDateTime::currentDateTime();
    QList<FuncHelper::WindowInfo>m_list;
};

#endif // TESTWIDGETGL_H
