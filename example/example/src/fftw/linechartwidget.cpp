#include "linechartwidget.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>
#include <QLegend>
#include <QLegendMarker>

LineChartWidget::LineChartWidget(QWidget *parent) :
    QWidget(parent)
{
    init();
}

LineChartWidget::~LineChartWidget()
{}

void LineChartWidget::init()
{
    m_chart = new QChart();
    m_chartView = new ChartView(m_chart, this);

    #define USEOPENGLWIDGET
#ifdef USEOPENGLWIDGET
    QOpenGLWidget *openglw = new QOpenGLWidget(this);
    m_chartView->setViewport(openglw);

    // openglw->setMouseTracking(true);
#endif // ifdef USEOPENGLWIDGET

    // setMouseTracking(true);
    // m_chartView->setMouseTracking(true);


    m_splineSeries = new QSplineSeries(m_chart);
    m_splineSeries->setUseOpenGL(true);
    qDebug() << m_splineSeries->useOpenGL();
    connect(m_splineSeries, &QSplineSeries::useOpenGLChanged, this, [ = ]() {
        qDebug() << m_splineSeries->useOpenGL();
    });

    // m_splineSeries->setColor(QColor(0, 0, 255, 555));
    m_chart->addSeries(m_splineSeries);

    m_chartView->setChart(m_chart);
    m_chart->legend()->setEnabled(true);

    // m_chart->setPlotAreaBackgroundVisible(true);
    // m_chart->setBackgroundBrush(QBrush(QColor(0, 0, 0, 0)));

    m_axisX = new QValueAxis(this);
    m_axisY = new QValueAxis(this);

    m_axisX->setRange(-60, 60);
    m_axisY->setRange(-2, 2);

    for (int i = -60; i < 61; i++) {
        m_splineSeries->append(i, qCos(i));
    }

    m_chart->addAxis(m_axisX, Qt::AlignmentFlag::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    m_splineSeries->attachAxis(m_axisX);
    m_splineSeries->attachAxis(m_axisY);

    // QHBoxLayout *hboxLayout1 = new QHBoxLayout(openglw);
    // hboxLayout1->addWidget(m_chartView);
    // hboxLayout1->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *hboxLayout = new QHBoxLayout(this);
    hboxLayout->setContentsMargins(0, 0, 0, 0);
    hboxLayout->addWidget(m_chartView);

    // m_chartView->hide();

    // hboxLayout->addStretch();

    // QTimer *timer = new QTimer(this);
    // timer->start(5000);
    // connect(timer, &QTimer::timeout, this, [ = ]() {
    //     QSplineSeries *splineSeries = new QSplineSeries(m_chart);
    //     splineSeries->setUseOpenGL(true);
    //     qDebug() << splineSeries->useOpenGL();
    //     connect(splineSeries, &QSplineSeries::useOpenGLChanged, this, [ = ]()
    // {
    //         qDebug() << splineSeries->useOpenGL();
    //     });

    //     m_chart->addSeries(splineSeries);

    //     QValueAxis *axisX = new QValueAxis(this);

    //     int cnt = rand() % 200;
    //     axisX->setRange(-cnt, cnt);

    //     for (int i = -cnt; i < cnt + 1; i++) {
    //         splineSeries->append(i, qSin(i) * (rand() % 2));
    //     }

    //     m_chart->addAxis(axisX, Qt::AlignmentFlag::AlignBottom);

    //     splineSeries->attachAxis(axisX);
    //     splineSeries->attachAxis(m_axisY);
    // });
}

ChartView::ChartView(QChart *chart, QWidget *parent)
    : QChartView(chart, parent)
{
    m_isPress = false;
    m_ctrlPress = false;
    m_alreadySaveRange = false;

    this->setDragMode(QGraphicsView::ScrollHandDrag);
    this->setMouseTracking(true);
    setCursor(QCursor(Qt::PointingHandCursor)); // 设置鼠标指针为手指形

    setRubberBand(QChartView::NoRubberBand);
}

ChartView::~ChartView()
{}

void ChartView::mousePressEvent(QMouseEvent *event)
{
    if (!chart()) {
        return QChartView::mousePressEvent(event);
    }

    if (event->button() == Qt::LeftButton)
    {
        m_lastPoint = event->pos();

        // qDebug() << "mousePressEvent:" << m_lastPoint;
        m_isPress = true;
    }

    if (event->button() == Qt::RightButton) {
        chart()->zoomReset();
    }
}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (!chart()) {
        return QChartView::mouseMoveEvent(event);
    }

    QList<QAbstractSeries *> series = chart()->series();

    if (series.size() != m_coordItem.size()) {
        for (int i = 0; i < m_coordItem.size(); i++) {
            delete m_coordItem[i];
        }
        m_coordItem.clear();

        for (int i = 0; i < series.size(); i++) {
            QGraphicsSimpleTextItem *coordItem = new QGraphicsSimpleTextItem(
                chart());
            coordItem->setZValue(5);
            coordItem->setPos(10, i * 10 + 10);
            coordItem->show();
            m_coordItem.append(coordItem);
        }
    }

    const QPoint curPos = event->pos();

    for (int i = 0; i < series.size(); i++) {
        QPointF curVal = this->chart()->mapToValue(QPointF(curPos), series[i]);
        QString coordStr = QString("X = %1, Y = %2").arg(curVal.x()).arg(
            curVal.y());
        m_coordItem[i]->setText(coordStr);
    }

    if (m_isPress)
    {
        QPoint offset = curPos - m_lastPoint;
        m_lastPoint = curPos;

        if (!m_alreadySaveRange)
        {
            this->saveAxisRange();
            m_alreadySaveRange = true;
        }
        this->chart()->scroll(-offset.x(), offset.y());
    }
}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (!chart()) {
        return QChartView::mouseReleaseEvent(event);
    }

    // qDebug() << "======mouseReleaseEvent======";
    m_isPress = false;

    if (event->button() == Qt::RightButton)
    {
        if (m_alreadySaveRange)
        {
            QList<QAbstractAxis *> list;
            list = chart()->axes(Qt::Horizontal);
            foreach(auto node, list) {
                node->setRange(m_xMin, m_xMax);
            }
            list = chart()->axes(Qt::Vertical);
            foreach(auto node, list) {
                node->setRange(m_yMin, m_yMax);
            }

            // chart()->axisX()->setRange(m_xMin, m_xMax);
            // chart()->axisY()->setRange(m_yMin, m_yMax);
        }
    }
}

