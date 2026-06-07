#ifndef INPUT_METHOD_WIDGET_H
#define INPUT_METHOD_WIDGET_H

#include <QWidget>
#include <QDialog>
#include "widget/key_input/key_input_widget.h"
#include "widget/virtual_input/virtual_input_widget.h"
#include "widget/virtual_inpue__widget.h"

enum INPUT_CODE_VALUE {
    INPUT_CODE_VALUE_RESET,  // 重输
    INPUT_CODE_VALUE_ENTER,  // 确认
    INPUT_CODE_VALUE_DELETE, // 删除
    INPUT_CODE_VALUE_CLOSE   // 关闭
};
enum INPUT_MODE {
    INPUT_MODE_KEY,
    INPUT_MODE_VIRTUAL1,
    INPUT_MODE_VIRTUAL2
};

// 虚拟键盘模式
enum INPUT_MODE_VALUE {
    INPUT_MODE_VALUE_T9,
    INPUT_MODE_VALUE_T26,
    INPUT_MODE_VALUE_NUMBER,               // 数字
    INPUT_MODE_VALUE_SYMBOL                // 符号
};

class Input_Method_Widget : public QWidget // QDialog
{
    Q_OBJECT

private:

    explicit Input_Method_Widget(QWidget *parent = NULL);

public:

    static Input_Method_Widget& instance();
    ~Input_Method_Widget();

    // 设置输入法
    bool set_mode(int mode);
    int  get_mode();

    // 设置虚拟键盘输入模式
    bool set_input_mode(int  mode = 1,
                        bool show = true);

protected:

    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:

    void set_show();

signals:

    void clicked_text(QString str);
    void clicked_value(int value);

public slots:

    void Key_Input_Code(int key);
    void Key_System_Code(int key);

    // key
    void current_clicked_chinese(QString str);

    // virtual 1
    void clicked_str(QString& str);
    void clicked_delete();
    void clicked_enter();

    // virtual 2
    void click_input_s(QString& value);
    void click_input_i(int value);

    void close_widget_v();

private:

    // 输入法选择,0,使用按键；1,使用虚拟键盘1;2，使用虚拟键盘2
    int input_mode;

    Key_Input_Widget *keyinputwidget;
    Virtual_Input_Widget *virtualinputwidget1;
    T9_Widget *virtualinputwidget2;
};

// 在主函数中使用，自动显示输入法
#define SETAUTOSHOW_INPUT_METHOD_WIDGET()                                  \
    QObject::connect(qApp, &QApplication::focusChanged,                    \
                     [&](QWidget *old,                                     \
                         QWidget *now) {                                   \
        if (Input_Method_Widget::instance().isHidden()  &&                 \
            now && (now->inherits("QLineEdit") ||                          \
                    now->inherits("QTextEdit")                             \
                    )                                                      \
            ) {                                                            \
            Input_Method_Widget::instance().set_mode(INPUT_MODE_VIRTUAL1); \
            Input_Method_Widget::instance().set_input_mode(                \
                INPUT_MODE_VALUE_T26);                                     \
            Input_Method_Widget::instance().show();                        \
        }                                                                  \
    })

#endif // INPUT_METHOD_WIDGET_H
