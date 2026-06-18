#ifndef CUSTOMPLOTQMLWIDGET_H
#define CUSTOMPLOTQMLWIDGET_H

#include <QWidget>
class QQuickWidget;
class CustomPlotQMLWidget : public QWidget {
    Q_OBJECT

public:

    explicit CustomPlotQMLWidget(QWidget *parent = nullptr);

    void addData(QVector<double>& data);
    void addData(QVector<double>& data,
                 double           l,
                 double           r);

signals:

private:

    QQuickWidget *qw;
};

#endif // CUSTOMPLOTQMLWIDGET_H
