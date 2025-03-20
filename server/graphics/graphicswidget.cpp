#include "graphicswidget.h"
#include <QBoxLayout>
#include <QPushButton>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsItem>
#include <QDebug>
#include <QPalette>
#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__

GraphicsWidget::GraphicsWidget(QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout *vlayout = new QVBoxLayout(this);

    //新建场景
    TMSecne* secne = new TMSecne(this);
    //创建图形项
    //QGraphicsRectItem* rectItem = new QGraphicsRectItem(0,0,100,100);
    //secne->setBackgroundBrush(QPixmap(":/images/Background.png"));
    //将图形项添加到场景中
    //secne->addItem(rectItem);
    TMItem *item = new TMItem(0,0,100,100);
    secne->addItem(item);
    TMItem *item1 = new TMItem(100,100,200,200);
    secne->addItem(item1);
    // QGraphicsRectItem *item2 = new QGraphicsRectItem(300,0,100,100);
    // item2->setFlag(QGraphicsItem::ItemIsMovable);
    // item2->setFlag(QGraphicsItem::ItemIsSelectable);
    // item2->setFlag(QGraphicsItem::ItemIsFocusable);
    // item2->setFlag(QGraphicsItem::ItemClipsToShape);
    // secne->addItem(item2);


    //创建视图
    TMView *view = new TMView(this);//QGraphicsView(this);
    view->setScene(secne);
    //设置场景的前景色
    view->setForegroundBrush(QColor(18,111,192,51));
    //设置场景的背景色
    //view->setBackgroundBrush(QPixmap(":/images/Background.png"));
    //设置场景与视图对齐
    view->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    // 获取当前的缩放因子
    //float currentScaleFactor = view->transform().m11();


    // QPushButton *btn = new QPushButton(this);
    // btn->setText("adsssssssssssssss");
    // btn->setMinimumSize(100,30);
    // vlayout->addWidget(btn);
    vlayout->addWidget(view);
    vlayout->setContentsMargins(0,0,0,0);
}
