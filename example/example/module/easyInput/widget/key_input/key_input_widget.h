#ifndef KEY_INPUT_WIDGET_H
#define KEY_INPUT_WIDGET_H

#include <QWidget>
#include <QLabel>
#include "../input_management_object.h"
#include "../key_define.h"

class Text_List_Widget;

class Key_Input_Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Key_Input_Widget(QWidget *parent = NULL);
    void reset();

protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
private:
    void key_input_oper(int key);
    void info_reset();
    bool find_chinese();
    void set_label_chinese_info();
signals:
    void current_clicked_chinese(QString text);
    void clicked_delete();
    void clicked_enter();
public slots:
    void Key_Input_Code(int key);
    void Key_System_Code(int key);
private:
    QLabel *label;
    QLabel *label_chinese_info;
    Text_List_Widget *pinyin_widget;
    Text_List_Widget *chinese_widget;

    easyInput *easyinput;
    PY_OBJ *mT9obj;

    bool check_pinyin; // 为true表示选择的是拼音,否则为汉字

    bool t26_mode; // 输入模式:true T26;false T9

    QString input_str; // 输入的字母或数字
    int pycount; // 记录匹配的字个数
    QStringList pinyinlist;
    QList<QStringList> T9chineselist;

    QStringList T26chineselist;
};



#endif // KEY_INPUT_WIDGET_H
