#include "t26_input_widget.h"
#include <QPushButton>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include "../basewidget/slipwidget.h"
#include "../basewidget/mtablewidget.h"

#include <QDebug>

T26_Input_Widget::T26_Input_Widget(QWidget *parent) : QWidget(parent)
{
    background_color = QColor(255, 255, 255, 100);
    show_more = false;
    chinese_mode = true;
    capital_mode = true;

    // QFont font(this->font());
    QFont font("Microsoft Yahei", 8, QFont::Normal);
    font.setPointSize(10);
    QPalette pe(this->palette());

    // pe.setColor(QPalette::ButtonText,QColor(0,0,0,255));
    pe.setBrush(QPalette::Base,       QBrush(QColor(255, 255, 255, 100)));
    pe.setBrush(QPalette::WindowText, QBrush(QColor(0, 0, 0, 255)));
    labelpinyin = new QLabel(this);
    labelpinyin->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    labelpinyin->setPalette(pe);

    for (quint32 i = 0; i < sizeof(pushbutton) / sizeof(pushbutton[0]); i++) {
        pushbutton[i] = new QPushButton(this);
        pushbutton[i]->setFlat(true);
        pushbutton[i]->setFont(font);

        // pushbutton[i]->setPalette(pe);
        // button[i]->setStyleSheet("QPushButton{background-color:rgba(255,255,255,255);border-radius:
        // 10px;}");
        // 字颜色
        // button[i]->setStyleSheet("QPushButton{color:rgba(0,0,0,255);}");
        // button[i]->setFont(QFont("宋体",12));
        pushbutton[i]->installEventFilter(this);
    }
    pushbutton[0]->setText("Q"); pushbutton[1]->setText("W");
    pushbutton[2]->setText("E");
    pushbutton[3]->setText("R"); pushbutton[4]->setText("T");
    pushbutton[5]->setText("Y");
    pushbutton[6]->setText("U"); pushbutton[7]->setText("I");
    pushbutton[8]->setText("O");
    pushbutton[9]->setText("P"); pushbutton[10]->setText("A");
    pushbutton[11]->setText("S");
    pushbutton[12]->setText("D"); pushbutton[13]->setText("F");
    pushbutton[14]->setText("G");
    pushbutton[15]->setText("H"); pushbutton[16]->setText("J");
    pushbutton[17]->setText("K");
    pushbutton[18]->setText("L"); pushbutton[19]->setText("Z");
    pushbutton[20]->setText("X");
    pushbutton[21]->setText("C"); pushbutton[22]->setText("V");
    pushbutton[23]->setText("B");
    pushbutton[24]->setText("N"); pushbutton[25]->setText("M");
    pushbutton[26]->setIcon(QIcon("easyInput/image/hid.png"));
    pushbutton[27]->setIcon(QIcon("easyInput/image/upper_white.png"));
    pushbutton[28]->setIcon(QIcon("easyInput/image/delete_white.png"));
    pushbutton[29]->setIcon(QIcon("easyInput/image/enter.png"));
    pushbutton[30]->setText(QString::fromUtf8("重输"));
    pushbutton[31]->setText(QString::fromUtf8("，"));
    pushbutton[32]->setText(QString::fromUtf8(" "));
    pushbutton[33]->setText(QString::fromUtf8("。"));
    pushbutton[34]->setText(QString::fromUtf8("中"));

    mlistwidget = new SlipWidget(this);
    connect(mlistwidget, SIGNAL(itemClicked(QListWidgetItem*)), this,
            SLOT(itemClicked(QListWidgetItem*)));
    mlistwidget->setPalette(pe);
    mlistwidget->setFont(font);

    mtablewidget = new MTableWidget(this);

    // mtablewidget->setRowCount(1);
    mtablewidget->set_move_state(1);
    connect(mtablewidget, SIGNAL(cellClicked(int,int)), this,
            SLOT(cellClicked(int,int)));
    mtablewidget->setPalette(pe);
    mtablewidget->setFont(font);

    show_widget();
}

void T26_Input_Widget::set_pinyin_str(QString& str)
{
    pinyin = str;

    if (chinese_mode == false) return;

    labelpinyin->setText(pinyin);
}

