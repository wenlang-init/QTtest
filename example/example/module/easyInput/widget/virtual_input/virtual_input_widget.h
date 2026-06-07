#ifndef VIRTUAL_INPUT_WIDGET_H
#define VIRTUAL_INPUT_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include "../input_management_object.h"
#include "t9_input_widget.h"
#include "t26_input_widget.h"
#include "number_input_widget.h"
#include "symbol_input_widget.h"

class Virtual_Input_Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Virtual_Input_Widget(QWidget *parent = NULL);
    void show_mode(int index);
    void reset();
protected:
    void resizeEvent(QResizeEvent *event);
private:
    void set_show(int value);
    bool find_chinese();
signals:
    void close_widget();
    void clicked_str(QString &text);
    void clicked_enter();
    void clicked_delete();
public slots:
    void button_clicked(bool);
    void clicked_key(int);
    void clicked_text(QString str);
    void Key_Input_Code(int key);
private:
    QPushButton *pushbutton[5];
    T9_Input_Widget *t9widget;
    T26_Input_Widget *t26widget;
    Number_Input_Widget *numberwidget;
    Symbol_Input_Widget *symbolwidget;

    easyInput *easyinput;
    PY_OBJ *mT9obj;

    QString input_str; // 输入的字母或数字
    int pycount; // 记录匹配的字个数
    QStringList pinyinlist;
    QList<QStringList> T9chineselist;

    QStringList T26chineselist;
    // 模式:0,t9;1,t26;2,数字;3,符号
    int input_mode;
};

#endif // VIRTUAL_INPUT_WIDGET_H
