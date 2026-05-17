#include "picturedelegate.h"
#include <QPainter>

// 标题栏的尺寸样式
const unsigned int LABEL_HEIGHT = 20;
const unsigned int LABEL_COLOR = 0x303030;
const unsigned int LABEL_ALPHA = 200;
const unsigned int LABEL_TEXT_COLOR = 0xffffff;
const unsigned int HIGHLIGHT_ALPHA = 100;

// 图片的尺寸样式
// const unsigned int PIXMAP_WIDTH = 200;
// const unsigned int PIXMAP_HEIGHT = 200;
PictureDelegate::PictureDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{}

void PictureDelegate::paint(QPainter                   *painter,
                            const QStyleOptionViewItem& option,
                            const QModelIndex         & index) const
{
    painter->save();

    // 绘制对应的图片
    QPixmap pixmap =
        index.model()->data(index, Qt::DecorationRole).value<QPixmap>();
    painter->drawPixmap(option.rect.x(),
                        option.rect.y(),
                        pixmap.width(),  // PIXMAP_WIDTH,
                        pixmap.height(), // PIXMAP_HEIGHT,
                        pixmap);

    // painter->fillRect(option.rect, QColor(0, 0, 0, 0));

    // 绘制图片的标题栏显示图片名称
    QRect bannerRect = QRect(option.rect.x(),
                             option.rect.y(),
                             pixmap.width(), // PIXMAP_WIDTH,
                             LABEL_HEIGHT);
    QColor bannerColor = QColor(LABEL_COLOR);
    bannerColor.setAlpha(LABEL_ALPHA);
    painter->fillRect(bannerRect, bannerColor);

    // 绘制标题文字
    QString filename = index.model()->data(index, Qt::DisplayRole).toString();
    painter->setPen(LABEL_TEXT_COLOR);
    painter->drawText(bannerRect, Qt::AlignCenter, filename);

    // 设置元素被选中之后的颜色
    if (option.state.testFlag(QStyle::State_Selected)) {
        QColor selectedColor = option.palette.highlight().color();
        selectedColor.setAlpha(HIGHLIGHT_ALPHA);
        painter->fillRect(option.rect, selectedColor);
    }
    painter->restore();
}

QSize PictureDelegate::sizeHint(const QStyleOptionViewItem& option,
                                const QModelIndex         & index) const
{
    Q_UNUSED(option)
    QPixmap pixmap =
        index.model()->data(index, Qt::DecorationRole).value<QPixmap>();

    // option.rect.size();
    return QSize(pixmap.width(), pixmap.height());

    // return QSize(PIXMAP_WIDTH, PIXMAP_HEIGHT);
}
