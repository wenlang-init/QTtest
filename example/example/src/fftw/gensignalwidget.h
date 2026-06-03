#ifndef GENSIGNALWIDGET_H
#define GENSIGNALWIDGET_H

#include <QWidget>
#include "waveformgenerator.h"

namespace Ui {
class genSignalWidget;
}

class genSignalWidget : public QWidget {
    Q_OBJECT

public:

    explicit genSignalWidget(QWidget *parent = nullptr);
    ~genSignalWidget();

signals:

    void updateData(QByteArray data,
                    int        channalCount,
                    int        audioSample,
                    int        byteType, // fftw3Object::signaType
                    bool       isLittle,
                    int        fftwindow,
                    bool       isupdate
                    );

private slots:

    void on_pushButtonadd_3_clicked();

    void on_pushButton_3_clicked();

    void on_pushButtonaddzs_clicked();

    void on_pushButtonremovezs_clicked();
    void setInfo();
    void timerOut();
    void toByteArrray(QVector<double>& fdata,
                      double           _magnitude);

private:

    void genterSignal();

    Ui::genSignalWidget *ui;

    struct _params
    {
        double a;
        double b;
        double c;
    };
    QList<_params>_cosparams;
    QList<_params>_cosparamszs;

    QList<WaveformGenerator<double> *>m_waveformGeneratorList;
    QList<WaveformGenerator<double> *>m_waveformGeneratorZSList;
    QTimer *timer;
};

#endif // GENSIGNALWIDGET_H
