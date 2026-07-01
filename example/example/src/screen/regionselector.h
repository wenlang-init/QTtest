#ifndef REGIONSELECTOR_H
#define REGIONSELECTOR_H

#include <QWidget>
#include <QRect>

class RegionSelector : public QWidget {
    Q_OBJECT

public:

    explicit RegionSelector(QWidget *parent = nullptr);
    ~RegionSelector();

    // 开始选择（阻塞模式），返回选择的矩形（屏幕坐标）
    // 如果用户按 ESC 取消，返回空 QRect
    // 得到的矩形是屏幕坐标(逻辑坐标)，需要根据屏幕缩放比例进行转换
    QRect selectRegion();

protected:

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

signals:

    void finished();

private:

    QRect m_selectionRect; // 当前选中的矩形
    QPoint m_startPoint;   // 鼠标按下时的起始点
    bool m_isSelecting;    // 是否正在选择中
    bool m_completed;      // 是否完成选择（用于阻塞循环）
};

#endif // REGIONSELECTOR_H
