#include "freehandselector.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QScreen>
#include <QGuiApplication>
#include <QApplication>
#include <QEventLoop>

FreehandSelector::FreehandSelector(QWidget *parent)
    : QWidget(parent)
    , m_isDrawing(false)
    , m_finished(false)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    // 覆盖所有屏幕
    QRect totalRect;

    for (QScreen *screen : QGuiApplication::screens()) {
        totalRect = totalRect.united(screen->geometry());
    }
    setGeometry(totalRect);

    // 设置鼠标追踪，以便在未按下时也能接收移动事件
    setMouseTracking(true);
}

FreehandSelector::~FreehandSelector() {}

QImage FreehandSelector::selectShapeScreen()
{
    showFullScreen();
    raise();
    activateWindow();

    QEventLoop loop;
    connect(this, &FreehandSelector::destroyed, &loop, &QEventLoop::quit);

    while (!m_finished) {
        QApplication::processEvents(QEventLoop::WaitForMoreEvents);
    }

    close();

    if (m_path.isEmpty()) {
        return QImage();
    }

    QScreen *screen = QGuiApplication::primaryScreen();

    if (!screen) {
        return QImage();
    }

    qreal dpr = screen->devicePixelRatio(); // 获取 DPI 缩放因子

    // 1. 将路径转换为物理像素坐标系
    QPainterPath physPath = m_path * QTransform::fromScale(dpr, dpr);

    // 2. 获取物理边界矩形
    QRect physBounds = physPath.boundingRect().toRect();

    if (physBounds.isEmpty()) {
        return QImage();
    }

    // 3. 抓取整个桌面（物理像素）
    QPixmap fullPixmap = screen->grabWindow(0);

    // 4. 裁剪物理区域
    QPixmap cropped = fullPixmap.copy(physBounds);

    if (cropped.isNull()) {
        return QImage();
    }

    // 5. 创建结果图像（物理尺寸，带 Alpha）
    QImage result(cropped.size(), QImage::Format_ARGB32);
    result.setDevicePixelRatio(dpr);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing);

    // 6. 将物理路径平移至裁剪区域局部坐标
    // physPath.translate(-physBounds.x(), -physBounds.y());
    // 7. 设置裁剪路径
    // painter.setClipPath(physPath);

    QPainterPath localPath = m_path;
    QRect rect = m_path.boundingRect().toRect();
    localPath.translate(-rect.x(), -rect.y());
    painter.setClipPath(localPath);

    painter.drawPixmap(0, 0, cropped);
    painter.end();

    close();
    return result;
}

QPainterPath FreehandSelector::selectShape()
{
    showFullScreen();
    raise();
    activateWindow();

    QEventLoop loop;
    connect(this, &FreehandSelector::destroyed, &loop, &QEventLoop::quit);

    // 等待选择完成或取消
    while (!m_finished) {
        QApplication::processEvents(QEventLoop::WaitForMoreEvents);
    }

    close();
    return m_path;
}

QImage FreehandSelector::fromPath(const QImage      & simage,
                                  const QPainterPath& path,
                                  qreal               dpr)
{
    if (simage.isNull() || path.isEmpty()) return QImage();

    QImage tempImage;

    if (simage.format() != QImage::Format_ARGB32) {
        tempImage = simage.convertToFormat(QImage::Format_ARGB32);
    } else {
        tempImage = simage;
    }

    // tempImage.setDevicePixelRatio(dpr);

    // 创建结果图像（带 Alpha 通道）
    QImage result(tempImage.size(), QImage::Format_ARGB32);
    result.setDevicePixelRatio(dpr);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing);
#if 1

    // 将路径转换到裁剪后的坐标（减去 rect 左上角）
    QPainterPath localPath = path;
    QRect rect = path.boundingRect().toRect();
    localPath.translate(-rect.x(), -rect.y());

    // 设置裁剪区域为路径（使用填充规则）
    painter.setClipPath(localPath);
#else // if 1
    QPainterPath physPath = path * QTransform::fromScale(dpr, dpr);
    QRect physBounds = physPath.boundingRect().toAlignedRect();
    physPath.translate(-physBounds.x(), -physBounds.y());
    painter.setClipPath(physPath);
#endif // if 1

    // 绘制截取的图像
    painter.drawImage(0, 0, tempImage);

    painter.end();
    return result;
}

void FreehandSelector::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制半透明遮罩（路径外部变暗）
    painter.setBrush(QColor(0, 0, 0, 100));
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());

    // 如果正在绘制，在遮罩上“挖”出路径内部（仅用于视觉预览）
    if (!m_path.isEmpty()) {
        // 为了显示路径内部明亮，我们在遮罩上使用 CompositionMode
        // 先绘制一个透明区域？更简单：直接绘制路径轮廓和半透明填充
        painter.setBrush(QColor(0, 0, 0, 50)); // 半透明填充
        painter.setPen(QPen(Qt::red, 2));
        painter.drawPath(m_path);

        // 如果路径已闭合，填充内部为更亮颜色
        if (m_isClosed) {
            painter.setBrush(QColor(255, 255, 255, 30));
            painter.setPen(QPen(Qt::green, 2));
            painter.drawPath(m_path);
        }
    }
}

void FreehandSelector::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_startPoint = event->globalPosition().toPoint();
        m_path = QPainterPath();
        m_path.moveTo(m_startPoint);
        m_isDrawing = true;
        m_finished = false;
        update();
    }
}

void FreehandSelector::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDrawing) {
        QPoint current = event->globalPosition().toPoint();

        // 添加直线段到当前点
        m_path.lineTo(current);
        update();
    }
}

void FreehandSelector::mouseReleaseEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton) && m_isDrawing) {
        m_isDrawing = false;

        // 闭合路径（连接终点到起点）
        m_path.closeSubpath();
        m_isClosed = true; // 记录状态
        m_finished = true;
        update();

        // 退出事件循环（在 selectShape 中会检测到 m_finished 为 true）
    }
}

void FreehandSelector::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        m_path = QPainterPath();
        m_finished = true;
        close();
    }
}
