#include "messagedelegate.h"
#include <QPainter>
#include <QDebug>
#include <QDateTime>
#include "messagemodel.h"

#define qdebug qDebug().noquote() << "[" << __FILE__ << ":" << __LINE__ << "] " << \
        __FUNCTION__ << "() "

MessageDelegate::MessageDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{}

void MessageDelegate::paint(QPainter                   *painter,
                            const QStyleOptionViewItem& option,
                            const QModelIndex         & index) const
{
    painter->save();

    bool isSend = index.model()->data(index,
                                      MessageModel::Roles::isSend).value<bool>();
    unsigned long long timenumberms = index.model()->data(index,
                                                          MessageModel::Roles::messageTime)
                                      .
                                      value<unsigned long long>();
    QDateTime datetime = QDateTime::fromMSecsSinceEpoch(timenumberms);
    QString   messageData = index.model()->data(index,
                                                MessageModel::Roles::messageData).
                                      value<QString>();

    messageData = datetime.toString("yyyy-MM-dd hh:mm:ss.zzz") + ":" +
                  messageData;

    int maxwidth = option.rect.width() * 4 / 5.0;

    QSize size = getQStringSize(messageData, option.font, maxwidth);
    QSize icosize(20, 20);

    int textWidth = size.width();
    int textHeight = size.height();

    if (textHeight < icosize.height()) textHeight = icosize.height();


    int startx = option.rect.x();

    if (isSend) {
        startx = option.rect.x() + option.rect.width() - icosize.width();
    }

    int textx = startx + icosize.width() + 5;

    if (isSend) {
        textx = option.rect.width() - (size.width() + icosize.width() + 5);
    }

    // 绘制头像
    QString imagename =
        index.model()->data(index,
                            MessageModel::Roles::headPortrait).value<QString>();
    QPixmap pixmap = QPixmap(imagename).scaled(20, 20);
    painter->drawPixmap(startx,
                        option.rect.y(),
                        pixmap.width(),
                        pixmap.height(),
                        pixmap);

    // 绘制文字消息
    painter->setPen(0xffffff);
    QRect textrect(textx, option.rect.y(), textWidth, textHeight);

    // painter->drawText(textrect, Qt::TextWordWrap, messageData);
    QTextOption option1;
    option1.setWrapMode(QTextOption::WrapAnywhere);
    painter->drawText(textrect, messageData, option1);

    // 设置元素被选中之后的颜色
    if (option.state.testFlag(QStyle::State_Selected)) {
        QColor selectedColor = option.palette.highlight().color();
        selectedColor.setAlpha(100);
        painter->fillRect(textrect, selectedColor);
    }
    painter->restore();
}

QSize MessageDelegate::sizeHint(const QStyleOptionViewItem& option,
                                const QModelIndex         & index) const
{
    QString messageData = index.model()->data(index,
                                              MessageModel::Roles::messageData).
                                      value<QString>();
    unsigned long long timenumberms = index.model()->data(index,
                                                          MessageModel::Roles::messageTime)
                                      .
                                      value<unsigned long long>();
    QDateTime datetime = QDateTime::fromMSecsSinceEpoch(timenumberms);

    messageData = datetime.toString("yyyy-MM-dd hh:mm:ss.zzz") + ":" +
                  messageData;
    int maxwidth = option.rect.width() * 4 / 5.0;

    if (option.rect.size().width() <= 0) {
        return QStyledItemDelegate::sizeHint(option, index);
    }
    QSize size = getQStringSize(messageData, option.font, maxwidth);

    int textWidth = option.rect.width();
    int textHeight = size.height();

    if (textHeight < 20) textHeight = 20;
    return QSize(textWidth, textHeight);
}

QSize MessageDelegate::getQStringSize(const QString& str,
                                      const QFont  & font,
                                      int            maxWidth) const
{
    QFontMetrics fm(font);
    int textWidth = fm.horizontalAdvance(str); // 字符串的像素宽度
    int textHeight = fm.height();              // 字符串的像素高度

    if (maxWidth <= 0) {
        return QSize(textWidth, textHeight);
    }

    if (textWidth > maxWidth) {
        int tmp = textWidth;
        textWidth = maxWidth;
        int stepheight = textHeight + 1;

        while (tmp - maxWidth > 0) {
            textHeight += stepheight;
            tmp -= maxWidth;
        }
    }
    return QSize(textWidth, textHeight);
}
