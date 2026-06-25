#ifndef CUSTOMPLOTQMLWIDGET_H
#define CUSTOMPLOTQMLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QDateTime>
class QComboBox;
class QSpinBox;
class QQuickWidget;
class genSignalWidget;
class CustomPlotQMLWidget : public QWidget {
    Q_OBJECT

public:

    explicit CustomPlotQMLWidget(QWidget *parent = nullptr);
    ~CustomPlotQMLWidget();

    void addData(QVector<double>& data);
    void addData(QVector<double>& data,
                 double           l,
                 double           r);

signals:

private:

    QQuickWidget *qw;
    genSignalWidget *m_genSignalWidget;
    qint64 m_xcount = 0;
    QLabel *label;
    QComboBox *comboBox;
    QSpinBox *spinbox;

    QDateTime m_dateTime = QDateTime::currentDateTime();
};

#endif // CUSTOMPLOTQMLWIDGET_H
