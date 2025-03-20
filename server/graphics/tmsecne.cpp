#include "tmsecne.h"
#include <QPainter>

TMSecne::TMSecne(QObject *parent)
    : QGraphicsScene{parent}
{}

void TMSecne::drawBackground(QPainter *painter, const QRectF &rect)
{
    return QGraphicsScene::drawBackground(painter,rect);
    QPixmap pm(":/images/Background.png");
    painter->drawPixmap(rect, pm,QRect());
}
