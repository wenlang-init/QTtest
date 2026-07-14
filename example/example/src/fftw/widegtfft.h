#ifndef WIDEGTFFT_H
#define WIDEGTFFT_H

#include <QWidget>
#include "audioobject.h"
#include <QThread>
#include <QAudioDecoder>
#include "udpobject.h"
#include "ffmpegobject.h"
#include "linechartwidget.h"
#include "gensignalwidget.h"

#define USEDXXWPLOT
#ifdef USEDXXWPLOT
# include "XxwCustomPlot.h"
#else // ifdef USEDXXWPLOT
# include "qcustomplotex.h"
#endif // ifdef USEDXXWPLOT

namespace Ui {
class widegtFFT;
}
class customPlotCurve;
class CustomPlotQMLWidget;
class widegtFFT : public QWidget {
    Q_OBJECT

public:

    void audioCoders();
    void testAudioDecoder();
    Q_INVOKABLE explicit widegtFFT(QWidget *parent = nullptr);
    ~widegtFFT();

protected slots:

    void testAudioDecoderERROR(QAudioDecoder::Error error);
    void readData(QByteArray data);

    void fftwData(QVector<double>        dsdata,
                  QList<QVector<double> >fftdata,
                  QList<QVector<double> >radianfftdata);

signals:

    void writeAudioSig(QByteArray   data,
                       QAudioFormat format);

protected:

    void initPlot();
    void init();

private:

    genSignalWidget *m_genSignalWidget;
    Ui::widegtFFT *ui;
    audioObject *m_audioObject;
    udpObject m_udpObject;
    QThread *thread;
    QAudioFormat m_format;
    double m_AudioSourceSample;
    int m_windowSize;

    bool m_audioIsOpen = false;

    QCheckBox *checkBox1;
    QCheckBox *checkBox2;
    QCheckBox *checkBox3;
    QCheckBox *checkBox4;
    QCheckBox *checkBox5;
    QCheckBox *checkBox6;
    QSpinBox *spinBox1;
    QSpinBox *spinBox2;

    QCheckBox *checkBox21;
    QCheckBox *checkBox22;
    QCheckBox *checkBox23;
    QCheckBox *checkBox24;
    QCheckBox *checkBox25;
    QDoubleSpinBox *doubleSpinBox21;
    QDoubleSpinBox *doubleSpinBox22;
    QDoubleSpinBox *doubleSpinBox23;
    QDoubleSpinBox *doubleSpinBox24;
    QDoubleSpinBox *doubleSpinBox25;

    customPlotCurve *m_customPlotCurve;
    customPlotCurve *m_customPlotCurveFFT;
    customPlotCurve *m_customPlotCurveRadian;
    customPlotCurve *m_customPlotCurveSData;
    LineChartWidget *m_lineChartWidget;
    customPlotCurve *m_customPlotCurveFFTDb;

    QDateTime m_dateTime1 = QDateTime::currentDateTime();
    QDateTime m_dateTime2 = QDateTime::currentDateTime();
    QDateTime m_dateTime3 = QDateTime::currentDateTime();
    QDateTime m_dateTime4 = QDateTime::currentDateTime();
    QDateTime m_dateTime5 = QDateTime::currentDateTime();
    const int m_plotFlushTime = 100; // 刷新间隔ms

    int m_xcount = 0;
    int m_xcount1 = 0;
    const int m_maxPointCount = 10000000;


    FfmpegObject m_ffmpegObject;
    QString m_saveFileName;
    QFile m_savePCMFile;
};

class customPlotCurve : public QWidget {
    Q_OBJECT

public:

    friend class widegtFFT;
    explicit customPlotCurve(QWidget *parent = nullptr);
    ~customPlotCurve();
    void setShowTracer(bool isShow);
    void setLabel(const QString& xStr,
                  const QString& yStr);
    void setLegendShow(bool isShow);
    void addGraph(const QString& name,
                  const QPen   & pen,
                  const QBrush & brush);
    void setXRang(double lower,
                  double upper);
    void setYRang(double lower,
                  double upper);

signals:

public slots:

private:

#ifdef USEDXXWPLOT
    XxwCustomPlot *m_customPlot;
#else // ifdef USEDXXWPLOT
    QCustomPlotEx *m_customPlot;
#endif // ifdef USEDXXWPLOT

    bool lLineV = false;
    bool rLineV = false;
    bool bLineH = false;
};

#endif // WIDEGTFFT_H
