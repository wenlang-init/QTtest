#ifndef LISTW_H
#define LISTW_H

#include <QWidget>
#include <QQuickWidget>
#include "listw/listmodel.h"

class ListW : public QWidget
{
    Q_OBJECT
public:
    explicit ListW(QWidget *parent = nullptr);
    ~ListW();
signals:
private:
    ListModel *listmodel;
    QQuickWidget *qw;
    QTimer *timer;
    int i=0;
    bool ib = false;
    QString s="0";
};

#endif // LISTW_H