void T26_Input_Widget::set_chinese_list(QStringList& strlist)
{
    chineselist = strlist;

    if (chinese_mode == false) return;

    mlistwidget->set_listtext(strlist);
    mtablewidget->reset_row_listtext(strlist);

    if (mtablewidget->rowCount() < 1) return;

    mtablewidget->setCurrentCell(0, 0);
}

void T26_Input_Widget::reset()
{
    labelpinyin->clear();
    pinyin.clear();
    chineselist.clear();
    mlistwidget->clear_list();
    mtablewidget->clear_list();
}

QString T26_Input_Widget::current_text()
{
    QString str;
    QTableWidgetItem *item = mtablewidget->currentItem();

    if (item != NULL) {
        str = item->text();
    }
    return str;
}

bool T26_Input_Widget::set_pretext()
{
    if (mtablewidget->rowCount() < 1) return false;

    if (mtablewidget->columnCount() < 1) return false;

    int column = mtablewidget->currentColumn();
    column--;

    if ((column < 0) || (column >= mtablewidget->columnCount())) {
        column = mtablewidget->columnCount() - 1;
    }
    mtablewidget->setCurrentCell(0, column);
    this->repaint();
    return true;
}

bool T26_Input_Widget::set_nexttext()
{
    if (mtablewidget->rowCount() < 1) return false;

    if (mtablewidget->columnCount() < 1) return false;

    int column = mtablewidget->currentColumn();
    column++;

    if ((column < 0) || (column >= mtablewidget->columnCount())) {
        column = 0;
    }
    mtablewidget->setCurrentCell(0, column);
    this->repaint();
    return true;
}

void T26_Input_Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(background_color);
    painter.drawRect(rect());
    QWidget::paintEvent(event);
}

