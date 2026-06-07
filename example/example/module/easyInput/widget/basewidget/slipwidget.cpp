#include "slipwidget.h"
#include <QMouseEvent>
#include <QScrollBar>
#include <QDebug>

SlipWidget::SlipWidget(QWidget *parent,int w,int h) :
    QListWidget(parent),
    ti_width(w),
    ti_height(h),
    press(false),
    move_state(0),
    move_end(0)
{
    // 透明
//    QPalette pl;
//    pl.setBrush(QPalette::Base,QBrush(QColor(255,255,255,0)));
//    this->setPalette(pl);

    setAttribute(Qt::WA_TranslucentBackground,true);
    this->viewport()->installEventFilter(this);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    //this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    //this->setViewMode(QListView::IconMode);
    //this->setFlow(QListView::TopToBottom);
    //this->adjustSize();
}

SlipWidget::~SlipWidget()
{

}

void SlipWidget::set_listtext(QStringList &strlist)
{
    //this->addItems(strlist);
    this->clear();
    for(int i=0;i<strlist.count();i++){
        QListWidgetItem *item = new QListWidgetItem;
        //item->setSizeHint(QSize(width(),40));
        item->setSizeHint(QSize(ti_width,ti_height));
        item->setText(strlist.at(i));
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignCenter);
        this->addItem(item);
    }
}

void SlipWidget::set_move_state(int value)
{
    move_state = value;
}

void SlipWidget::clear_list()
{
    this->clear();
}

bool SlipWidget::eventFilter(QObject *watch, QEvent *evn)
{
    QMouseEvent *event = static_cast<QMouseEvent*>(evn);
    if(event->type() == QEvent::MouseButtonPress){
        //if(event->buttons() & Qt::LeftButton);
        lastposy = event->globalY();
        press = true;
    } else if(event->type() == QEvent::MouseButtonRelease) {
        press = false;
        if(move_end == 2){
            move_end = 0;
            return true; // 不向后传递该信号
        }
    } else if(event->type() == QEvent::MouseMove) { // 移动
        if(press == true){
            move_end = 1;
            int positiony = event->globalY();
            int move_cnt = abs(positiony - lastposy);
            if(move_state==0){
                if(this->verticalScrollBar() != NULL){
                    int ind = this->verticalScrollBar()->value();
                    if(positiony < lastposy){ // 下移
                        ind += move_cnt;
                        if(ind > this->verticalScrollBar()->maximum()){
                            ind = this->verticalScrollBar()->maximum();
                        }
                        this->verticalScrollBar()->setValue(ind);
                    } else {
                        ind -= move_cnt;
                        if(ind < 0){
                            ind = 0;
                        }
                        this->verticalScrollBar()->setValue(ind);
                    }
                    lastposy = positiony;
                }
                move_end = 2;
                return true;
            } else if(move_state==1){
                move_end = 2;
                return true;
            }
        }
    }
    //qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"xxxxx";
    return QWidget::eventFilter(watch,evn);
}

void SlipWidget::resizeEvent(QResizeEvent *e)
{
    //ti_width = this->width();
    //ti_height = this->height();
    QWidget::resizeEvent(e);
}
