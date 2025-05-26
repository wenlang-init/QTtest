#ifndef TMVIEW_H
#define TMVIEW_H

#include <QGraphicsView>

class TMView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TMView(QWidget *parent = nullptr);

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void keyPressEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent* e) override;
    void resizeEvent(QResizeEvent* resizeEvt) override;
signals:
private:
    QPixmap pixmapbackground;
};

#endif // TMVIEW_H
