#include "input_method_widget.h"
#include <QKeyEvent>
#include <QDebug>
#include <QPainter>

#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QApplication>

Input_Method_Widget& Input_Method_Widget::instance()
{
    static Input_Method_Widget imw;

    return imw;
}

Input_Method_Widget::~Input_Method_Widget()
{}

Input_Method_Widget::Input_Method_Widget(QWidget *parent) : QWidget(parent)
{
    // 注册事件过滤器，控制输入框弹出
    qApp->installEventFilter(this);

    input_mode = 0;

    this->setWindowFlags(
        Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |
        Qt::Dialog | Qt::WindowDoesNotAcceptFocus);

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
#if 0
    connect(this, &Input_Method_Widget::clicked_text, this, [ = ](QString str) {
        QWidget *widget = QApplication::focusWidget();

        if (widget) {
            if (auto w = qobject_cast<QAbstractSpinBox *>(widget)) {
                QLineEdit *lineedit = w->findChild<QLineEdit *>(
                    QString(),
                    Qt::FindDirectChildrenOnly);

                if (lineedit) {
                    lineedit->setText(lineedit->text() + str);
                }
            }

            if (auto combobox = qobject_cast<QComboBox *>(widget)) {
                if (combobox->isEditable()) {
                    combobox->lineEdit()->setText(combobox->lineEdit()->text() +
                                                  str);
                }
            }

            if (auto w = qobject_cast<QLineEdit *>(widget)) {
                w->setText(w->text() + str);
            }

            if (auto w = qobject_cast<QTextEdit *>(widget)) {
                w->setPlainText(w->toPlainText() + str);
            }

            if (auto w = qobject_cast<QPlainTextEdit *>(widget)) {
                w->setPlainText(w->toPlainText() + str);
            }
        }
    });
#endif // if 0
    set_show();

    resize(256, 200);
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

bool Input_Method_Widget::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::FocusIn) {
        // QWidget *widget = qobject_cast<QWidget *>(watched);
        QWidget *widget = QApplication::focusWidget();

        if (widget && isEditableWidget(widget)) {
            if (isHidden()) {
                adjustKeyboardToCursor(widget);
                set_mode(INPUT_MODE_VIRTUAL1);
                set_input_mode(INPUT_MODE_VALUE_T26);
                show();
            }
        }
    } else if (event->type() == QEvent::FocusOut) {
        // QWidget *widget = qobject_cast<QWidget *>(watched);
        QWidget *widget = QApplication::focusWidget();

        if (widget && isEditableWidget(widget)) {
            // 失去焦点时如果是可编辑的，判断新控件是否可编辑
            QMetaObject::invokeMethod(this, [this]() {
                QWidget *now = QApplication::focusWidget();

                if (now) {
                    if (!isEditableWidget(now)) {
                        close_widget_v();
                    } else {
                        adjustKeyboardToCursor(now);
                    }
                }
            }, Qt::QueuedConnection);
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        // QWidget *widget = qobject_cast<QWidget *>(watched);
        QWidget *widget = QApplication::focusWidget();

        if (!widget) return false;

        // 查询最近的可编辑父控件(点击的可能是子控件，如textedit的viewport)
        QWidget *editable = widget;

        while (editable && !isEditableWidget(editable)) {
            editable = editable->parentWidget();
        }

        if (editable && (editable == QApplication::focusWidget()) &&
            isHidden()) {
            adjustKeyboardToCursor(editable);
            set_mode(INPUT_MODE_VIRTUAL1);
            set_input_mode(INPUT_MODE_VALUE_T26);
            show();
        }
    }
    return false; // 不拦截事件，继续传递
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

bool Input_Method_Widget::isEditableWidget(QWidget *widget) const
{
    if (!widget) return false;

    if (!widget->isEnabled()) return false;

    if (qobject_cast<QAbstractSpinBox *>(widget)) return true;

    if (auto combobox = qobject_cast<QComboBox *>(widget)) {
        return combobox->isEditable();
    }

    if (qobject_cast<QLineEdit *>(widget) ||
        qobject_cast<QTextEdit *>(     widget) ||
        qobject_cast<QPlainTextEdit *>(widget)) return true;

    return false;
}

void Input_Method_Widget::adjustKeyboardPosition(QWidget *focuseWidget)
{
    if (!focuseWidget) return;

    // 获取实际输入子控件
    if (auto widget = qobject_cast<QAbstractSpinBox *>(focuseWidget)) {
        // spinbox,doublespinbox,dateedit等
        QLineEdit *lineedit = widget->findChild<QLineEdit *>(
            QString(),
            Qt::FindDirectChildrenOnly);

        if (lineedit) focuseWidget = lineedit;
    } else if (auto widget = qobject_cast<QComboBox *>(focuseWidget)) {
        // combobox编辑框
        if (widget->isEditable()) {
            focuseWidget = widget->lineEdit();
        }
    }

    // 获取输入框全局矩形
    QRect  widgetRect = focuseWidget->rect();
    QPoint globalPos = focuseWidget->mapToGlobal(QPoint(0, 0));
    widgetRect.moveTopLeft(globalPos);

    QSize keyboardSize = size();

    // 获取屏幕可用区域
    if (!screen()) return;

    QRect screenRect = screen()->availableGeometry();

    // 计算键盘的X坐标，水平尽量居中，但不超出屏幕
    int keyboardX = widgetRect.center().x() - keyboardSize.width() / 2;
    keyboardX = qMax(keyboardX, screenRect.left());
    keyboardX = qMin(keyboardX, screenRect.right() - keyboardSize.width());

    // 计算键盘的Y坐标，优先在输入框下方,不足则放在上方
    int keyboardY = widgetRect.bottom() + 5;

    if (keyboardY + keyboardSize.height() > screenRect.bottom()) {
        keyboardY = widgetRect.top() - keyboardSize.height() - 5;
    }

    if (keyboardY < screenRect.top()) {
        keyboardY = screenRect.top();
    }

    // 移动键盘窗口
    move(keyboardX, keyboardY);
}

void Input_Method_Widget::adjustKeyboardToCursor(QWidget *focuseWidget)
{
    if (!focuseWidget) return;

    // 获取实际输入子控件
    if (auto widget = qobject_cast<QAbstractSpinBox *>(focuseWidget)) {
        // spinbox,doublespinbox,dateedit等
        QLineEdit *lineedit = widget->findChild<QLineEdit *>(
            QString(),
            Qt::FindDirectChildrenOnly);

        if (lineedit) focuseWidget = lineedit;
    } else if (auto widget = qobject_cast<QComboBox *>(focuseWidget)) {
        // combobox编辑框
        if (widget->isEditable()) {
            focuseWidget = widget->lineEdit();
        }
    }

    // 获取光标全局右下坐标
    QRect cursorRect =
        focuseWidget->inputMethodQuery(Qt::ImCursorRectangle).toRect();
    QPoint globalBottomRight =
        focuseWidget->mapToGlobal(cursorRect.bottomRight());

    QSize keyboardSize = size();

    // 获取屏幕可用区域
    if (!screen()) return;

    QRect screenRect = screen()->availableGeometry();

    // 计算键盘左上坐标
    int keyboardX = globalBottomRight.x() + 5;
    int keyboardY = globalBottomRight.y() + 5;

    // 边界处理
    if (keyboardX + keyboardSize.width() > screenRect.right()) {
        // 改光标为左下方
        QPoint point = focuseWidget->mapToGlobal(cursorRect.bottomLeft());
        keyboardX = point.x() - keyboardSize.width() - 5;
    }

    if (keyboardX < screenRect.left()) {
        keyboardX = screenRect.left();
    }

    if (keyboardY + keyboardSize.height() > screenRect.bottom()) {
        QPoint point = focuseWidget->mapToGlobal(cursorRect.topRight());
        keyboardY = point.y() - keyboardSize.height() - 5;
    }

    if (keyboardY < screenRect.top()) {
        keyboardY = screenRect.top();
    }

    // 移动键盘窗口
    move(keyboardX, keyboardY);
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
