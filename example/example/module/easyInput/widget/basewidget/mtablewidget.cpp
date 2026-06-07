#include "mtablewidget.h"
#include <QMouseEvent>
#include <QScrollBar>
#include <QHeaderView>
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>

MTableWidget::MTableWidget(QWidget *parent, int rcnt, int w,
                           int h) : QTableWidget(parent),
    press(false),
    ti_width(w),
    ti_height(h),
    row_strcnt(rcnt),
    move_state(0),
    move_end(0)
{
    // 透明
    //    QPalette pl;
    //    pl.setBrush(QPalette::Base,QBrush(QColor(255,255,255,0)));
    //    this->setPalette(pl);

    this->resize(w, h);
    setAttribute(Qt::WA_TranslucentBackground, true);
    this->viewport()->installEventFilter(this);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    // this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    // 透明
    // QPalette pl;
    // pl.setBrush(QPalette::Base,QBrush(QColor(255,255,255,0)));
    // this->setPalette(pl);

    // this->adjustSize();

    // setRowCount(1);
    // setRowHeight(0,this->height());

    // 表头
    horizontalHeader()->setHidden(true);
    verticalHeader()->setHidden(true);

    // 编辑
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    // item->setFlags(Qt::ItemIsEnabled);
    // 选择模式
    setSelectionMode(QAbstractItemView::SingleSelection); // 单选

    // 在arm上刷新异常
    // setStyleSheet("QTableWidget::item{border:5px solid black;}");

    this->setShowGrid(false); // 无网格线
    // 在每个item下设置线
    // this->setStyleSheet("QTableWidget::item{border:0px solid
    // rgb(0,0,0);border-bottom:1px solid rgb(0,0,0);}");

    // test
    if (0) {
        int xm = 1;
        int ym = 20;
        setRowCount(xm);
        setColumnCount(ym);
        QString qst = QString::number(0);

        for (int i = 0; i < xm; i++) {
            setRowHeight(i, this->height());

            for (int j = 0; j < ym; j++) {
                // setColumnWidth(j,10);
                int row = i;
                int col = j;
                qst += QString::number(j + i * 10);
                QTableWidgetItem *item = new QTableWidgetItem(qst);
                item->setTextAlignment(Qt::AlignCenter);

                // item->setTextColor(QColor(255,255,255,255));
                setItem(row, col, item);
            }
        }
    }
}

MTableWidget::~MTableWidget()
{}

void MTableWidget::set_listtext(QStringList& strlist)
{
    int cnt = strlist.count();
    int rowcnt = cnt / row_strcnt;

    if ((float)rowcnt < (float)cnt / row_strcnt) {
        rowcnt++;
    }
    this->setRowCount(rowcnt);
    this->setColumnCount(row_strcnt);

    for (int i = 0; i < rowCount(); i++) {
        setRowHeight(i, ti_height);

        for (int j = 0; j < columnCount(); j++) {
            setColumnWidth(j, ti_width);

            if (i * columnCount() + j < strlist.count()) {
                int row = i;
                int col = j;
                QTableWidgetItem *item =
                    new QTableWidgetItem(strlist.at(i * columnCount() + j));
                item->setTextAlignment(Qt::AlignHCenter | Qt::AlignCenter);

                // item->setFont(QFont("宋体",18));
                setItem(row, col, item);
            } else {
                break;
            }
        }
    }
}

void MTableWidget::reset_row_listtext(QStringList& strlist)
{
    this->clear();
    this->setRowCount(1);

    // setRowHeight(0,(ti_height-150)/6);
    setRowHeight(0, 31);
    int cnt = strlist.count();
    this->setColumnCount(cnt);

    for (int i = 0; i < cnt; i++) {
        // setColumnWidth(i,ti_width);
        int row = 0;
        int col = i;
        QTableWidgetItem *item = new QTableWidgetItem(strlist.at(i));
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignCenter);

        // item->setSizeHint(QSize(ti_width,ti_height));

        setItem(row, col, item);

        // int size = item->font().pointSize()*2;
        // qDebug()<<"xxxxxxxxxxxxxx"<<size<<this->font().pointSize();
        // int w = QWidget::fontMetrics().width(strlist.at(i));
        QFontMetrics fm(this->font());
        int w = fm.horizontalAdvance(strlist.at(i));

        // int h = fm.ascent()+fm.descent();
        setColumnWidth(i, w + 20);

        // setColumnWidth(i,strlist.at(i).length()*18+10);
    }
}

