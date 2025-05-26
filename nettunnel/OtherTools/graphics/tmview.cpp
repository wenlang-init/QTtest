#include "tmview.h"
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QDebug>
#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__

TMView::TMView(QWidget *parent)
    : QGraphicsView{parent}
{
    setDragMode(QGraphicsView::RubberBandDrag);//可拖拽
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setCacheMode(QGraphicsView::CacheBackground);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
    // QTimer *refreshTimer = new QTimer(this);
    // connect(refreshTimer, &QTimer::timeout, [=](){this->viewport()->update();});
    // refreshTimer->setInterval(100);
    // refreshTimer->start();

    pixmapbackground.load(":/images/Background.png");
}

void TMView::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect)
    //return QGraphicsView::drawBackground(painter,rect);
    //painter->drawPixmap(rect, pixmapbackground,QRect());
    painter->drawPixmap(pixmapbackground.rect(), pixmapbackground,QRect());
}

void TMView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Left){
        for(int i=0;i<scene()->selectedItems().size();i++){
            scene()->selectedItems()[i]->moveBy(-1,0);
        }
    } else if(event->key() == Qt::Key_Right){
        for(int i=0;i<scene()->selectedItems().size();i++){
            scene()->selectedItems()[i]->moveBy(1,0);
        }
    } else if(event->key() == Qt::Key_Down){
        for(int i=0;i<scene()->selectedItems().size();i++){
            scene()->selectedItems()[i]->moveBy(0,1);
        }
    } else if(event->key() == Qt::Key_Up){
        for(int i=0;i<scene()->selectedItems().size();i++){
            scene()->selectedItems()[i]->moveBy(0,-1);
        }
    }
    return QGraphicsView::keyPressEvent(event);
}

void TMView::wheelEvent(QWheelEvent *e)
{
    QList<QGraphicsItem*>items = scene()->selectedItems();
    for(int i=0;i<items.size();i++){
        items[i]->setScale(e->angleDelta().ry() > 0 ? items[i]->scale()*1.1 : items[i]->scale()*0.9);
    }

    {
        // 获取当前鼠标相对于view的位置;
        QPointF cursorPoint = e->position();
        // 获取当前鼠标相对于scene的位置;
        QPointF scenePos = this->mapToScene(QPoint(cursorPoint.x(), cursorPoint.y()));
        qdebug<<cursorPoint<<scenePos<<transform().m11()<<transform().m12()
               <<viewport()->width()<<viewport()->height();
    }

    return QGraphicsView::wheelEvent(e);
}

void TMView::resizeEvent(QResizeEvent *resizeEvt)
{
    //return QGraphicsView::resizeEvent(resizeEvt);
    //视图所显示的场景区域
    setSceneRect(pixmapbackground.rect());
    // 缩放视图矩阵并滚动滚动条，以确保场景矩形矩形适合viewport
    fitInView(pixmapbackground.rect(), Qt::AspectRatioMode::KeepAspectRatio); // 等比缩放
    //fitInView(pixmapbackground.rect(), Qt::AspectRatioMode::IgnoreAspectRatio); // 完全填充
    //fitInView(pixmapbackground.rect(), Qt::AspectRatioMode::KeepAspectRatioByExpanding); // 等比填充

    return QGraphicsView::resizeEvent(resizeEvt);
}
