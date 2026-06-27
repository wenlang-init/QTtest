#ifndef MAPGRAPHICSVIEW3_H
#define MAPGRAPHICSVIEW3_H

#include "graphicsitemgroup.h"
#include "src/map/public/mapStruct.h"
#include <QGraphicsView>

class MapGraphicsView3 : public QGraphicsView {
    Q_OBJECT

public:

    explicit MapGraphicsView3(QWidget *parent = nullptr);
    ~MapGraphicsView3() override;

    void setRect(int level);
    void drawImg(const ImageInfo& info);
    void clear();

signals:

    void updateImage(const ImageInfo& info); // 添加瓦片图
    void showRect(QRect rect);
    void mousePos(QPoint pos);

protected:

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:

    void getShowRect(); // 获取显示范围

private:

    QGraphicsScene *m_scene = nullptr;
    int m_level = 5;                                // 当前显示瓦片等级
    bool m_moveView = false;                        // 鼠标移动显示视图
    QPointF m_pos;
    QPointF m_scenePos;
    QHash<quint16, GraphicsItemGroup *>m_itemGroup; // 瓦片图元组
};

#endif                                              // MAPGRAPHICSVIEW_H
