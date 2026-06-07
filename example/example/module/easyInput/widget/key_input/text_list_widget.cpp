#include "text_list_widget.h"
#include <QLabel>
#include <QKeyEvent>
#include <QPainter>
#include <QDebug>

Text_List_Widget::Text_List_Widget(QWidget *parent) : QWidget(parent)
{
    label = NULL;
    label_cnt = 0;
    m_checked = false;
    current_checked_text = -1;
    line_count = 0;
    line_start_index = NULL;
    current_line = 0;
}

Text_List_Widget::~Text_List_Widget()
{
    if (label != NULL) {
        for (int i = 0; i < label_cnt; i++) {
            label[i]->deleteLater();
        }
        delete[] label;
    }

    if (line_start_index != NULL) {
        delete[] line_start_index;
    }
}

void Text_List_Widget::reset()
{
    if (label != NULL) {
        for (int i = 0; i < label_cnt; i++) {
            label[i]->deleteLater();
        }
        delete[] label;
        label = NULL;
    }

    if (line_start_index != NULL) {
        delete line_start_index;
        line_start_index = NULL;
    }

    label_cnt = 0;
    current_checked_text = -1;
    line_count = 0;
    current_line = 0;
}

void Text_List_Widget::set_text(QStringList& strlist)
{
    if (label != NULL) {
        for (int i = 0; i < label_cnt; i++) {
            label[i]->deleteLater();
        }
        delete[] label;
        label = NULL;
    }

    if (line_start_index != NULL) {
        delete line_start_index;
        line_start_index = NULL;
    }
    m_strlist = strlist;
    label_cnt = m_strlist.size();

    if (label_cnt <= 0) {
        current_checked_text = -1;
        line_count = 0;
        current_line = 0;
        return;
    }

    label = new QLabel *[label_cnt];

    for (int i = 0; i < label_cnt; i++) {
        label[i] = new QLabel(this);
        label[i]->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        label[i]->setText(m_strlist[i]);

        // 设置边框
        // label[i]->setFrameShape(QFrame::Box);
        // label[i]->setStyleSheet("border-width:10px;border-style:solid;border-color:rgb(255,170,0);");
    }

    line_start_index = new int[label_cnt];

    setgeometry();
}

bool Text_List_Widget::is_checked()
{
    return m_checked;
}

void Text_List_Widget::set_check(bool checked)
{
    m_checked = checked;
    this->repaint();
}

void Text_List_Widget::select_next()
{
    if (label_cnt <= 0) return;

    current_checked_text++;

    if (current_checked_text >= label_cnt) {
        current_checked_text = 0;
        current_line = 1;
    } else {
        int next_start;

        if (line_count > current_line) {
            next_start = line_start_index[current_line];

            if (current_checked_text >= next_start) {
                current_line++;
            }
        }
    }
    set_show();
}

void Text_List_Widget::select_prev()
{
    if (label_cnt <= 0) return;

    int index_start = line_start_index[current_line - 1];
    current_checked_text--;

    if (current_checked_text < 0) {
        current_checked_text = label_cnt - 1;
        current_line = line_count;
    } else {
        if (current_checked_text < index_start) {
            current_line--;
        }
    }
    set_show();
}

void Text_List_Widget::select_next_page()
{
    if (label_cnt <= 0) return;

    current_line++;

    if (current_line > line_count) {
        current_line = 1;
    }
    current_checked_text = line_start_index[current_line - 1];
    set_show();
}

void Text_List_Widget::select_prev_page()
{
    if (label_cnt <= 0) return;

    current_line--;

    if (current_line < 1) {
        current_line = line_count;
    }
    current_checked_text = line_start_index[current_line - 1];
    set_show();
}

QString Text_List_Widget::get_current_text()
{
    if (current_checked_text < 0) {
        return QString("");
    }
    return m_strlist.at(current_checked_text);
}

int Text_List_Widget::get_current_index()
{
    return current_checked_text;
}

int Text_List_Widget::get_current_row()
{
    return current_line;
}

int Text_List_Widget::get_row_cnt()
{
    return line_count;
}

QString Text_List_Widget::get_index_text(int index)
{
    QString str;

    if ((current_line > 0) && (current_line <= line_count)) {
        int start = line_start_index[current_line - 1];
        int end;

        if (current_line == line_count) {
            end = label_cnt - 1;
        } else {
            end = line_start_index[current_line] - 1;
        }

        if ((index >= 0) && (index < (end - start + 1))) {
            str = m_strlist.at(start + index);
        }
    }
    return str;
}

void Text_List_Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // QPen pen;
    // pen.setWidth(2);
    if (m_checked == true) {
        // pen.setColor(QColor(0,0,255,255));
        painter.setBrush(QBrush(QColor(128, 255, 128, 255)));
    } else {
        // pen.setColor(QColor(0,0,255,0));
        painter.setBrush(QBrush(QColor(128, 255, 128, 0)));
    }

    // painter.setPen(pen);
    // painter.setBrush(QColor(0,0,255,255));
    painter.drawRect(this->rect());

    QWidget::paintEvent(event);
}

void Text_List_Widget::resizeEvent(QResizeEvent *event)
{
    setgeometry();
    QWidget::resizeEvent(event);
}

void Text_List_Widget::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);
}

void Text_List_Widget::keyReleaseEvent(QKeyEvent *event)
{
    QWidget::keyReleaseEvent(event);
}

void Text_List_Widget::setgeometry()
{
    if (label != NULL) {
        current_checked_text = 0;
        line_count = 1;
        current_line = 1;

        int x = 0, w = 0, m = 0;
        line_start_index[m] = 0;

        for (int i = 0; i < label_cnt; i++) {
            QFontMetrics fm(label[i]->font());

            // w = fm.width(label[i]->text())+4;
            w = fm.horizontalAdvance(label[i]->text()) + 4;

            if (x + w > this->width()) {
                if (x == 0) {
                    // 每行至少1个
                } else {
                    x = 0;
                    m++;
                    line_start_index[m] = i;
                    line_count++;
                }
            }
            label[i]->setGeometry(x, 0, w, this->height());
            x += w;
        }
        set_show();
    }
}

void Text_List_Widget::set_show()
{
    if (label != NULL) {
        for (int i = 0; i < label_cnt; i++) {
            label[i]->hide();
        }
        int start = line_start_index[current_line - 1];
        int end;

        if (line_count > current_line) {
            end = line_start_index[current_line];
        } else {
            end = label_cnt;
        }

        QPalette pe;
        pe.setColor(QPalette::WindowText, QColor(0, 0, 0, 255));

        for (int i = start; i < end; i++) {
            if (i == current_checked_text) {
                pe.setColor(QPalette::WindowText, QColor(255, 0, 0, 255));

                // label[i]->setPalette(pe);
                label[i]->setStyleSheet("QLabel{color:rgba(255,0,0,255);}");
                pe.setColor(QPalette::WindowText, QColor(0, 0, 0, 255));
            } else {
                // label[i]->setPalette(pe);
                label[i]->setStyleSheet("QLabel{color:rgba(0,0,0,255);}");
            }
            label[i]->show();
        }
    }
}
