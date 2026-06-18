#ifndef CUSTOMPLOTITEM_H
#define CUSTOMPLOTITEM_H

#include <QQuickPaintedItem>
#include "XxwCustomPlot.h"

#define USEDXXWPLOT
#ifdef USEDXXWPLOT
using CustomPlot = XxwCustomPlot;
#else // ifdef USEDXXWPLOT
using CustomPlot = QCustomPlot;
#endif // ifdef USEDXXWPLOT

class CustomPlotItem : public QQuickPaintedItem {
    Q_OBJECT

    // QML_ELEMENT

public:

    CustomPlotItem(QQuickItem *parent = nullptr);
    virtual ~CustomPlotItem();

    void              paint(QPainter *painter) override;

    Q_INVOKABLE void  initCustomPlot();
    Q_INVOKABLE  void setShowTracer(bool isShow) {
#ifdef USEDXXWPLOT
        m_customPlot->showTracer(isShow);
        m_customPlot->replot();
#endif // ifdef USEDXXWPLOT
    }

    Q_INVOKABLE  void setLabel(const QString& xStr, const QString& yStr)
    {
        m_customPlot->xAxis->setLabel(xStr);
        m_customPlot->yAxis->setLabel(yStr);
    }

    Q_INVOKABLE void setXRang(double lower, double upper)
    {
        m_customPlot->xAxis->setRange(lower, upper);
    }

    Q_INVOKABLE void setYRang(double lower, double upper)
    {
        m_customPlot->yAxis->setRange(lower, upper);
    }

    Q_INVOKABLE  void setLegendShow(bool isShow)
    {
        m_customPlot->legend->setVisible(isShow);
    }

    Q_INVOKABLE  void addGraph(QString name,
                               QString color = "#ff00ff00",
                               QString backgroundColor = "#00ffffff") {
        m_customPlot->addGraph();
        QPen   pen(color);
        QColor cl(backgroundColor);
        QBrush brush(cl);
        m_customPlot->graph(m_customPlot->graphCount() - 1)->setPen(pen);
        m_customPlot->graph(m_customPlot->graphCount() - 1)->setBrush(brush);
        m_customPlot->graph(m_customPlot->graphCount() - 1)->setName(name);

        m_countVector.append(0);
    }

    Q_INVOKABLE void clearData(int index) {
        if (index < m_customPlot->graphCount()) {
            m_customPlot->graph(index)->data().data()->clear();
            m_countVector[index] = 0;
        }
        m_customPlot->replot();
    }

    Q_INVOKABLE void clearAllData() {
        for (int i = 0; i < m_customPlot->graphCount(); i++) {
            m_customPlot->graph(i)->data().data()->clear();
            m_countVector[i] = 0;
        }
        m_customPlot->replot();
    }

    Q_INVOKABLE void addData(QVector<double>& data, int index) {
        if (index < m_customPlot->graphCount()) {
            if (m_countVector[index] > m_maxcount) {
                m_countVector[index] = 0;
                clearData(index);
            }

            for (int i = 0; i < data.size(); i++) {
                m_customPlot->graph(index)->
                addData(m_countVector[index]++, data[i]);
            }
        }
    }

    Q_INVOKABLE void setMaxCount(int maxcount) {
        m_maxcount = maxcount;
    }

    // 显示区间内
    Q_INVOKABLE void setXShow(qint64 left, qint64 right) {
        m_customPlot->xAxis->setRange(left, right);
        m_customPlot->replot();
    }

    Q_INVOKABLE void setYShow(qint64 bottom, qint64 top) {
        m_customPlot->yAxis->setRange(bottom, top);
        m_customPlot->replot();
    }

    // 显示第index条曲线的后count个点
    Q_INVOKABLE void setShowLastLen(qint64 count, int index) {
        if ((index < m_customPlot->graphCount()) && (count > 0)) {
            qint64 left, right;
            right = m_countVector[index];
            left = right - count + 1;
            m_customPlot->xAxis->setRange(left, right);
            m_customPlot->replot();
        }
    }

    Q_INVOKABLE void refresh() {
        m_customPlot->replot();
    }

    Q_INVOKABLE QVariant getOBJQV() {
        // return QVariant::fromValue(m_customPlot);
        return QVariant::fromValue(this);
    }

    CustomPlot* getCustomPlot() {
        return m_customPlot;
    }

protected:

    virtual void mousePressEvent(QMouseEvent *event) override;

    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    virtual void mouseMoveEvent(QMouseEvent *event) override;

    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

    virtual void wheelEvent(QWheelEvent *event) override;

    // virtual bool childMouseEventFilter(QQuickItem *item,
    //                                    QEvent     *event) override;

private:

    CustomPlot *m_customPlot;
    QVector<qint64>m_countVector;
    qint64 m_maxcount = 10000000;

signals:
};

#endif // CUSTOMPLOTITEM_H
