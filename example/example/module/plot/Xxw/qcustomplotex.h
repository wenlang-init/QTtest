#ifndef QCUSTOMPLOTEX_H
#define QCUSTOMPLOTEX_H

#include "qcustomplot.h"
#include "mousetraceline.h"
class QCustomPlotEx : public QCustomPlot {
    Q_OBJECT

public:

    explicit QCustomPlotEx(QWidget *parent = nullptr);
    void setShowTrace(bool isShow) {
        m_isShowTrace = isShow;
    }

protected:

    virtual void mouseMoveEvent(QMouseEvent *event);

private:

    MouseTraceLine *m_lineTracer; // 直线
    bool m_isShowTrace = true;
};

#endif // QCUSTOMPLOTEX_H
