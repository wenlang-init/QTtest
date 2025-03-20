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

signals:
private:
    ListModel *listmodel;
};

#endif // LISTW_H
