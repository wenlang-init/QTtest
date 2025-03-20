#ifndef TMSECNE_H
#define TMSECNE_H

#include <QGraphicsScene>

class TMSecne : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit TMSecne(QObject *parent = nullptr);
protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;
signals:
};

#endif // TMSECNE_H
