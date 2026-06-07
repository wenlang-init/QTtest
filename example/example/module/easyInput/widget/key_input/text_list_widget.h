#ifndef TEXT_LIST_WIDGET_H
#define TEXT_LIST_WIDGET_H

#include <QWidget>

class QLabel;

class Text_List_Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Text_List_Widget(QWidget *parent = NULL);
    ~Text_List_Widget();
    void reset();
    // 设置数据
    void set_text(QStringList &strlist);
    // 返回选中状态
    bool is_checked();
    // 设置选中状态
    void set_check(bool checked);
    //选择下一个
    void select_next();
    //选择上一个
    void select_prev();
    //选择下一页
    void select_next_page();
    //选择上一页
    void select_prev_page();
    // 获取选中的文本数据
    QString get_current_text();
    // 获取选中的序号(用于拼音)
    int get_current_index();
    // 获取当前行
    int get_current_row();
    // 获取行数
    int get_row_cnt();
    // 获取当前行第index个的文本内容
    QString get_index_text(int index);
protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
private:
    void setgeometry();
    void set_show();
signals:
public slots:
private:
    QLabel **label;
    int label_cnt; // label个数
    int current_checked_text; // 当前选中的文本项0~label_cnt-1;
    int line_count; // 行数
    int *line_start_index; // 每行的开始序号
    int current_line; // 当前行号,1~line_count
    bool m_checked; // 自己的选中状态
    QStringList m_strlist; // 保存的数据
};

#endif // TEXT_LIST_WIDGET_H
