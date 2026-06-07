#ifndef SLIPWIDGET_H
#define SLIPWIDGET_H

#include <QListWidget>

class SlipWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit SlipWidget(QWidget *parent = NULL,int w=48,int h=40);
    ~SlipWidget();
    void set_listtext(QStringList &strlist);

    void set_move_state(int value);
    void clear_list();
signals:

public slots:

protected:
    bool eventFilter(QObject *watch, QEvent *evn);
    void resizeEvent(QResizeEvent *e);
private slots:

private:
    int ti_width;
    int ti_height;
    QTimer *timer;
    int move_end;
    bool press;
    int lastposy;

    int move_state; // 0:可移动;1:默认方式移动,其他不可移动(包含默认移动方式)
};

#endif // SLIPWIDGET_H
