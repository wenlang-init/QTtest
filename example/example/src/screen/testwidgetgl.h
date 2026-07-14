#ifndef TESTWIDGETGL_H
#define TESTWIDGETGL_H

#include <QWidget>
#include "glimagewidget.h"
#include "glimagewindow.h"
#include <QDateTime>
#include <QImage>
#include <QPainterPath>
#include "src/public/funchelper.h"

class QLabel;
class QSpinBox;
class QCheckBox;
class ffmpegScreen;
class SaveAV1FromQImage;
class ContinuousScreenCapture;
class TestWidgetGL : public QWidget {
    Q_OBJECT

public:

    Q_INVOKABLE explicit TestWidgetGL(QWidget *parent = nullptr);
    ~TestWidgetGL();

signals:

    void loadImage(const QImage& image);
    void saveImage(const QImage& image);

protected:

    void loaderImageFFmpeg(const QRect& m_rect);
    void loaderImageDXGI(const QRect& m_rect,
                         HWND         hwnd = nullptr,
                         bool         ishwnd = false);
    void setImageInfo(QImage& image,
                      bool    showsy = true);
    void paintEvent(QPaintEvent *event) override;

private:

    void resetSaveObject();

private:

    QLabel *labelfps;
    QSpinBox *spinBoxFps;
    QCheckBox *checkboxshow;
    QPainterPath m_path;
    bool m_isPath = false;

    QImage m_image;
    int m_imageWidth = -1;
    int m_imageHeight = -1;

    // 用于存储多个 SaveAV1FromQImage 对象及其对应的文件名
    QMap<SaveAV1FromQImage *, QString>m_saveAV1Map;

    GLImageWidget *m_glImageWidget = nullptr;
    GLImageWindow *m_glImageWindow = nullptr;
    ffmpegScreen *m_ffmscreen = nullptr;
    SaveAV1FromQImage *m_saveAV1FromQImage = nullptr;
    ContinuousScreenCapture *m_capturer = nullptr;

    qint64 m_allcount = 0, m_count = 0, m_fps = 0;

    QDateTime m_datetime = QDateTime::currentDateTime();
    QList<FuncHelper::WindowInfo>m_list;
};

#endif // TESTWIDGETGL_H
