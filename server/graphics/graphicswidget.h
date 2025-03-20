#ifndef GRAPHICSWIDGET_H
#define GRAPHICSWIDGET_H

#include <QWidget>
#include "tmview.h"
#include "tmsecne.h"
#include "tmitem.h"

class GraphicsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphicsWidget(QWidget *parent = nullptr);

signals:
};

#endif // GRAPHICSWIDGET_H
