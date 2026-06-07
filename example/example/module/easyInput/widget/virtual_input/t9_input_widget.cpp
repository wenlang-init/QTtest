#include "t9_input_widget.h"
#include "../basewidget/slipwidget.h"
#include "../basewidget/mtablewidget.h"
#include <QPushButton>
#include <QPainter>
#include <QMouseEvent>

#include <QDebug>

T9_Input_Widget::T9_Input_Widget(QWidget *parent) : QWidget(parent)
{
    show_more = false;

    background_color = QColor(255, 255, 255, 100);

    this->setMinimumSize(240, 100);

    init_widget();

    set_show(0);
}

T9_Input_Widget::~T9_Input_Widget()
{}

void T9_Input_Widget::set_pinyin_list(QStringList& strlist)
{
    show_more = false;
    mlistwidget[0]->set_listtext(strlist);

    if (mlistwidget[0]->count() > 0) {
        mlistwidget[0]->setCurrentRow(0);
    }
    set_show(0);
}

void T9_Input_Widget::set_chinese_list(QList<QStringList>& strlist)
{
    outchinese = strlist;
    show_more = false;

    if (strlist.count() > 0) {
        mlistwidget[1]->set_listtext(strlist[0]);

        if (mlistwidget[1]->count() > 0) {
            mlistwidget[1]->setCurrentRow(0);
        }
        mtablewidget[0]->reset_row_listtext(strlist[0]);

        if (mtablewidget[0]->columnCount() > 0) {
            mtablewidget[0]->setCurrentCell(0, 0);
        }
    }
    set_show(0);
}

void T9_Input_Widget::reset()
{
    show_more = false;
    mlistwidget[0]->clear_list();
    mlistwidget[1]->clear_list();
    mtablewidget[0]->clear_list();
}

QString T9_Input_Widget::current_text()
{
    QString str;

    str.clear();
    QTableWidgetItem *item = mtablewidget[0]->currentItem();

    if (item != NULL) {
        // emit clicked_chinese(item->text());
        str = item->text();
    }
    return str;
}

bool T9_Input_Widget::set_pretext()
{
    if (mtablewidget[0]->rowCount() < 1) return false;

    if (mtablewidget[0]->columnCount() < 1) return false;

    int column = mtablewidget[0]->currentColumn();
    column--;

    if ((column < 0) || (column >= mtablewidget[0]->columnCount())) {
        column = mtablewidget[0]->columnCount() - 1;
    }
    mtablewidget[0]->setCurrentCell(0, column);
    this->repaint();
    return true;
}

bool T9_Input_Widget::set_nexttext()
{
    if (mtablewidget[0]->rowCount() < 1) return false;

    if (mtablewidget[0]->columnCount() < 1) return false;

    int column = mtablewidget[0]->currentColumn();
    column++;

    if ((column < 0) || (column >= mtablewidget[0]->columnCount())) {
        column = 0;
    }
    mtablewidget[0]->setCurrentCell(0, column);
    this->repaint();
    return true;
}

bool T9_Input_Widget::set_prepinyin()
{
    int row =  mlistwidget[0]->currentRow();

    row--;

    if ((row < 0) || (row >= mlistwidget[0]->count())) {
        row = mlistwidget[0]->count() - 1;
    }
    mlistwidget[0]->setCurrentRow(row);

    // 点击第row个
    if (row < outchinese.count()) {
        mlistwidget[1]->set_listtext(outchinese[row]);
        mtablewidget[0]->reset_row_listtext(outchinese[row]);

        if (mlistwidget[1]->count() > 0) {
            mlistwidget[1]->setCurrentRow(0);
        }

        if (mtablewidget[0]->columnCount() > 0) {
            mtablewidget[0]->setCurrentCell(0, 0);
        }
        this->repaint();
    }

    return true;
}

