#ifndef T9_INPUT_WIDGET_H
#define T9_INPUT_WIDGET_H

#include <QWidget>

enum KEY_DEFINE_T9{
    KEY_DEFINE_T9_MORE, // 更多
    KEY_DEFINE_T9_1, // 按键1
    KEY_DEFINE_T9_2,
    KEY_DEFINE_T9_3,
    KEY_DEFINE_T9_4,
    KEY_DEFINE_T9_5,
    KEY_DEFINE_T9_6,
    KEY_DEFINE_T9_7,
    KEY_DEFINE_T9_8,
    KEY_DEFINE_T9_9,
    KEY_DEFINE_T9_DEL, // 删除
    KEY_DEFINE_T9_RELOSE, // 重输
    KEY_DEFINE_T9_ENTER // 回车键
};

class QPushButton;
class SlipWidget;
class MTableWidget;
class QListWidgetItem;

class T9_Input_Widget : public QWidget
{
    Q_OBJECT
public:
    explicit T9_Input_Widget(QWidget *parent = NULL);
    ~T9_Input_Widget();
    void set_pinyin_list(QStringList &strlist);
    void set_chinese_list(QList<QStringList> &strlist);
    void reset();
    // 获取当前文字
    QString current_text();
    // 选择上一个文字
    bool set_pretext();
    // 选择下一个文字
    bool set_nexttext();
    // 选择上一个拼音
    bool set_prepinyin();
    // 选择下一个拼音
    bool set_nextpinyin();
private:
    void init_widget();
    void set_show(bool state);
protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
signals:
    // 0~12,对应枚举值
    void clicked_key(int value);
    // 点击项的汉字
    void clicked_chinese(QString text);
public slots:
    void itemClicked(QListWidgetItem *item);
    void cellClicked(int row, int column);
private:
    QColor background_color;
    QPushButton *pushbutton[13];
    SlipWidget *mlistwidget[2];
    MTableWidget* mtablewidget[1];

    QStringList outpinyin;
    QList<QStringList> outchinese;

    bool show_more;
};

#endif // T9_INPUT_WIDGET_H
