#include <QDebug>
#include <QMouseEvent>
#include "desktopwidget.h"
#include "custombtn.h"

// 动画时间
#define ANIMATIONTIME 200

DeskTopWidget::DeskTopWidget(QWidget *parent) : QWidget(parent)
{
    onepagecnt = 9;
    onerowcnt = 3;
    m_row_cnt = 3;
    mcurrentpage = 0;
    pagecount = 0;
    moveFlag = false;

    //    label = new QLabel(this);
    //    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    //    label->setText("0/0");
    btn_icon = new bottom_icon(this);
    btn_icon->set_count(1);
    btn_icon->set_current(0);

    homewidget = new QWidget(this);
    homewidget->installEventFilter(this);

    /* 绑定动画对象 */
    propertyAnimation = new QPropertyAnimation(homewidget, "pos");
    propertyAnimation->setDuration(ANIMATIONTIME); // 动画持续时间
    connect(propertyAnimation, SIGNAL(finished()), this,
            SLOT(abimationfinished()));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));

    //    QStringList imageurl;
    //    QStringList name;
    //    for(int i=0;i<20;i++){
    //        imageurl.append(QString(":/resource/home/1.png"));
    //        name.append(QString("aaa%1").arg(i));
    //    }
    //    additemlist(imageurl,name);
    //    this->resize(240,320);
}

void DeskTopWidget::set_page_count(int cnt)
{
    onepagecnt = cnt;
    onerowcnt = onepagecnt / m_row_cnt;
    this->repaint();
}

void DeskTopWidget::set_row_count(int cnt)
{
    m_row_cnt = cnt;
    onerowcnt = onepagecnt / m_row_cnt;
    this->repaint();
}

void DeskTopWidget::additemlist(QStringList imageurl, QStringList name)
{
    if (imageurl.count() != name.count()) {
        return;
    }

    for (int i = 0; i < imageurl.count(); i++) {
        CustomBtn *btn = new CustomBtn(homewidget, imageurl.at(i), name.at(i));

        // CustomBtn *btn = new CustomBtn(homewidget);
        // btn->setimage(imageurl.at(i));
        // btn->settext(name.at(i));
        connect(btn, SIGNAL(clicked(CustomBtn*)), this,
                SLOT(clicked(CustomBtn*)));
        itemlist.append(btn);
    }
    resize_homewidget();
    set_btn_current(0);
}

void DeskTopWidget::additem(QString imageurl, QString name)
{
    CustomBtn *btn = new CustomBtn(homewidget, imageurl, name);

    connect(btn, SIGNAL(clicked(CustomBtn*)), this, SLOT(clicked(CustomBtn*)));
    itemlist.append(btn);

    resize_homewidget();

    set_btn_current(0);
}

void DeskTopWidget::additem(const QPixmap& pixmap, QString name)
{
    CustomBtn *btn = new CustomBtn(homewidget, "", name);

    btn->setpixmap(pixmap);

    connect(btn, SIGNAL(clicked(CustomBtn*)), this, SLOT(clicked(CustomBtn*)));
    itemlist.append(btn);

    resize_homewidget();

    set_btn_current(0);
}

void DeskTopWidget::setpixmap(const QPixmap& pixmap, int index)
{
    if ((index < 0) || (itemlist.size() <= index)) return;

    itemlist[index]->setpixmap(pixmap);
}

void DeskTopWidget::insertat(int index, QString imageurl, QString name)
{
    if (index < 0) {
        index = 0;
    }

    if (index > itemlist.count()) {
        index = itemlist.count();
    }
    CustomBtn *btn = new CustomBtn(homewidget, imageurl, name);
    connect(btn, SIGNAL(clicked(CustomBtn*)), this, SLOT(clicked(CustomBtn*)));
    itemlist.insert(index, btn);

    resize_homewidget();

    set_btn_current(0);
}

void DeskTopWidget::removeitem(int index)
{
    if ((index < 0) || (index >= itemlist.count())) return;

    for (int i = 0; i < itemlist.count(); i++) {
        if (i == index) {
            disconnect(itemlist.at(i));
            itemlist.at(i)->deleteLater();
            itemlist.removeAt(i);
            break;
        }
    }

    resize_homewidget();

    set_btn_current(0);
}

int DeskTopWidget::get_itemcount()
{
    return itemlist.count();
}