bool T9_Input_Widget::set_nextpinyin()
{
    int row = mlistwidget[0]->currentRow();

    row++;

    if ((row < 0) || (row >= mlistwidget[0]->count())) {
        row = 0;
    }
    mlistwidget[0]->setCurrentRow(row);

    // 点击第row个
    if (row < outchinese.count()) {
        mlistwidget[1]->set_listtext(outchinese[row]);
        mtablewidget[0]->reset_row_listtext(outchinese[row]);

        if (mlistwidget[1]->count() > 0) {
            mlistwidget[1]->setCurrentRow(0);
        }

        if (mtablewidget[0]->columnCount() > 0) {
            mtablewidget[0]->setCurrentCell(0, 0);
        }
        this->repaint();
    }

    return true;
}

void T9_Input_Widget::init_widget()
{
    QPalette pl;

    pl.setBrush(QPalette::Base, QBrush(QColor(255, 255, 255, 100)));
    QFont font("Microsoft Yahei", 8, QFont::Normal);

    // 按键
    for (unsigned int i = 0; i < (sizeof(pushbutton) / sizeof(pushbutton[0]));
         i++) {
        pushbutton[i] = new QPushButton(this);
        pushbutton[i]->installEventFilter(this);

        // 按钮透明
        pushbutton[i]->setFlat(true);
        pushbutton[i]->setFont(font);

        // pushbutton[i]->setPalette(pl);
        // 背景色 圆角
        // pushbutton[i]->setStyleSheet("QPushButton{background-color:rgba(255,255,255,255);border-radius:
        // 10px;}");
        // 字颜色
        // pushbutton[i]->setStyleSheet("QPushButton{color:rgba(0,0,0,255);}");
        // pushbutton[i]->setFont(QFont("宋体",12));
    }
    pushbutton[0]->setIcon(QIcon("easyInput/image/hid.png"));
    pushbutton[1]->setText("1");
    pushbutton[2]->setText("ABC");
    pushbutton[3]->setText("DEF");
    pushbutton[4]->setText("GHI");
    pushbutton[5]->setText("JKL");
    pushbutton[6]->setText("MNO");
    pushbutton[7]->setText("PQRS");
    pushbutton[8]->setText("TUV");
    pushbutton[9]->setText("WXYZ");
    pushbutton[10]->setIcon(QIcon("easyInput/image/delete_white.png"));
    pushbutton[11]->setText(QString::fromUtf8("重输"));
    pushbutton[12]->setIcon(QIcon("easyInput/image/enter.png"));

    // 拼音列表,更多汉字列表
    for (unsigned int i = 0; i < (sizeof(mlistwidget) / sizeof(mlistwidget[0]));
         i++) {
        mlistwidget[i] = new SlipWidget(this);

        // mlistwidget[i]->setStyleSheet("QListWidget{outline:0px;}");
        connect(mlistwidget[i], SIGNAL(itemClicked(QListWidgetItem*)), this,
                SLOT(itemClicked(QListWidgetItem*)));
        mlistwidget[i]->setPalette(pl);
        mlistwidget[i]->setFont(font);
    }
    mlistwidget[1]->hide();

    // 汉字列表
    for (unsigned int i = 0; i < (sizeof(mtablewidget) / sizeof(mtablewidget[0]));
         i++) {
        mtablewidget[i] = new MTableWidget(this);

        // mtablewidget[i] ->setStyleSheet("QTableWidget{outline:0px;}");
        // mtablewidget[i]->setRowCount(1);
        mtablewidget[i]->set_move_state(1);
        connect(mtablewidget[i], SIGNAL(cellClicked(int,int)), this,
                SLOT(cellClicked(int,int)));
        mtablewidget[i]->setPalette(pl);
        mtablewidget[i]->setFont(font);
    }
}