void ChartView::wheelEvent(QWheelEvent *event)
{
    if (!chart()) {
        return QChartView::wheelEvent(event);
    }

    // qDebug() << "wheelEvent:" << event->position();

    if (!m_alreadySaveRange)
    {
        this->saveAxisRange();
        m_alreadySaveRange = true;
    }

    QRectF plotArea = chart()->plotArea(); // 绘图区域（场景坐标）
    // 假设 X 轴在底部，其 Y 坐标就是 plotArea 的底部 Y 值
    qreal xAxisY = plotArea.bottom();

    // 假设 Y 轴在左侧，其 X 坐标就是 plotArea 的左侧 X 值
    qreal yAxisX = plotArea.left();

    int status = 0;

    if (event->position().x() <= yAxisX) {
        // setRubberBand(QChartView::VerticalRubberBand);
        status = 1; // 缩放Y轴
    } else if (event->position().y() >= xAxisY) {
        // setRubberBand(QChartView::HorizontalRubberBand);
        status = 2; // 缩放X轴
    } else {
        // setRubberBand(QChartView::RectangleRubberBand);
    }

    // qDebug() << status << event->position() << yAxisX << xAxisY;

    // const static qreal zoomCnt = 1.1;

    // if (event->angleDelta().y() > 0) {
    //     chart()->zoom(zoomCnt);
    // } else {
    //     chart()->zoom(1 / zoomCnt);
    // }

    // event->accept();

    // 获取 X 轴和 Y 轴
    QValueAxis *axisX = qobject_cast<QValueAxis *>(chart()->axes(
                                                       Qt::Horizontal).first());
    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart()->axes(
                                                       Qt::Vertical).first());

    if (!axisX || !axisY) return;

    double factor = 1.1;

    if (event->angleDelta().y() < 0) factor = 1 / factor;

    // 转换鼠标位置到图表坐标
    QPointF valuePos = chart()->mapToValue(event->position());

    // 计算 X 轴新范围
    qreal xMin = axisX->min();
    qreal xMax = axisX->max();
    qreal xRange = xMax - xMin;
    qreal newXRange = xRange / factor;
    qreal ratioX = (valuePos.x() - xMin) / xRange;
    qreal newXMin = valuePos.x() - ratioX * newXRange;
    qreal newXMax = newXMin + newXRange;

    // 计算 Y 轴新范围
    qreal yMin = axisY->min();
    qreal yMax = axisY->max();
    qreal yRange = yMax - yMin;
    qreal newYRange = yRange / factor;
    qreal ratioY = (valuePos.y() - yMin) / yRange;
    qreal newYMin = valuePos.y() - ratioY * newYRange;
    qreal newYMax = newYMin + newYRange;

    // 应用新范围
    if ((status == 0) || (status == 2)) axisX->setRange(newXMin, newXMax);

    if ((status == 0) || (status == 1)) axisY->setRange(newYMin, newYMax);
}

void ChartView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control)
    {
        m_ctrlPress = true;
    }
}

void ChartView::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control)
    {
        m_ctrlPress = false;
    }
}

void ChartView::saveAxisRange()
{
    QList<QAbstractAxis *> list;


    list = chart()->axes(Qt::Horizontal);

    if (list.size() > 0) {
        QValueAxis *axisX = dynamic_cast<QValueAxis *>(list[0]);

        // QValueAxis *axisX = dynamic_cast<QValueAxis
        // *>(this->chart()->axisX());
        m_xMin = axisX->min();
        m_xMax = axisX->max();
    }

    list = chart()->axes(Qt::Vertical);

    if (list.size() > 0) {
        QValueAxis *axisY = dynamic_cast<QValueAxis *>(list[0]);

        // QValueAxis *axisY = dynamic_cast<QValueAxis
        // *>(this->chart()->axisY());
        m_yMin = axisY->min();
        m_yMax = axisY->max();
    }
}
