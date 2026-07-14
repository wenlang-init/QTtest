#ifndef CUSTOMBTN_H
#define CUSTOMBTN_H

#include "basewidget.h"
#include <QLabel>
#include <QPixmap>

class CustomBtn : public BaseWidget {
    Q_OBJECT

public:

    explicit CustomBtn(QWidget *parent = NULL,
                       QString  img_url = "",
                       QString  text = "button");

    void setimage(QString img_url);
    void setpixmap(const QPixmap& pixmap);
    void settext(QString text);

    void settext_color(const QColor& textColor = Qt::black);
    void settext_fontcolor(const QFont& textfont = QFont("Microsoft Yahei", 10,
                                                         QFont::Normal),
                           const QColor& textColor = Qt::black);

    void set_checked(QColor color);
    void set_hover(QColor color);

    void set_focus(bool state);
    bool get_focus();

protected:

    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:

    QColor color_checked; // 选中
    QColor color_hover;   // 未选中

    QColor color_pr;

    bool status;       // 状态
    bool _status;      // 状态pr

    QString show_text; // 显示文本
    QString img_str;
    QPixmap icon;      // 显示图片

    QLabel *label_image;
    QLabel *label_text;

    QPoint mpoint;

signals:

    void mousePressed();
    void mouseRelease();
    void clicked();
    void clicked(CustomBtn *btn);

public slots:

    void Key_Pass_Slot(int key,
                       int index); // 按键槽函数
};

#endif // CUSTOMBTN_H
