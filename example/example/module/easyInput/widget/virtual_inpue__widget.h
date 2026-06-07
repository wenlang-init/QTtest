#ifndef T9_WIDGET_H
#define T9_WIDGET_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include "basewidget/mtablewidget.h"
#include "basewidget/slipwidget.h"
#include "../googleinput/easyinput.h"
#include "../t9pinyin/py_t9_core.h"

class T9_Widget : public QDialog {
    Q_OBJECT

public:

    T9_Widget(QWidget *parent = 0,
              int      width = 240,
              int      height = 200);
    ~T9_Widget();
    void set_show(int state = 1); // 输入模式.
                                  // 1:piinyin;2:符号;3:数字,4:更多pinyin输入,5:t26
    int  start_exec(int state = 1);
    void set_background(QColor color);
    void reset();

signals:

    void input_text(QString& str);
    void input_key(int key); // 0重输，1确认，2删除，3关闭
    void close_widget();

protected:

    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    bool eventFilter(QObject *obj,
                     QEvent  *event);

private:

    void set_fuhao_table();
    void set_windows(int state);
    void get_chinese_w(QString& pinyinstr); // pinyin
    void get_en_w(int id);                  // en

    void swidget_last_check_pinyin();       // 上一个拼音
    void swidget_next_check_pinyin();

private slots:

    void buttont26press();

    void Swidgetb_click(QModelIndex index);    // 选择符号列表槽
    void twidget_m_clicked(QModelIndex index); // 选择符号槽

    void twidget_clicked(QModelIndex index);   // 选择汉字槽
    void Swidget_m_click(QModelIndex index);   // 选择汉字槽(扩展)

    void swidget_click(QModelIndex);           // 选择pinyin

    void Inpue_Key_Slot(int key);

private:

    QColor background_color;

    int ti_width;
    int ti_height;
    easyInput *easyinput;
    QString pinyinstr;
    QStringList pinyinlist;
    QStringList outstr;
    QList<QStringList>outlist;
    int pinyin_checkindex;

    QLabel *pinyinlabel;         // 拼音输入显示窗口

    SlipWidget *swidget;         // pinyin选择栏
    MTableWidget *twidget;       // 显示输入的结果

    SlipWidget *swidget_m;       // pinyin选择栏1
    SlipWidget *swidget_more;    // 显示更多输入的结果

    SlipWidget *swidget_b;       // 符号类型选择栏
    MTableWidget **twidget_more; // 显示符号

    int fuhao_rowcnt;            // 每行符号的个数,最少为4，窗口宽度/60

    QPushButton **button;
    QPushButton *returnbutton;   // 符号返回
    int input_state;             // 输入模式. 1:piinyin;2:符号;3:数字,4:更多pinyin输入,5:t26
    int input_state_last;
    bool ceng_mode;              // true:表示拼音,false:表示字母

    QPushButton **buttont26;

    QStringList strlisthead; // 符号头
    QStringList **strlist;   // 符号 表

    // T9查找
    // T9PY_IDX **mT9PyChinese;
    int pycount; // 记录匹配的字个数

    PY_OBJ *mT9obj;
};

#endif // T9_WIDGET_H
