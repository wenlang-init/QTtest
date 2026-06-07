#ifndef BOTTOM_ICON_H
#define BOTTOM_ICON_H

#include <QWidget>

class bottom_icon : public QWidget
{
    Q_OBJECT
public:
    explicit bottom_icon(QWidget *parent = NULL);
    void set_count(int count);
    void set_current(int index);
protected:
    void paintEvent(QPaintEvent *event);
signals:

private:
    int m_count;
    int m_index;
};

#endif // BOTTOM_ICON_H
