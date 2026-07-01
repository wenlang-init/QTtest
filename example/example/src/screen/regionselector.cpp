#include "regionselector.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QScreen>
#include <QGuiApplication>
#include <QEventLoop>

RegionSelector::RegionSelector(QWidget *parent)
    : QWidget(parent)
    , m_isSelecting(false)
    , m_completed(false)
{
    // 设置为全屏、无边框、置顶、透明背景
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    // 获取所有屏幕的总区域
    QRect totalRect;

    for (QScreen *screen : QGuiApplication::screens()) {
        totalRect = totalRect.united(screen->geometry());
    }
    setGeometry(totalRect);
}

RegionSelector::~RegionSelector() {}

QRect RegionSelector::selectRegion()
{
    showFullScreen(); // 或者 show() 已在构造函数中全屏
    raise();
    activateWindow();

    QEventLoop loop;

    // 当选择完成或取消时退出循环
    connect(this, &RegionSelector::finished, &loop, &QEventLoop::quit);

    // finished 信号由我们自定义，在 mouseReleaseEvent 或 keyPressEvent 中发射
    loop.exec();
    close();
    return m_selectionRect;
}

void RegionSelector::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    // 绘制半透明遮罩
    painter.setBrush(QColor(0, 0, 0, 100));
    painter.setPen(Qt::NoPen);

    // 绘制整个窗口，但排除选择矩形区域
    QRect fullRect = rect();

    if (!m_selectionRect.isNull()) {
        // 计算需要绘制的四个区域（上、下、左、右）
        QRegion region(fullRect);
        region = region.subtracted(QRegion(m_selectionRect));
        painter.setClipRegion(region);
    }
    painter.drawRect(fullRect);

    // 绘制选择矩形边框
    if (!m_selectionRect.isNull()) {
        painter.setClipRect(fullRect);
        painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(m_selectionRect);
    }
}

void RegionSelector::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_startPoint = event->globalPosition().toPoint();
        m_selectionRect = QRect(m_startPoint, QSize(0, 0));
        m_isSelecting = true;
        m_completed = false;
    }
}

void RegionSelector::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isSelecting) {
        QPoint currentPos = event->globalPosition().toPoint();
        m_selectionRect = QRect(m_startPoint, currentPos).normalized();
        update();
    }
}

void RegionSelector::mouseReleaseEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton) && m_isSelecting) {
        m_isSelecting = false;
        m_completed = true;

        // 如果矩形太小，视为取消
        if ((m_selectionRect.width() < 5) || (m_selectionRect.height() < 5)) {
            m_selectionRect = QRect();
        }

        // 退出事件循环
        // 因为 selectRegion 中有循环，这里只需标记，循环会退出
        emit finished();
    }
}

void RegionSelector::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        m_selectionRect = QRect();
        m_completed = true;
        close();
    }
}
