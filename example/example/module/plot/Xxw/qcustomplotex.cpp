#include "qcustomplotex.h"

QCustomPlotEx::QCustomPlotEx(QWidget *parent) : QCustomPlot{parent}
{
    m_lineTracer = new MouseTraceLine(this, LineType::Both);
}

void QCustomPlotEx::mouseMoveEvent(QMouseEvent *event)
{
    QCustomPlot::mouseMoveEvent(event);

    if (!m_isShowTrace) return;

    // 当前鼠标位置（像素坐标）
    int x_pos = event->pos().x();
    int y_pos = event->pos().y();

    // 像素坐标转成实际的x,y轴的坐标
    double x_val = this->xAxis->pixelToCoord(x_pos);
    double y_val = this->yAxis->pixelToCoord(y_pos);

    m_lineTracer->updatePosition(x_val, y_val);
    this->replot();
}