void DeskTopWidget::set_switch_page(bool state)
{
    int currentindex = get_current_index();

    if (currentindex < 0) return;

    int currentpage = (currentindex + 1) / onepagecnt +
                      ((currentindex + 1) % onepagecnt ? 1 : 0);
    int allpage = get_itemcount() / onepagecnt +
                  (get_itemcount() % onepagecnt ? 1 : 0);

    if (state == true) {
        if (currentpage + 1 > allpage) {
            currentindex = 0;
        } else {
            currentindex = currentpage * onepagecnt;
        }
    } else {
        if (currentpage - 1 <= 0) {
            currentindex = (allpage - 1) * onepagecnt;
        } else {
            currentindex = (currentpage - 2) * onepagecnt;
        }
    }
    set_btn_current(currentindex);
}

void DeskTopWidget::set_switch_row(bool state)
{
    int currentindex = get_current_index();

    if (currentindex < 0) return;

    if (state == true) {
        if (currentindex + onerowcnt >= get_itemcount()) {
            currentindex = currentindex % onerowcnt;
        } else {
            currentindex += onerowcnt;
        }
    } else {
        if (currentindex - onerowcnt < 0) {
            currentindex = (get_itemcount() / onerowcnt) * onerowcnt +
                           (currentindex % onerowcnt);

            if (currentindex > get_itemcount() - 1) {
                currentindex = get_itemcount() - 1;
            }
        } else {
            currentindex -= onerowcnt;
        }
    }
    set_btn_current(currentindex);
}

void DeskTopWidget::set_switch_one(bool state)
{
    int currentindex = get_current_index();

    if (currentindex < 0) return;

    if (state == true) {
        currentindex++;

        if (currentindex >= get_itemcount()) {
            currentindex = 0;
        }
    } else {
        currentindex--;

        if (currentindex < 0) {
            currentindex = get_itemcount() - 1;
        }
    }
    set_btn_current(currentindex);
}

void DeskTopWidget::set_btn_current(int index)
{
    if ((index < 0) || (index >= itemlist.count())) return;

    for (int i = 0; i < itemlist.count(); i++) {
        if (i == index) {
            itemlist.at(i)->set_focus(true);
        } else {
            itemlist.at(i)->set_focus(false);
        }
    }

    if (mcurrentpage != index / onepagecnt) {
        mcurrentpage = index / onepagecnt;

        // if(propertyAnimation->state() == QPropertyAnimation::Stopped){
        propertyAnimation->stop();
        propertyAnimation->setStartValue(QPoint(homewidget->pos().x(), 0));
        int widthValue = this->geometry().width();
        propertyAnimation->setEndValue(QPoint(-widthValue * mcurrentpage, 0));
        propertyAnimation->start();

        // label->setText(QString("%1/%2").arg(mcurrentpage+1).arg(pagecount));
        btn_icon->set_current(mcurrentpage);
        moveFlag = true;

        // }
    }
    homewidget->repaint();
}

int DeskTopWidget::get_current_index()
{
    for (int i = 0; i < itemlist.count(); i++) {
        if (itemlist.at(i)->get_focus() == true) {
            return i;
        }
    }
    return -1;
}

void DeskTopWidget::resizeEvent(QResizeEvent *event)
{
    resize_homewidget();
    QWidget::resizeEvent(event);
}

