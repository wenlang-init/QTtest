#ifndef WIDGET_H
#define WIDGET_H

#define USED_XXW
#ifdef USED_XXW
#include "Xxw/XxwCustomPlot.h"
#else
#include "qcustomplot/qcustomplot.h"
#endif
#include <QWidget>
#include <QThread>
#include "audioproc.h"
#include <QAudioSink>
#include <QAudioSource>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void initWidget();
    void initcustomolot();
#ifndef USED_XXW
    bool eventFilter(QObject *watched, QEvent *event) override;
    void handlePositionToolTip(QObject *watched,QMouseEvent *event);
    bool getGraphMinDistance(QCustomPlot *m_pPlot,QCPGraph* m_vecGraph,int iIndex, double &dValueX,double &dValueY);
#endif
protected slots:
    void opensource();
    void closesource();

    void opensink();
    void closesink();

    void handleStateChanged(QAudio::State newState);
    void dataflush(CurveData timeData,CurveData frequencyDomainData,QString str);

    void timeout();
signals:
    void readData(QByteArray data,QAudioFormat format);
    void clearData();
private slots:
    void on_pushButtonadd_clicked();

    void on_pushButton_clicked();

    void on_pushButtonrand_clicked();

    void on_pushButton1_clicked();

    void on_spinBox_valueChanged(int arg1);

    void on_doubleSpinBox_valueChanged(double arg1);

private:
    Ui::Widget *ui;

    QAudioFormat format;
    QAudioSource *audioin = nullptr;
    QAudioSink *audioout = nullptr;

    AudioProc *audiop;
    QThread *thread;

    #ifdef USED_XXW
    XxwCustomPlot *customPlot;
    XxwCustomPlot *customPlotfft;
    #else
    QCustomPlot *customPlot;
    QCustomPlot *customPlotfft;
    #endif

    qreal last_index = 0;
    qreal index = 0;

    QByteArray m_data;

    struct _params
    {
        double a;
        double b;
        double c;
    };
    QList<_params> _cosparams;
    QList<_params> _cosrandparams;
};
#endif // WIDGET_H
