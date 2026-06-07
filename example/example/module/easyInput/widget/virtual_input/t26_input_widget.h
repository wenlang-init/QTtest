#ifndef T26_INPUT_WIDGET_H
#define T26_INPUT_WIDGET_H

#include <QWidget>

enum KEY_DEFINE_T26{
    KEY_DEFINE_T26_0,
    KEY_DEFINE_T26_A,
    KEY_DEFINE_T26_B,
    KEY_DEFINE_T26_C,
    KEY_DEFINE_T26_D,
    KEY_DEFINE_T26_E,
    KEY_DEFINE_T26_F,
    KEY_DEFINE_T26_G,
    KEY_DEFINE_T26_H,
    KEY_DEFINE_T26_I,
    KEY_DEFINE_T26_J,
    KEY_DEFINE_T26_K,
    KEY_DEFINE_T26_L,
    KEY_DEFINE_T26_M,
    KEY_DEFINE_T26_N,
    KEY_DEFINE_T26_O,
    KEY_DEFINE_T26_P,
    KEY_DEFINE_T26_Q,
    KEY_DEFINE_T26_R,
    KEY_DEFINE_T26_S,
    KEY_DEFINE_T26_T,
    KEY_DEFINE_T26_U,
    KEY_DEFINE_T26_V,
    KEY_DEFINE_T26_W,
    KEY_DEFINE_T26_X,
    KEY_DEFINE_T26_Y,
    KEY_DEFINE_T26_Z,
    KEY_DEFINE_T26_DEL, // 删除
    KEY_DEFINE_T26_RELOSE, // 重输
    KEY_DEFINE_T26_ENTER // 回车键
};

class QLabel;
class QPushButton;
class SlipWidget;
class MTableWidget;
class QListWidgetItem;

class T26_Input_Widget : public QWidget
{
    Q_OBJECT
public:
    explicit T26_Input_Widget(QWidget *parent = NULL);
    void set_pinyin_str(QString &pinyin);
    void set_chinese_list(QStringList &strlist);
    void reset();

    // 获取当前文字
    QString current_text();
    // 选择上一个文字
    bool set_pretext();
    // 选择下一个文字
    bool set_nexttext();
protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
private:
    void show_widget();
    bool eventFilter(QObject *watched, QEvent *event);
signals:
    void clicked_key(int);
    void clicked_chinese(QString);
public slots:
    void itemClicked(QListWidgetItem *item);
    void cellClicked(int r,int c);
private:
    QColor background_color;
    QPushButton *pushbutton[35];
    QLabel *labelpinyin;
    SlipWidget *mlistwidget;
    MTableWidget* mtablewidget;

    QString pinyin;
    QStringList chineselist;

    bool show_more; // 显示更多
    bool chinese_mode; // 中文模式
    bool capital_mode; // 大写模式
};

#endif // T26_INPUT_WIDGET_H