void MTableWidget::set_move_state(int value)
{
    move_state = value;
}

void MTableWidget::clear_list()
{
    this->clear();
    this->setRowCount(0);
    this->setColumnCount(0);
}

bool MTableWidget::eventFilter(QObject *watch, QEvent *evn)
{
    QMouseEvent *event = static_cast<QMouseEvent *>(evn);

    if (event->type() == QEvent::MouseButtonPress) {
        // if(event->buttons() & Qt::LeftButton);
        lastposx = event->globalX();
        lastposy = event->globalY();
        press = true;
    } else if (event->type() == QEvent::MouseButtonRelease) {
        press = false;

        if (move_end == 2) {
            move_end = 0;
            return true;                             // 不向后传递该信号
        }
    } else if (event->type() == QEvent::MouseMove) { // 移动
        if (press == true) {
            move_end = 1;
            QPoint nowpos = event->globalPos();
            int    move_cnt_x = abs(nowpos.x() - lastposx);
            int    move_cnt_y = abs(nowpos.y() - lastposy);

            if ((move_cnt_y > 0) && ((move_state == 0) || (move_state == 2))) {
                if (this->verticalScrollBar() != NULL) {
                    int ind = this->verticalScrollBar()->value();

                    if (nowpos.y() < lastposy) { // 下移
                        ind += move_cnt_y;

                        if (ind > this->verticalScrollBar()->maximum()) {
                            ind = this->verticalScrollBar()->maximum();
                        }
                        this->verticalScrollBar()->setValue(ind);
                    } else {
                        ind -= move_cnt_y;

                        if (ind < 0) {
                            ind = 0;
                        }
                        this->verticalScrollBar()->setValue(ind);
                    }
                    lastposy = nowpos.y();
                }
            }

            if ((move_cnt_x > 0) && ((move_state == 0) || (move_state == 1))) {
                if (this->horizontalScrollBar() != NULL) {
                    int ind = this->horizontalScrollBar()->value();

                    if (nowpos.x() < lastposx) { // 右移
                        ind += move_cnt_x;

                        if (ind > this->horizontalScrollBar()->maximum()) {
                            ind = this->horizontalScrollBar()->maximum();
                        }
                        this->horizontalScrollBar()->setValue(ind);
                    } else {
                        ind -= move_cnt_x;

                        if (ind < 0) {
                            ind = 0;
                        }
                        this->horizontalScrollBar()->setValue(ind);
                    }
                    lastposx = nowpos.x();
                }
            }

            if ((move_state == 3) || (move_state == 0) ||
                (move_state == 1) || (move_state == 2)) {
                move_end = 2;
                return true;
            }
        }
    }

    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"xxxxx";
    return QWidget::eventFilter(watch, evn);
}

void MTableWidget::paintEvent(QPaintEvent *e)
{
    QTableWidget::paintEvent(e);

    //    QPainter painter(viewport());
    //    for(int i=0;i<columnCount();i++){
    //        int start = horizontalHeader()->sectionViewportPosition(i);
    //        QPoint from = QPoint(start,0);
    //        QPoint to = QPoint(start,height());
    //        painter.drawLine(from,to);
    //        start += horizontalHeader()->sectionSize(i) - 10;
    //        from = QPoint(start,0);
    //        to = QPoint(start,height());
    //        painter.drawLine(from,to);
    //    }
    //    for(int j=0;j<rowCount();j++){
    //        int start = verticalHeader()->sectionViewportPosition(j);
    //        QPoint from = QPoint(0,start);
    //        QPoint to = QPoint(width(),start);
    //        painter.drawLine(from,to);
    //        start += verticalHeader()->sectionSize(j) - 10;
    //        from = QPoint(0,start);
    //        to = QPoint(width(),start);
    //        painter.drawLine(from,to);
    //    }
    //    QTableWidget::paintEvent(e);
}