void T26_Input_Widget::resizeEvent(QResizeEvent *event)
{
    int h0 = 20;
    int h = (this->height() - h0) / 5;

    labelpinyin->setGeometry(0, 0, this->width(), h0);

    mlistwidget->setGeometry(0, h0 + h, this->width(), this->height() - h0);
    int w0 = h;
    mtablewidget->setGeometry(0, h0, this->width() - w0, h);
    pushbutton[26]->setGeometry(this->width() - w0, h0, w0, h);

    int width_space = 2;
    int w2 = this->width() / (float)10 - width_space;
    pushbutton[0]->setGeometry(0, mtablewidget->geometry().bottom(), w2, h);
    pushbutton[1]->setGeometry(pushbutton[0]->geometry().right() + width_space,
                               pushbutton[0]->geometry().top(),
                               w2,
                               h);
    pushbutton[2]->setGeometry(pushbutton[1]->geometry().right() + width_space,
                               pushbutton[0]->geometry().top(),
                               w2,
                               h);
    pushbutton[3]->setGeometry(pushbutton[2]->geometry().right() + width_space,
                               pushbutton[0]->geometry().top(),
                               w2,
                               h);
    pushbutton[4]->setGeometry(pushbutton[3]->geometry().right() + width_space,
                               pushbutton[0]->geometry().top(),
                               w2,
                               h);
    pushbutton[5]->setGeometry(pushbutton[4]->geometry().right() + width_space,
                               pushbutton[0]->geometry().top(),
                               w2,
                               h);
    pushbutton[6]->setGeometry(pushbutton[5]->geometry().right() + width_space,
                               pushbutton[0]->geometry().top(),
                               w2,
                               h);
    pushbutton[7]->setGeometry(pushbutton[6]->geometry().right() + width_space,
                               pushbutton[0]->geometry().top(),
                               w2,
                               h);
    pushbutton[8]->setGeometry(pushbutton[7]->geometry().right() + width_space,
                               pushbutton[0]->geometry().top(),
                               w2,
                               h);
    pushbutton[9]->setGeometry(pushbutton[8]->geometry().right() + width_space,
                               pushbutton[0]->geometry().top(),
                               w2,
                               h);

    pushbutton[10]->setGeometry(w2 / 2, pushbutton[0]->geometry().bottom(), w2,
                                h);
    pushbutton[11]->setGeometry(pushbutton[10]->geometry().right() + width_space,
                                pushbutton[10]->geometry().top(),
                                w2,
                                h);
    pushbutton[12]->setGeometry(pushbutton[11]->geometry().right() + width_space,
                                pushbutton[10]->geometry().top(),
                                w2,
                                h);
    pushbutton[13]->setGeometry(pushbutton[12]->geometry().right() + width_space,
                                pushbutton[10]->geometry().top(),
                                w2,
                                h);
    pushbutton[14]->setGeometry(pushbutton[13]->geometry().right() + width_space,
                                pushbutton[10]->geometry().top(),
                                w2,
                                h);
    pushbutton[15]->setGeometry(pushbutton[14]->geometry().right() + width_space,
                                pushbutton[10]->geometry().top(),
                                w2,
                                h);
    pushbutton[16]->setGeometry(pushbutton[15]->geometry().right() + width_space,
                                pushbutton[10]->geometry().top(),
                                w2,
                                h);
    pushbutton[17]->setGeometry(pushbutton[16]->geometry().right() + width_space,
                                pushbutton[10]->geometry().top(),
                                w2,
                                h);
    pushbutton[18]->setGeometry(pushbutton[17]->geometry().right() + width_space,
                                pushbutton[10]->geometry().top(),
                                w2,
                                h);

    int w3 = w2 * 1.5;
    pushbutton[27]->setGeometry(0, pushbutton[10]->geometry().bottom(), w3, h);
    pushbutton[19]->setGeometry(pushbutton[27]->geometry().right() + width_space,
                                pushbutton[27]->geometry().top(),
                                w2,
                                h);
    pushbutton[20]->setGeometry(pushbutton[19]->geometry().right() + width_space,
                                pushbutton[27]->geometry().top(),
                                w2,
                                h);
    pushbutton[21]->setGeometry(pushbutton[20]->geometry().right() + width_space,
                                pushbutton[27]->geometry().top(),
                                w2,
                                h);
    pushbutton[22]->setGeometry(pushbutton[21]->geometry().right() + width_space,
                                pushbutton[27]->geometry().top(),
                                w2,
                                h);
    pushbutton[23]->setGeometry(pushbutton[22]->geometry().right() + width_space,
                                pushbutton[27]->geometry().top(),
                                w2,
                                h);
    pushbutton[24]->setGeometry(pushbutton[23]->geometry().right() + width_space,
                                pushbutton[27]->geometry().top(),
                                w2,
                                h);
    pushbutton[25]->setGeometry(pushbutton[24]->geometry().right() + width_space,
                                pushbutton[27]->geometry().top(),
                                w2,
                                h);
    pushbutton[28]->setGeometry(pushbutton[25]->geometry().right() + width_space,
                                pushbutton[27]->geometry().top(),
                                w3,
                                h);

    int w4 = this->width() / (float)6 - width_space;
    pushbutton[30]->setGeometry(0, pushbutton[27]->geometry().bottom(), w4, h);
    pushbutton[31]->setGeometry(pushbutton[30]->geometry().right() + width_space,
                                pushbutton[30]->geometry().top(),
                                w4,
                                h);
    pushbutton[32]->setGeometry(pushbutton[31]->geometry().right() + width_space,
                                pushbutton[30]->geometry().top(),
                                w4,
                                h);
    pushbutton[33]->setGeometry(pushbutton[32]->geometry().right() + width_space,
                                pushbutton[30]->geometry().top(),
                                w4,
                                h);
    pushbutton[34]->setGeometry(pushbutton[33]->geometry().right() + width_space,
                                pushbutton[30]->geometry().top(),
                                w4,
                                h);
    pushbutton[29]->setGeometry(pushbutton[34]->geometry().right() + width_space,
                                pushbutton[30]->geometry().top(),
                                w4,
                                h);


    QWidget::resizeEvent(event);
}

