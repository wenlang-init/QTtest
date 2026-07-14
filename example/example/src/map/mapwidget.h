#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>

class MapWidget : public QWidget {
    Q_OBJECT

public:

    Q_INVOKABLE explicit MapWidget(QWidget *parent = nullptr);

signals:
};

#endif // MAPWIDGET_H
