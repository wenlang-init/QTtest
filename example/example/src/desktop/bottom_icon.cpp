#include "bottom_icon.h"
#include <QPainter>
#include <QPainterPath>
#include <QDebug>

#define qdebug qDebug()<<__FILE__<<__LINE__<<__FUNCTION__

bottom_icon::bottom_icon(QWidget *parent) : QWidget(parent)
{
    m_count = 1;
    m_index = 0;
}

void bottom_icon::set_count(int count)
{
    m_count = count;
    if(m_count < 1)m_count=1;
    this->repaint();
}

void bottom_icon::set_current(int index)
{
    m_index = index;
    if(m_index<0)m_index=0;
    if(m_index>m_count-1)m_index=m_index-1;
    this->repaint();
}

void bottom_icon::paintEvent(QPaintEvent *event)
{
    static int space = 5;
    int y = 3;
    int r = this->height()/2-y*2;
    int x = (this->width() - (m_count*(r*2+space)-space))/2;
//    if(x < 0){
//        return;
//    }
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    painter.setPen(QColor(0,255,255,255)); // 边框色
    for(int i=0;i<m_count;i++){
        if(i == m_index){
            painter.setBrush(QBrush(QColor(0,255,255,255)));
            QPainterPath path;
            path.setFillRule(Qt::WindingFill);
            path.addEllipse(x,y,r*2,r*2);
            //painter.drawArc(x,y,r*2,r*2,0,360*16);
            //painter.drawPath(path);
        } else {
            painter.setBrush(QBrush(QColor(0,255,255,0)));
            //painter.drawArc(x,y,r*2,r*2,0,360*16);
            //painter.drawEllipse(x,y,r*2,r*2);
        }
        painter.drawEllipse(x,y,r*2,r*2);
        x += r*2+space;
    }
    QWidget::paintEvent(event);
}
