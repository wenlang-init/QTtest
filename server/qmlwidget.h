
#ifndef QMLWIDGET_H
#define QMLWIDGET_H


#include <QWidget>
#include <QQuickWidget>

class qmlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit qmlWidget(QWidget *parent = nullptr);

signals:
    void hidewidget();
public slots:
    void closeshow();
//public:
//    Q_INVOKABLE void closeshow();
};

#endif // QMLWIDGET_H
