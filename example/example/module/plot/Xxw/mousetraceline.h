#ifndef MOUSETRACELINE_H
#define MOUSETRACELINE_H
#include "qcustomplot.h"
#include <QObject>

enum LineType
{
    VerticalLine,   //垂直线
    HorizonLine,    //水平线
    Both            //同时显示水平和垂直线
};

class MouseTraceLine : public QObject
{
    Q_OBJECT
public:
    explicit MouseTraceLine(QCustomPlot *_plot, LineType _type = VerticalLine,QObject *parent = nullptr);
    ~MouseTraceLine();

    void init_line();
    void updatePosition(double xValue, double yValue);
protected:
    bool m_visible;//是否可见
    LineType m_type;//类型
    QCustomPlot *m_plot;//图表
    QCPItemStraightLine *m_lineV; //垂直线
    QCPItemStraightLine *m_lineH; //水平线

signals:

public slots:
};

#endif // MOUSETRACELINE_H
