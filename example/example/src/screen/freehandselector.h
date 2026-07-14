#ifndef FREEHANDSELECTOR_H
#define FREEHANDSELECTOR_H

#include <QWidget>
#include <QPainterPath>
#include <QPoint>

class FreehandSelector : public QWidget {
    Q_OBJECT

public:

    explicit FreehandSelector(QWidget *parent = nullptr);
    ~FreehandSelector();

    // 开始选择，返回 QImage（包含路径内部像素，外部透明）
    // 如果取消，返回空 QImage
    QImage       selectShapeScreen();
    QPainterPath selectShape();
    static QRect fromPath(const QPainterPath& path) {
        return path.boundingRect().toRect();
    }

    // 从给定的图像和路径中裁剪出路径内部的区域，返回新的 QImage
    // 使用path的边界矩形作为裁剪区域，路径外部像素透明
    // 结合selectShape()使用，simage为path边界矩形对应的屏幕截图，path为选择的路径
    // dpr 为设备像素比率，默认为 1.0
    static QImage fromPath(const QImage      & simage,
                           const QPainterPath& path,
                           qreal               dpr = 1.0);

protected:

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

signals:

    void finished();

private:

    QPainterPath m_path; // 当前绘制的路径
    QPoint m_startPoint; // 起点
    bool m_isDrawing;    // 是否正在绘制中
    bool m_isClosed;     // 记录路径是否已闭合
};

#endif // FREEHANDSELECTOR_H
