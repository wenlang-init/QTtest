#include "input_method_widget.h"
#include <QKeyEvent>
#include <QDebug>
#include <QPainter>

Input_Method_Widget& Input_Method_Widget::instance()
{
    static Input_Method_Widget imw;

    return imw;
}

Input_Method_Widget::~Input_Method_Widget()
{}

Input_Method_Widget::Input_Method_Widget(QWidget *parent) : QWidget(parent)
{
    input_mode = 0;

    this->setWindowFlags(
        Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |
        Qt::Dialog);

    // this->setWindowModality(Qt::ApplicationModal); // 设置为模态，且阻止所有窗口输入

    keyinputwidget = new Key_Input_Widget(this);
    connect(keyinputwidget, SIGNAL(current_clicked_chinese(QString)), this,
            SLOT(current_clicked_chinese(QString)));
    connect(keyinputwidget, SIGNAL(clicked_delete()), this,
            SLOT(clicked_delete()));
    connect(keyinputwidget, SIGNAL(clicked_enter()), this,
            SLOT(clicked_enter()));


    virtualinputwidget1 = new Virtual_Input_Widget(this);
    connect(virtualinputwidget1, SIGNAL(clicked_str(QString&)), this,
            SLOT(clicked_str(QString&)));
    connect(virtualinputwidget1, SIGNAL(clicked_delete()), this,
            SLOT(clicked_delete()));
    connect(virtualinputwidget1, SIGNAL(clicked_enter()), this,
            SLOT(clicked_enter()));
    connect(virtualinputwidget1, SIGNAL(close_widget()), this,
            SLOT(close_widget_v()));

    virtualinputwidget2 = new T9_Widget(this);
    connect(virtualinputwidget2, SIGNAL(input_text(QString&)), this,
            SLOT(click_input_s(QString&)));
    connect(virtualinputwidget2, SIGNAL(input_key(int)), this,
            SLOT(click_input_i(int)));
    connect(virtualinputwidget2, SIGNAL(close_widget()), this,
            SLOT(close_widget_v()));

    set_show();
}

bool Input_Method_Widget::set_mode(int mode)
{
    if ((mode < 0) || (mode > 2)) return false;

    input_mode = mode;

    set_show();

    return true;
}

int Input_Method_Widget::get_mode()
{
    return input_mode;
}

bool Input_Method_Widget::set_input_mode(int mode, bool show)
{
    if ((input_mode != 1) && (input_mode != 2)) {
        if (show == true) {
            this->show();
        }
        return false;
    }

    if ((mode < INPUT_MODE_VALUE_T9) ||
        (mode > INPUT_MODE_VALUE_SYMBOL)) return false;

    if (input_mode == 1) {
        virtualinputwidget1->show_mode(mode);
    } else {
        int value;

        if (mode == INPUT_MODE_VALUE_T9) {
            value = 1;
        } else if (mode == INPUT_MODE_VALUE_T26) {
            value = 5;
        } else if (mode == INPUT_MODE_VALUE_NUMBER) {
            value = 3;
        } else if (mode == INPUT_MODE_VALUE_SYMBOL) {
            value = 2;
        }
        virtualinputwidget2->set_show(value);
    }

    if (show == true) {
        this->show();
    }
    return true;
}

void Input_Method_Widget::resizeEvent(QResizeEvent *event)
{
    // keyinputwidget->setGeometry(0,this->height()-40,this->width(),40);
    keyinputwidget->setGeometry(this->rect());

    // virtualinputwidget1->setGeometry(0,this->height()-200,this->width(),200);
    // virtualinputwidget2->setGeometry(0,this->height()-200,this->width(),200);

    virtualinputwidget1->setGeometry(this->rect());
    virtualinputwidget2->setGeometry(this->rect());

    QWidget::resizeEvent(event);
}

void Input_Method_Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    QColor background_color = QColor(0, 0, 0, 255);
    painter.setBrush(background_color);
    painter.drawRect(rect());

    QWidget::paintEvent(event);
}

void Input_Method_Widget::keyPressEvent(QKeyEvent *event)
{
    if (input_mode == 0) {
        keyinputwidget->Key_System_Code(event->key());

        // keyinputwidget->Key_Input_Code(event->key());
    }
    QWidget::keyPressEvent(event);
}

void Input_Method_Widget::set_show()
{
    keyinputwidget->reset();
    virtualinputwidget1->reset();
    virtualinputwidget2->reset();

    keyinputwidget->hide();
    virtualinputwidget1->hide();
    virtualinputwidget2->hide();

    if (input_mode == 0) {
        keyinputwidget->show();
    } else if (input_mode == 1) {
        virtualinputwidget1->show();
    } else if (input_mode == 2) {
        virtualinputwidget2->show();
    }
}

void Input_Method_Widget::Key_Input_Code(int key)
{
    if (this->isVisible() == false) {
        return;
    }

    if (input_mode == 0) {
        keyinputwidget->Key_Input_Code(key);
    } else if (input_mode == 1) {
        virtualinputwidget1->Key_Input_Code(key);
    } else if (input_mode == 2) {
        // virtualinputwidget2->Key_Input_Code(key);
    }
}

void Input_Method_Widget::Key_System_Code(int key)
{
    if (input_mode == 0) {
        keyinputwidget->Key_System_Code(key);
    }
}

void Input_Method_Widget::current_clicked_chinese(QString str)
{
    emit clicked_text(str);
}

void Input_Method_Widget::clicked_str(QString& str)
{
    emit clicked_text(str);
}

void Input_Method_Widget::clicked_delete()
{
    emit clicked_value(INPUT_CODE_VALUE_DELETE);
}

void Input_Method_Widget::clicked_enter()
{
    emit clicked_value(INPUT_CODE_VALUE_ENTER);
}

void Input_Method_Widget::click_input_s(QString& value)
{
    emit clicked_text(value);
}

void Input_Method_Widget::click_input_i(int value)
{
    switch (value) {
    case 0:

        // emit clicked_value(INPUT_CODE_VALUE_RESET);
        break;

    case 1:
        emit clicked_value(INPUT_CODE_VALUE_ENTER);
        break;

    case 2:
        emit clicked_value(INPUT_CODE_VALUE_DELETE);
        break;

    default:
        break;
    }
}

void Input_Method_Widget::close_widget_v()
{
    emit clicked_value(INPUT_CODE_VALUE_CLOSE);

    hide();
}
