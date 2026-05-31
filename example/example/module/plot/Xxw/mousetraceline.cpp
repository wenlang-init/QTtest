#include "mousetraceline.h"

MouseTraceLine::MouseTraceLine(QCustomPlot *_plot, LineType _type,QObject *parent) : QObject(parent),
    m_type(_type),
    m_plot(_plot)
{
    m_lineH = Q_NULLPTR;
    m_lineV = Q_NULLPTR;
    init_line();
}

MouseTraceLine::~MouseTraceLine()
{
    if(m_plot)
    {
        if(m_lineH)
            m_plot->removeItem(m_lineH);
        if(m_lineV)
            m_plot->removeItem(m_lineV);
    }
}

void MouseTraceLine::init_line()
{
    if(m_plot)
    {
        QPen linesPen(Qt::red, 1, Qt::DashLine);
        if(VerticalLine == m_type || Both == m_type)
        {
            m_lineV = new QCPItemStraightLine(m_plot);//垂直线
            m_lineV->setPen(linesPen);
            m_lineV->point1->setCoords(0, 0);
            m_lineV->point2->setCoords(0, 0);
        }

        if(HorizonLine == m_type || Both == m_type)
        {
            m_lineH = new QCPItemStraightLine(m_plot);//水平线
            m_lineH->setPen(linesPen);
            m_lineH->point1->setCoords(0, 0);
            m_lineH->point2->setCoords(0, 0);
        }
    }
}

void MouseTraceLine::updatePosition(double xValue, double yValue)
{
    if(VerticalLine == m_type || Both == m_type)
    {
        if(m_lineV)
        {
            m_lineV->point1->setCoords(xValue, m_plot->yAxis->range().lower);
            m_lineV->point2->setCoords(xValue, m_plot->yAxis->range().upper);
        }
    }

    if(HorizonLine == m_type || Both == m_type)
    {
        if(m_lineH)
        {
            m_lineH->point1->setCoords(m_plot->xAxis->range().lower, yValue);
            m_lineH->point2->setCoords(m_plot->xAxis->range().upper, yValue);
        }
    }
}