bool DeskTopWidget::eventFilter(QObject *watched, QEvent *event)
{
    static QPoint point1;
    static QPoint point2;

    if (watched == homewidget) {
        switch (event->type()) {
        case QEvent::MouseButtonPress:
        {
            point1.setX(cursor().pos().x());
            point2.setX(cursor().pos().x());
            break;
        }

        case QEvent::MouseButtonRelease:
        {
            // 还在滚动
            if (propertyAnimation->state() != QPropertyAnimation::Stopped) {
                break;
            }
            int offsetx = cursor().pos().x() - point1.x();
            int position = mcurrentpage * this->geometry().width();

            if (qAbs(offsetx) >= 30) {
                if (offsetx > 0) {
                    // 向后滚动
                    backward();
                } else {
                    // 向前滚动
                    forward();
                }
            } else {
                propertyAnimation->stop();
                propertyAnimation->setStartValue(QPoint(homewidget->pos().x(),
                                                        0));
                propertyAnimation->setEndValue(QPoint(-position, 0));
                propertyAnimation->start();

                // label->setText(QString("%1/%2").arg(mcurrentpage+1).arg(pagecount));
                btn_icon->set_current(mcurrentpage);

                // moveFlag = true;
            }
            break;
        }

        case QEvent::MouseMove:
        {
            if (propertyAnimation->state() != QPropertyAnimation::Stopped) {
                break;
            }
            int moveOffset = cursor().pos().x() - point2.x();
            point2.setX(cursor().pos().x());

            if (qAbs(homewidget->pos().x() + moveOffset) >=
                (pagecount - 1) * this->width()) {
                // break;
            }
            homewidget->move(homewidget->pos().x() + moveOffset, 0);

            // qdebug<<homewidget->pos().x()<<cursor().pos().x()<<moveOffset;
            break;
        }

        default:
        {
            break;
        }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void DeskTopWidget::backward()
{
    if (mcurrentpage == 0) mcurrentpage = 1;  // 使用第一页
    propertyAnimation->stop();
    propertyAnimation->setStartValue(QPoint(homewidget->pos().x(), 0));
    int widthValue = this->geometry().width();
    propertyAnimation->setEndValue(QPoint(-widthValue * (mcurrentpage - 1), 0));

    propertyAnimation->start();
    mcurrentpage--;

    if (mcurrentpage < 0) mcurrentpage = 0;

    // label->setText(QString("%1/%2").arg(mcurrentpage+1).arg(pagecount));
    btn_icon->set_current(mcurrentpage);
    moveFlag = true;
}

void DeskTopWidget::forward()
{
    if (mcurrentpage == pagecount - 1) mcurrentpage -= 1;  // 使用最后一页
    propertyAnimation->stop();
    propertyAnimation->setStartValue(QPoint(homewidget->pos().x(), 0));
    int widthValue = this->geometry().width();
    propertyAnimation->setEndValue(QPoint(-widthValue * (mcurrentpage + 1), 0));

    propertyAnimation->start();
    mcurrentpage++;

    if (mcurrentpage >= pagecount) mcurrentpage = pagecount - 1;

    // label->setText(QString("%1/%2").arg(mcurrentpage+1).arg(pagecount));
    btn_icon->set_current(mcurrentpage);
    moveFlag = true;
}

void DeskTopWidget::resize_homewidget()
{
    int page = itemlist.count() / onepagecnt +
               (itemlist.count() % onepagecnt ? 1 : 0);

    // int row = itemlist.count()/onerowcnt+(itemlist.count()%onerowcnt?1:0);
    int row_space = 20;
    int column_space = 20;
    int currentpage = 0;
    int currentrow = 0;
    int currentcolumn = 0;
    int x_page = column_space;
    int y_page = row_space;

    // int x = column_space;
    int y = row_space;
    int w = (this->width() - row_space * (onerowcnt + 1)) / onerowcnt;
    int h = (this->height() - column_space * (onepagecnt / onerowcnt + 1)) /
            (onepagecnt / onerowcnt);

    homewidget->setGeometry(0, 0, page * this->width(), this->height() - 20);

    // label->setGeometry(0,this->height()-20,this->width(),20);
    btn_icon->setGeometry(0, this->height() - 20, this->width(), 20);

    for (int i = 0; i < itemlist.count(); i++) {
        if ((i > 0) && (i % onepagecnt == 0)) {
            // 下一页
            currentpage++;
            currentcolumn = 0;
            currentrow = 0;
            x_page = column_space + currentpage * this->width();
            y_page = row_space;
            y = y_page;
        } else if ((i > 0) && (i % onerowcnt == 0)) {
            // 下一行
            currentrow++;
            currentcolumn = 0;

            // x = x_page;
            y = y_page + currentrow * (row_space + h);
        }
        int x = x_page + currentcolumn * (column_space + w);
        itemlist.at(i)->setGeometry(x, y, w, h);

        // qdebug<<i<<x<<y<<w<<h<<currentrow<<currentcolumn;
        currentcolumn++;
    }
    mcurrentpage = 0;
    pagecount = page;
    btn_icon->set_count(pagecount);

    // label->setText(QString("%1/%2").arg(mcurrentpage+1).arg(pagecount));
    btn_icon->set_current(mcurrentpage);
}

void DeskTopWidget::clicked(CustomBtn *btn)
{
    for (int i = 0; i < itemlist.count(); i++) {
        if (btn == itemlist.at(i)) {
#if 1

            if (moveFlag == false) {
                set_btn_current(i);
                emit itemclicked(i);
            }
#else // if 1
            clicked_index = i;
            timer->start(10);
#endif // if 1
            break;
        }
    }
}

void DeskTopWidget::abimationfinished()
{
    moveFlag = false;
}

void DeskTopWidget::timeout()
{
    // 在10ms内没有触发移动控制，认为是点击
    if (moveFlag == false) {
        emit itemclicked(clicked_index);
    }
    timer->stop();
}
