#ifndef PICTUREDELEGATE_H
#define PICTUREDELEGATE_H

#include <QStyledItemDelegate>
#include <QMouseEvent>
class PictureDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:

    PictureDelegate(QObject *parent = 0);

    // 代理的绘制事件
    void paint(QPainter                   *painter,
               const QStyleOptionViewItem& option,
               const QModelIndex         & index) const override;

    // 代理的尺寸
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex         & index) const override;

protected:
};

#endif // PICTUREDELEGATE_H
