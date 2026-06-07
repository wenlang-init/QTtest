#ifndef MTABLEWIDGET_H
#define MTABLEWIDGET_H

#include <QTableWidget>

class MTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit MTableWidget(QWidget *parent = NULL,int rcnt = 4,int w=46,int h=50);
    ~MTableWidget();
    void set_listtext(QStringList &strlist);
    void reset_row_listtext(QStringList &strlist);

    void set_move_state(int value);
    void clear_list();

signals:

public slots:

protected:
    bool eventFilter(QObject *watch, QEvent *evn);
    void paintEvent(QPaintEvent *e);
private slots:

private:
    int ti_width;
    int ti_height;

    int move_end;

    bool press;
    int lastposx;
    int lastposy;
    int row_strcnt;

    int move_state; // 0:x,y都可移动;1:x可移动;2:y可移动;3:不可移动;其他默认移动
};

#endif // SLIPWIDGET_H
