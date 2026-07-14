#ifndef LISTW_H
#define LISTW_H

#include <QWidget>
#include <QQuickWidget>
#include "listmodel.h"

class ListW : public QWidget {
    Q_OBJECT

public:

    Q_INVOKABLE explicit ListW(QWidget *parent = nullptr);
    ~ListW();

signals:

private:

    ListModel *listmodel;
    QQuickWidget *qw;
    QTimer *timer;
    int i = 0;
    bool ib = false;
    QString s = "0";
};

#endif // LISTW_H
