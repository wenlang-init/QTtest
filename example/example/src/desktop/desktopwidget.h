#ifndef DESKTOPWIDGET_H
#define DESKTOPWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QLabel>
#include <QTimer>
#include "bottom_icon.h"

class CustomBtn;

class DeskTopWidget : public QWidget {
    Q_OBJECT

public:

    explicit DeskTopWidget(QWidget *parent = NULL);

    // 每页个数,必须为行数的整数倍
    void set_page_count(int cnt);

    // 每页行数
    void set_row_count(int cnt);
    void additemlist(QStringList imageurl,
                     QStringList name);
    void additem(QString imageurl,
                 QString name);
    void additem(const QPixmap& pixmap,
                 QString        name);
    void setpixmap(const QPixmap& pixmap,
                   int            index);
    void insertat(int     index,
                  QString imageurl,
                  QString name);
    void removeitem(int index);
    int  get_itemcount();

    void set_switch_page(bool state); // 为true时跳到下一页，否则上一页
    void set_switch_row(bool state);  // 为true时跳到下一行，否则上一行
    void set_switch_one(bool state);  // 为true时跳到下一个，否则上一个
    void set_btn_current(int index);
    int  get_current_index();

protected:

    void resizeEvent(QResizeEvent *event);
    bool eventFilter(QObject *watched,
                     QEvent  *event);

private:

    void backward();
    void forward();
    void resize_homewidget();

signals:

    void itemclicked(int index);

public slots:

    void clicked(CustomBtn *btn);
    void abimationfinished();
    void timeout();

private:

    // QLabel *label;
    bottom_icon *btn_icon;
    QWidget *homewidget;
    QPropertyAnimation *propertyAnimation;
    QTimer *timer;
    bool moveFlag; // 移动时不响应点击
    int clicked_index;
    QList<CustomBtn *>itemlist;

    int onepagecnt;   // 每页的个数
    int onerowcnt;    // 每行的个数
    int m_row_cnt;    // 每页行数
    int pagecount;    // 总页数
    int mcurrentpage; // 当前页
};

#endif // DESKTOPWIDGET_H
