#ifndef SCREENWIDGET_H
#define SCREENWIDGET_H

#include <QWidget>
#include <QDateTime>
#include "src/public/funchelper.h"

class screenWidget : public QWidget {
    Q_OBJECT

public:

    explicit screenWidget(QWidget *parent = nullptr);
    ~screenWidget();

protected:

    void keyPressEvent(QKeyEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

    void getScreen(int x = 0,
                   int y = 0,
                   int w = -1,
                   int h = -1,
                   WId window = 0);

public slots:

    void changeWindow(void *hwnd);

signals:

private:

    QPixmap pixmap;
    QDateTime m_datetime = QDateTime::currentDateTime();
    int m_count = 0;
    int m_fps = 0;
    qint64 m_allcount = 0;

    QRect m_rect;
};

class titleWidget : public QWidget {
    Q_OBJECT

signals:

    void changeWindow(void *hwnd);

public:

    explicit titleWidget(QWidget *parent = nullptr);

private:

    QList<FuncHelper::WindowInfo>m_list;
};

class screenWidgetShow : public QWidget {
    Q_OBJECT

public:

    explicit screenWidgetShow(QWidget *parent = nullptr);
};

#endif // SCREENWIDGET_H
