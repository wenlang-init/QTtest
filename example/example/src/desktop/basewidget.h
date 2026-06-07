#ifndef BASEWIDGET_H
#define BASEWIDGET_H

#include <QWidget>
#include <QPainter>

class BaseWidget : public QWidget {
    Q_OBJECT

public:

    explicit BaseWidget(QWidget *parent = NULL);

    void setWidgetIndex(int index);
    int  getWidgetIndex();

    void set_input_status(bool status = true); // 设置输入法的数据是否有效

signals:

    void returnSignal(int);          // 返回信号
    void Key_Pass_Signal(int key,
                         int index); // 按键信号

public slots:

    void         returnWidget();               // 发出返回信号的函数(返回槽,多级返回)
    virtual void Key_Pass_Slot(int key,
                               int index) = 0; // 按键槽纯虚函数

protected:

    QPalette pe;
    QFont font;
    bool input_status; // 标志输入法的数据是否有效

private:

    int widgetIndex;
};

#endif // BASEWIDGET_H
