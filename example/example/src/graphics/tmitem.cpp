#include "tmitem.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>

TMItem::TMItem(qreal x, qreal y, qreal w, qreal h,QGraphicsItem *parent)
    : QGraphicsItem{parent}
{
    rectf.setRect(x,y,w,h);

    //setSelected(true);
    setFlag(QGraphicsItem::ItemIsMovable); // 允许拖动
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache); // 加快渲染性能
    setZValue(-1); // 节点始终堆叠在边缘之上
}

void TMItem::setRect(const QRectF &rect)
{
    rectf= rect;
}

void TMItem::setRect(qreal x, qreal y, qreal w, qreal h)
{
    rectf.setRect(x,y,w,h);
}

QRectF TMItem::boundingRect() const
{
    return rectf;
}

void TMItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    painter->save();

    QPen pen;
    pen.setStyle(Qt::PenStyle::DashLine);
    pen.setColor(QColor(Qt::GlobalColor::green));
    painter->setPen(pen);
    QBrush brush;
    brush.setStyle(Qt::BrushStyle::CrossPattern);
    brush.setColor(QColor(Qt::GlobalColor::red));
    painter->setBrush(brush);
    // painter->drawRoundedRect(-10, -10, 20, 20, 5, 5);
    painter->drawRoundedRect(option->rect,5,5);

    painter->restore();
}
