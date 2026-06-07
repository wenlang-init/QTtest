#ifndef SYMBOL_INPUT_WIDGET_H
#define SYMBOL_INPUT_WIDGET_H

#include <QWidget>

class SlipWidget;
class MTableWidget;
class QListWidgetItem;

class Symbol_Input_Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Symbol_Input_Widget(QWidget *parent = NULL);
    ~Symbol_Input_Widget();
    void reset();

    // 获取当前符号
    QString current_text();
    // 选择上一个符号表
    bool set_pretab();
    // 选择下一个符号表
    bool set_nexttab();
    // 选择上一个符号
    bool set_pretext();
    // 选择下一个符号
    bool set_nexttext();
protected:
    void resizeEvent(QResizeEvent *event);
private:
    void set_fuhao_table();
signals:
    void clicked_chinese(QString);
public slots:
    void itemClicked(QListWidgetItem *item);
    void cellClicked(int row,int column);
private:
    SlipWidget *mlistwidget;
    MTableWidget** mtablewidget;

    QStringList list;
    QList<QStringList> listlist;

    int fuhao_rowcnt; // 每行符号的个数,最少为4，窗口宽度/60
};

#endif // SYMBOL_INPUT_WIDGET_H