void T9_Input_Widget::set_show(bool state)
{
    if (state == 0) {
        mlistwidget[0]->show();
        mlistwidget[1]->hide();
        mtablewidget[0]->show();

        for (unsigned int i = 1; i < (sizeof(pushbutton) / sizeof(pushbutton[0]));
             i++) {
            pushbutton[i]->show();
        }
    } else {
        mlistwidget[0]->show();
        mlistwidget[1]->show();
        mtablewidget[0]->hide();

        for (unsigned int i = 1; i < (sizeof(pushbutton) / sizeof(pushbutton[0]));
             i++) {
            pushbutton[i]->hide();
        }
    }
}

void T9_Input_Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(background_color);
    painter.drawRect(rect());
    QWidget::paintEvent(event);
}

void T9_Input_Widget::resizeEvent(QResizeEvent *event)
{
    int h = 30;
    int w = this->width() / 5;

    mtablewidget[0]->setGeometry(0, 0, this->width() - w, h);
    pushbutton[0]->setGeometry(this->width() - w, 0, w, h);

    mlistwidget[1]->setGeometry(w, h, this->width() - w, this->height() - h);
    mlistwidget[0]->setGeometry(0, h, w, this->height() - h);

    int btn_h = (this->height() - h) / 3;
    pushbutton[1]->setGeometry(w, h, w, btn_h);
    pushbutton[2]->setGeometry(w * 2, h, w, btn_h);
    pushbutton[3]->setGeometry(w * 3, h, w, btn_h);
    pushbutton[4]->setGeometry(w, h + btn_h, w, btn_h);
    pushbutton[5]->setGeometry(w * 2, h + btn_h, w, btn_h);
    pushbutton[6]->setGeometry(w * 3, h + btn_h, w, btn_h);
    pushbutton[7]->setGeometry(w, h + btn_h * 2, w, btn_h);
    pushbutton[8]->setGeometry(w * 2, h + btn_h * 2, w, btn_h);
    pushbutton[9]->setGeometry(w * 3, h + btn_h * 2, w, btn_h);
    pushbutton[10]->setGeometry(w * 4, h, w, btn_h);
    pushbutton[11]->setGeometry(w * 4, h + btn_h, w, btn_h);
    pushbutton[12]->setGeometry(w * 4, h + btn_h * 2, w, btn_h);

    QWidget::resizeEvent(event);
}

bool T9_Input_Widget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

        if (mouseEvent->buttons() & Qt::LeftButton) {
            for (unsigned int i = 0;
                 i < (sizeof(pushbutton) / sizeof(pushbutton[0])); i++) {
                if (obj == pushbutton[i]) {
                    if (KEY_DEFINE_T9_MORE == i) {
                        show_more = !show_more;
                        set_show(show_more);
                    } else if (KEY_DEFINE_T9_DEL ==
                               i) {} else if (KEY_DEFINE_T9_RELOSE == i) {
                        mtablewidget[0]->clear_list();
                        mlistwidget[0]->clear_list();
                        mlistwidget[1]->clear_list();
                    } else if (KEY_DEFINE_T9_ENTER == i) {} else {}
                    emit clicked_key(i);
                    break;
                }
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}

void T9_Input_Widget::itemClicked(QListWidgetItem *item)
{
    if (item == NULL) return;

    QListWidget *listwidget = qobject_cast<QListWidget *>(sender());

    if (listwidget == mlistwidget[0]) {
        // 拼音列表
        int i = 0;

        for (i = 0; i < listwidget->count(); i++) {
            if (item == listwidget->item(i)) {
                break;
            }
        }

        if ((i >= 0) && (i < listwidget->count())) {
            // 点击第i个
            if (i < outchinese.count()) {
                mlistwidget[1]->set_listtext(outchinese[i]);
                mtablewidget[0]->reset_row_listtext(outchinese[i]);
                this->repaint();
            }
        }
    } else {
        // 更多汉字列表
        emit clicked_chinese(item->text());
    }
}

void T9_Input_Widget::cellClicked(int row, int column)
{
    QTableWidgetItem *item = mtablewidget[0]->item(row, column);

    if (item != NULL) {
        emit clicked_chinese(item->text());
    }
}
