#ifndef MESSAGEDELEGATE_H
#define MESSAGEDELEGATE_H

#include <QStyledItemDelegate>
#include <QMouseEvent>

class MessageDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:

    explicit MessageDelegate(QObject *parent = nullptr);

    // 代理的绘制事件
    void paint(QPainter                   *painter,
               const QStyleOptionViewItem& option,
               const QModelIndex         & index) const override;

    // 代理的尺寸
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex         & index) const override;

    QSize getQStringSize(const QString& str,
                         const QFont  & font,
                         int            maxWidth) const;

signals:
};

#endif // MESSAGEDELEGATE_H
