#ifndef TMITEM_H
#define TMITEM_H

#include <QGraphicsItem>

class TMItem : public QGraphicsItem
{
public:
    explicit TMItem(qreal x, qreal y, qreal w, qreal h,QGraphicsItem *parent = nullptr);

    void setRect(const QRectF &rect);
    inline void setRect(qreal x, qreal y, qreal w, qreal h);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget) override;

signals:
private:
    QRectF rectf;
};

#endif // TMITEM_H
