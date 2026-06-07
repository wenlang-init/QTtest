#ifndef NUMBER_INPUT_WIDGET_H
#define NUMBER_INPUT_WIDGET_H

#include <QWidget>

class QPushButton;

class Number_Input_Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Number_Input_Widget(QWidget *parent = NULL);
    void reset();
protected:
    void resizeEvent(QResizeEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
signals:
    void clicked_chinese(QString);
public slots:
private:
    QColor background_color;
    QPushButton *pushbutton[17];
};

#endif // NUMBER_INPUT_WIDGET_H
