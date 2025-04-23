#include "XxwCustomPlot.h"

XxwCustomPlot::XxwCustomPlot(QWidget *parent)
    :QCustomPlot(parent)
    ,m_isShowTracer(false)
    ,m_xTracer(Q_NULLPTR)
    ,m_yTracer(Q_NULLPTR)
    ,m_dataTracers(QList<XxwTracer *>())
    ,m_lineTracer(Q_NULLPTR)
{
}

void XxwCustomPlot::mouseMoveEvent(QMouseEvent *event)
{
    QCustomPlot::mouseMoveEvent(event);

    if(m_isShowTracer)
    {
        //当前鼠标位置（像素坐标）
        int x_pos = event->pos().x();
        int y_pos = event->pos().y();

        //像素坐标转成实际的x,y轴的坐标
        double x_val = this->xAxis->pixelToCoord(x_pos);
        double y_val = this->yAxis->pixelToCoord(y_pos);

        if(Q_NULLPTR == m_xTracer)
            m_xTracer = new XxwTracer(this, XxwTracer::XAxisTracer);//x轴
        m_xTracer->updatePosition(x_val, y_val);

        if(Q_NULLPTR == m_yTracer)
            m_yTracer = new XxwTracer(this, XxwTracer::YAxisTracer);//y轴
        m_yTracer->updatePosition(x_val, y_val);

        int nTracerCount = m_dataTracers.count();
        int nGraphCount = graphCount();
        if(nTracerCount < nGraphCount)
        {
            for(int i = nTracerCount; i < nGraphCount; ++i)
            {
                XxwTracer *tracer = new XxwTracer(this, XxwTracer::DataTracer);
                m_dataTracers.append(tracer);
            }
        }
        else if(nTracerCount > nGraphCount)
        {
            for(int i = nGraphCount; i < nTracerCount; ++i)
            {
                XxwTracer *tracer = m_dataTracers[i];
                if(tracer)
                {
                    tracer->setVisible(false);
                }
            }
        }
        for (int i = 0; i < nGraphCount; ++i)
        {
            if(graph(i)->dataCount() < 1)continue;
            XxwTracer *tracer = m_dataTracers[i];
            if(!tracer)
                tracer = new XxwTracer(this, XxwTracer::DataTracer);
            tracer->setVisible(true);
            tracer->setPen(this->graph(i)->pen());
            tracer->setBrush(Qt::NoBrush);
            tracer->setLabelPen(this->graph(i)->pen());
#if 1
            int index_left = this->graph(i)->findBegin(x_val); //左边最近的一个key值索引
            int index_right = this->graph(i)->findEnd(x_val); //右边最近的一个key值索引
            double dif_left = fabs(graph(i)->data()->at(index_left)->key - x_val);
            double dif_right = fabs(graph(i)->data()->at(index_right)->key - x_val);
            int iPointIdx = ((dif_left < dif_right) ? index_left : index_right);
            double x = graph(i)->data()->at(iPointIdx)->key;
            double y = graph(i)->data()->at(iPointIdx)->value;
            tracer->updatePosition(x, y);
            // 2个像素点内最近的x
            // double dRatioX = xAxis->axisRect()->width() / (xAxis->range().upper - xAxis->range().lower); //求得X轴像素比
            // double dx = fabs(x - x_val);
            // if(dx * dRatioX <= 2){
            //     tracer->updatePosition(x, y);
            // }
#else
            auto iter = this->graph(i)->data()->findBegin(x_val);
            double value = iter->mainValue();
//            double value = this->graph(i)->data()->findBegin(x_val)->value;
            tracer->updatePosition(x_val, value);
#endif
        }

        if(Q_NULLPTR == m_lineTracer)
            m_lineTracer = new XxwTraceLine(this,XxwTraceLine::Both);//直线
        m_lineTracer->updatePosition(x_val, y_val);

        this->replot();//曲线重绘
    }
}