void T26_Input_Widget::show_widget()
{
    for (quint32 i = 0; i < sizeof(pushbutton) / sizeof(pushbutton[0]); i++) {
        pushbutton[i]->hide();
    }
    pushbutton[26]->show();
    mlistwidget->hide();
    mtablewidget->hide();
    labelpinyin->hide();

    if (show_more == true) {
        mlistwidget->show();
    } else {
        labelpinyin->show();
        mtablewidget->show();

        for (quint32 i = 0; i < sizeof(pushbutton) / sizeof(pushbutton[0]); i++) {
            pushbutton[i]->show();
        }

        if (chinese_mode == true) {
            pushbutton[31]->setText(QString::fromUtf8("，"));
            pushbutton[33]->setText(QString::fromUtf8("。"));
            pushbutton[34]->setText(QString::fromUtf8("中"));
        } else {
            pushbutton[31]->setText(QString::fromUtf8(","));
            pushbutton[32]->setText(QString::fromUtf8(" "));
            pushbutton[33]->setText(QString::fromUtf8("."));
            pushbutton[34]->setText(QString::fromUtf8("英"));
        }

        if ((capital_mode == true) || (chinese_mode == true)) {
            pushbutton[0]->setText("Q"); pushbutton[1]->setText("W");
            pushbutton[2]->setText("E");
            pushbutton[3]->setText("R"); pushbutton[4]->setText("T");
            pushbutton[5]->setText("Y");
            pushbutton[6]->setText("U"); pushbutton[7]->setText("I");
            pushbutton[8]->setText("O");
            pushbutton[9]->setText("P"); pushbutton[10]->setText("A");
            pushbutton[11]->setText("S");
            pushbutton[12]->setText("D"); pushbutton[13]->setText("F");
            pushbutton[14]->setText("G");
            pushbutton[15]->setText("H"); pushbutton[16]->setText("J");
            pushbutton[17]->setText("K");
            pushbutton[18]->setText("L"); pushbutton[19]->setText("Z");
            pushbutton[20]->setText("X");
            pushbutton[21]->setText("C"); pushbutton[22]->setText("V");
            pushbutton[23]->setText("B");
            pushbutton[24]->setText("N"); pushbutton[25]->setText("M");
        } else {
            pushbutton[0]->setText("q"); pushbutton[1]->setText("w");
            pushbutton[2]->setText("e");
            pushbutton[3]->setText("r"); pushbutton[4]->setText("t");
            pushbutton[5]->setText("y");
            pushbutton[6]->setText("u"); pushbutton[7]->setText("i");
            pushbutton[8]->setText("o");
            pushbutton[9]->setText("p"); pushbutton[10]->setText("a");
            pushbutton[11]->setText("s");
            pushbutton[12]->setText("d"); pushbutton[13]->setText("f");
            pushbutton[14]->setText("g");
            pushbutton[15]->setText("h"); pushbutton[16]->setText("j");
            pushbutton[17]->setText("k");
            pushbutton[18]->setText("l"); pushbutton[19]->setText("z");
            pushbutton[20]->setText("x");
            pushbutton[21]->setText("c"); pushbutton[22]->setText("v");
            pushbutton[23]->setText("b");
            pushbutton[24]->setText("n"); pushbutton[25]->setText("m");
        }
    }
}

bool T26_Input_Widget::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

        if (mouseEvent->buttons() & Qt::LeftButton) {
            for (unsigned int i = 0;
                 i < (sizeof(pushbutton) / sizeof(pushbutton[0])); i++) {
                if (watched == pushbutton[i]) {
                    int key = -1;

                    if (i < 26) {
                        if (chinese_mode == false) {
                            emit clicked_chinese(pushbutton[i]->text());
                        } else {
                            char c =
                                pushbutton[i]->text().toUpper().toStdString().
                                c_str()[
                                    0];
                            key = c - 'A' + KEY_DEFINE_T26_A;
                        }
                    } else if (i == 26) {
                        if (chinese_mode == true) {
                            show_more = !show_more;
                            show_widget();
                        }
                    } else if (i == 27) {
                        if (chinese_mode == false) {
                            capital_mode = !capital_mode;
                            show_widget();
                        }
                    } else if (i == 34) {
                        chinese_mode = !chinese_mode;
                        reset();
                        show_widget();
                        key = KEY_DEFINE_T26_RELOSE;
                    } else if (i == 28) {
                        key = KEY_DEFINE_T26_DEL;
                    } else if (i == 29) {
                        key = KEY_DEFINE_T26_ENTER;
                    } else if (i == 30) {
                        key = KEY_DEFINE_T26_RELOSE;
                    } else if ((i == 31) || (i == 32) || (i == 33)) {
                        emit clicked_chinese(pushbutton[i]->text());
                        return QWidget::eventFilter(watched, event);
                    }

                    if (key > 0) {
                        emit clicked_key(key);
                    }
                }
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void T26_Input_Widget::itemClicked(QListWidgetItem *item)
{
    emit clicked_chinese(item->text());
}

void T26_Input_Widget::cellClicked(int row, int column)
{
    QTableWidgetItem *item = mtablewidget->item(row, column);

    if (item != NULL) {
        emit clicked_chinese(item->text());
    }
}
