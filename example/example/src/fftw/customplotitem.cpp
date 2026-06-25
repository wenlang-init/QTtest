#include "customplotitem.h"

CustomPlotItem::CustomPlotItem(QQuickItem *parent) :
    QQuickPaintedItem(parent),
    m_customPlot(nullptr)
{
    setFlag(ItemHasContents, true);

    // 设置接受鼠标事件
    setAcceptedMouseButtons(Qt::AllButtons);

    // 启用子控件事件过滤
    setFiltersChildMouseEvents(true);

    m_customPlot = new CustomPlot();
}

CustomPlotItem::~CustomPlotItem()
{
    delete m_customPlot;
    m_customPlot = nullptr;
}

void CustomPlotItem::paint(QPainter *painter)
{
    if (m_customPlot) {
        QPixmap picture(boundingRect().size().toSize());
        QCPPainter qcpPainter(&picture);

        // qcpPainter.begin(&picture);
        m_customPlot->toPainter(&qcpPainter);

        // qcpPainter.end();
        painter->drawPixmap(QPoint(), picture);

        // picture.play(painter);
    }
}

void CustomPlotItem::initCustomPlot()
{
#ifdef USEDXXWPLOT
    m_customPlot->showTracer(true);
#endif // ifdef USEDXXWPLOT

#if (QT_VERSION <= QT_VERSION_CHECK(6, 5, 0))
    m_customPlot->setOpenGl(true);
    qDebug() << m_customPlot->openGl();
#endif // if (QT_VERSION <= QT_VERSION_CHECK(6, 5, 0))

    m_customPlot->xAxis->setVisible(true);
    m_customPlot->xAxis->setTickLabels(true);
    m_customPlot->yAxis->setVisible(true);
    m_customPlot->yAxis->setTickLabels(true);

    m_customPlot->xAxis2->setVisible(false);
    m_customPlot->xAxis2->setTickLabels(false); // 隐藏x轴刻度线
    m_customPlot->yAxis2->setVisible(false);
    m_customPlot->yAxis2->setTickLabels(false);

    // 背景色
    m_customPlot->setBackground(QColor(0, 0, 0, 0));

    m_customPlot->xAxis->setBasePen(QPen(Qt::darkGray, 2)); // x轴主线
    m_customPlot->yAxis->setBasePen(QPen(Qt::darkGray, 2)); // y轴主线
    // 网格线
    m_customPlot->xAxis->grid()->setVisible(false);
    m_customPlot->yAxis->grid()->setVisible(false);

    // 设置多选键
    m_customPlot->setMultiSelectModifier(Qt::KeyboardModifier::ControlModifier);

    // 可拖动、可缩放、轴可选、图例可选、绘图可选,可多选
    m_customPlot->setInteractions(
        QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes

        // | QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iMultiSelect
        );

    // 鼠标滚动缩放倍率，默认0.85
    // customPlot->axisRect()->setRangeZoomFactor(0.5);
    // 设置鼠标滚轮轴方向
    m_customPlot->axisRect()->setRangeZoom(Qt::Vertical | Qt::Horizontal);

    // 默认情况下，图例位于主轴矩形的插入布局中，所以这就是我们如何访问它来改变图例的位置：
    m_customPlot->axisRect()->insetLayout()->setInsetAlignment(0,
                                                               Qt::AlignTop |
                                                               Qt::AlignCenter);

    m_customPlot->legend->setSelectableParts(QCPLegend::spItems);

    // 使左轴和下轴始终将其范围转移到右轴和上轴:
    connect(m_customPlot->xAxis,
            SIGNAL(rangeChanged(QCPRange)),
            m_customPlot->xAxis2,
            SLOT(setRange(QCPRange)));
    connect(m_customPlot->yAxis,
            SIGNAL(rangeChanged(QCPRange)),
            m_customPlot->yAxis2,
            SLOT(setRange(QCPRange)));

    m_customPlot->xAxis->setLabel("x");
    m_customPlot->yAxis->setLabel("y");

    // 显示图例
    m_customPlot->legend->setVisible(true);
    m_customPlot->legend->setBrush(QBrush(QColor(0, 0, 255, 20)));
    m_customPlot->legend->setTextColor(QColor(Qt::lightGray));
    m_customPlot->legend->setBorderPen(Qt::NoPen);          // 无边框
    m_customPlot->legend->setMargins(QMargins(0, 0, 0, 0)); // 文字与边框的距离

    // QVBoxLayout *mainLayout = new QVBoxLayout(this);
    // mainLayout->setContentsMargins(0, 0, 0, 0);
    // mainLayout->addWidget(m_customPlot);
}

void CustomPlotItem::mousePressEvent(QMouseEvent *event)
{
    // 将事件转发给 CustomPlot
    QCoreApplication::sendEvent(m_customPlot, event);

    // event->accept();
    // grabMouse();
    // ungrabMouse();

    // 调用 update() 触发重绘，以显示交互效果
    update(QRect(0, 0, width(), height()));
}

void CustomPlotItem::mouseReleaseEvent(QMouseEvent *event)
{
    // 将事件转发给 CustomPlot
    QCoreApplication::sendEvent(m_customPlot, event);

    // 调用 update() 触发重绘，以显示交互效果
    update(QRect(0, 0, width(), height()));
}

void CustomPlotItem::mouseMoveEvent(QMouseEvent *event)
{
    // 将事件转发给 CustomPlot
    QCoreApplication::sendEvent(m_customPlot, event);

    // 调用 update() 触发重绘，以显示交互效果
    update(QRect(0, 0, width(), height()));
}

void CustomPlotItem::mouseDoubleClickEvent(QMouseEvent *event)
{ // 将事件转发给 CustomPlot
    QCoreApplication::sendEvent(m_customPlot, event);

    // 调用 update() 触发重绘，以显示交互效果
    update(QRect(0, 0, width(), height()));
}

void CustomPlotItem::wheelEvent(QWheelEvent *event)
{ // 将事件转发给 CustomPlot
    QCoreApplication::sendEvent(m_customPlot, event);

    // 调用 update() 触发重绘，以显示交互效果
    update(QRect(0, 0, width(), height()));
}

// bool CustomPlotItem::childMouseEventFilter(QQuickItem *item, QEvent *event)
// {
//     // 可以在这里将事件转发给 CustomPlot
//     if ((event->type() == QEvent::MouseButtonPress) ||
//         (event->type() == QEvent::MouseMove) ||
//         (event->type() == QEvent::MouseButtonRelease)) {
//         QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

//         // 将事件转发给 CustomPlot
//         QCoreApplication::sendEvent(m_customPlot, mouseEvent);

//         // 返回 true 表示事件已被处理，不再继续传递
//         return true;
//     }

//     // 对于其他事件，返回 false 让 Qt 继续默认处理
//     return false;
// }
