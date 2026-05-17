#ifndef WLAYOUT_H
#define WLAYOUT_H

#include <QWidget>
#include <QQuickWidget>

class wLayout : public QWidget {
    Q_OBJECT

public:

    explicit wLayout(QWidget *parent = nullptr);
    Q_INVOKABLE bool changeFullScreen();

signals:

private:

    QQuickWidget *qw;
};

#endif // WLAYOUT_H
