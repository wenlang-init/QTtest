#ifndef LINECHARTWIDGET_H
#define LINECHARTWIDGET_H

#include <QWidget>
#include <QChart>
#include <QChartView>
#include <QValueAxis>

// #include <QLineSeries>
#include <QSplineSeries>

class ChartView;
class LineChartWidget : public QWidget {
    Q_OBJECT

public:

    explicit LineChartWidget(QWidget *parent = nullptr);
    ~LineChartWidget();

    inline void addData(qreal& x, qreal& y) {
        m_splineSeries->append(x, y);

        // qreal xmin = m_axisX->max() - m_xrang;
        // xmin = xmin < m_axisX->min() ? xmin : m_axisX->min();
        // m_axisX->setRange(xmin, m_axisX->max());
    }

    inline void replace(const QList<QPointF>& points) {
        m_splineSeries->replace(points);

        // m_axisX->setRange(m_axisX->min(), m_axisX->max());
    }

    inline void setRangSize(int len = 1000) {
        m_xrang = len;
    }

    inline QValueAxis* getAxisX() {
        return m_axisX;
    }

    inline QValueAxis* getAxisY() {
        return m_axisY;
    }

    inline QSplineSeries* getLineSeries() {
        return m_splineSeries;
    }

    inline QChart* getChart() {
        return m_chart;
    }

protected:

    void init();

private:

    ChartView *m_chartView;
    QChart *m_chart;

    // QLineSeries *m_lineSerier;
    QSplineSeries *m_splineSeries; // 平滑曲线
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;

    int m_xrang = 1000;
};


class ChartView : public QChartView {
    Q_OBJECT

public:

    ChartView(QChart  *chart,
              QWidget *parent = nullptr);
    ~ChartView();

    // 保存坐标区域，用于复位
    void saveAxisRange();

protected:

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:

    QPoint m_lastPoint;
    bool m_isPress;
    bool m_ctrlPress;
    bool m_alreadySaveRange;
    double m_xMin, m_xMax, m_yMin, m_yMax;
    QList<QGraphicsSimpleTextItem *>m_coordItem;
};

#endif // LINECHARTWIDGET_H
