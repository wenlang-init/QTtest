#include "customplotqmlwidget.h"
#include <QQuickWidget>
#include <QQmlEngine>
#include <QQmlContext>
#include <QBoxLayout>
#include <QDebug>
#include "customplotitem.h"
#include "fftw3object.h"
#include <QTimer>
#include "gensignalwidget.h"
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>

CustomPlotQMLWidget::CustomPlotQMLWidget(QWidget *parent)
    : QWidget{parent}
{
    qw = new QQuickWidget(this);
    qw->setResizeMode(QQuickWidget::SizeRootObjectToView);
    qw->rootContext()->setContextProperty("CustomPlotQMLWidget", this);
    qmlRegisterType<CustomPlotItem>("CustomPlotItem", 1, 0, "CustomPlotItem");
    qw->setSource(QUrl("qrc:/src/fftw/customplot.qml"));
    qw->showFullScreen();
    connect(qw, &QQuickWidget::sceneGraphError, this,
            [ = ](QQuickWindow::SceneGraphError error, const QString& message) {
        qDebug() << error << message;
    });
    connect(qw, &QQuickWidget::statusChanged, this,
            [ = ](QQuickWidget::Status status) {
        qDebug() << status;
    });

    {
        m_genSignalWidget = new genSignalWidget;
        m_genSignalWidget->hide();
        connect(m_genSignalWidget, &genSignalWidget::updateData, this, [ = ](
                    QByteArray data,
                    int        channalCount,
                    int        audioSample,
                    int        byteType,
                    bool       isLittle,
                    int        fftwindow,
                    bool       isupdate)
        {
            if (1) {
                int byteRate =
                    fftw3Object::signaTypeToByte(
                        (fftw3Object::signaType)byteType);
                QString strtime;
                strtime += " 时长: ";
                qint64 times = data.size() /
                               (1.0 * byteRate * audioSample * channalCount) * 1000;
                strtime += QTime::fromMSecsSinceStartOfDay(times)
                           .toString("hh:mm:ss.zzz");

                // qDebug() << strtime;
                label->setText(strtime);
            }

            QVariant ret;
            QMetaObject::invokeMethod(
                qw->rootObject(),
                "getPlotPtr",
                Qt::DirectConnection,
                Q_RETURN_ARG(QVariant, ret) // ,Q_ARG(QVariant, arg1)
                );
            CustomPlotItem *item = ret.value<CustomPlotItem *>();

            if (!item) {
                return;
            }

            if (!isupdate) {
                item->clearAllData();
            }

            QVector<double>dsdata;
            QVector<double>fftdata;
            QVector<double>radiandata;
            fftw3Object::fft(data, dsdata, fftdata, radiandata,
                             channalCount, (fftw3Object::signaType)byteType,
                             fftwindow, fftwindow / 2, isLittle);

            if (comboBox->currentIndex() == 0) {
                // dsdata
                item->addData(dsdata, 0);
                item->setShowLastLen(spinbox->value(), 0);
            } else if (comboBox->currentIndex() == 1) {
                // fftdata
                item->clearData(0);
                item->setXRang(0, fftdata.size());
                item->addData(fftdata, 0);
            } else if (comboBox->currentIndex() == 2) {
                // fftdata
                double xmin = 0, xmax = 1;
                item->clearData(0);

                for (int i = 0; i < fftdata.size(); i++) {
                    double val =  20 * log10(fftdata[i]) + 1e-12;
                    double fs = 1.0 * i * audioSample / (fftwindow / 2.0);
                    fs /= 2;

                    if (i == 0) xmin = fs;

                    if (i == fftdata.size() - 1) xmax = fs;
                    item->addData(fs, val, 0);
                }
                item->setXRang(xmin, xmax);
            } else if (comboBox->currentIndex() == 3) {
                // radiandata
                item->clearData(0);
                item->setXRang(0, radiandata.size());
                item->addData(radiandata, 0);
            } else  {
                // data
                QVector<double>_data;

                for (int i = 0; i < data.size(); i++) {
                    _data.append(data[i]);
                }
                item->addData(_data, 0);
                item->setShowLastLen(spinbox->value(), 0);
            }

            // item->refresh();

            if (100 < m_dateTime.msecsTo(QDateTime::currentDateTime())) {
                m_dateTime = QDateTime::currentDateTime();
                item->refresh();
            }
        });
    }

    QPushButton *pushbutton = new QPushButton(this);
    pushbutton->setText("打开信号生成配置窗口");
    connect(pushbutton, &QPushButton::clicked, this, [ = ]() {
        if (m_genSignalWidget->isHidden()) {
            m_genSignalWidget->show();
        }
    });

    comboBox = new QComboBox(this);
    comboBox->setEditable(false);
    comboBox->addItem("显示时域数据");
    comboBox->addItem("显示频域数据");
    comboBox->addItem("显示DB频率数据");
    comboBox->addItem("显示相位数据");
    comboBox->addItem("显示原始数据");
    connect(comboBox, &QComboBox::currentIndexChanged, this, [ = ] {
        QVariant ret;
        QMetaObject::invokeMethod(
            qw->rootObject(),
            "getPlotPtr",
            Qt::DirectConnection,
            Q_RETURN_ARG(QVariant, ret) // ,Q_ARG(QVariant, arg1)
            );
        CustomPlotItem *item = ret.value<CustomPlotItem *>();

        if (item) {
            item->clearAllData();
            item->setMaxCount(10000000);
        }

        if (comboBox->currentIndex() == 0) {
            item->setLabel("x", "y");
        } else if (comboBox->currentIndex() == 1) {
            item->setYRang(0, 0.2);
            item->setLabel("x", "幅度");
        }  else if (comboBox->currentIndex() == 2) {
            item->setYRang(-100, -10);
            item->setLabel("频率", "DB");
        } else if (comboBox->currentIndex() == 3) {
            item->setYRang(-10, 10);
            item->setLabel("x", "相位");
        } else {
            item->setLabel("x", "y");
        }
    });

    QCheckBox *checkbox = new QCheckBox("显示追踪线", this);
    checkbox->setChecked(true);
    connect(checkbox, &QCheckBox::checkStateChanged, this, [ = ] {
        QVariant ret;
        QMetaObject::invokeMethod(
            qw->rootObject(),
            "getPlotPtr",
            Qt::DirectConnection,
            Q_RETURN_ARG(QVariant, ret) // ,Q_ARG(QVariant, arg1)
            );
        CustomPlotItem *item = ret.value<CustomPlotItem *>();

        if (item) {
            item->setShowTracer(checkbox->isChecked());
            item->refresh();
        }
    });

    spinbox = new QSpinBox(this);
    spinbox->setRange(1, 0x7fffffff);
    spinbox->setValue(3000);

    QHBoxLayout *hboxlayout = new QHBoxLayout;
    hboxlayout->setContentsMargins(0, 0, 0, 0);
    hboxlayout->addWidget(comboBox);
    hboxlayout->addWidget(checkbox);
    hboxlayout->addWidget(spinbox);

    label = new QLabel(this);
    label->setMaximumHeight(30);
    QVBoxLayout *vboxlayout = new QVBoxLayout(this);
    vboxlayout->setContentsMargins(0, 0, 0, 0);
    vboxlayout->addWidget(pushbutton);
    vboxlayout->addLayout(hboxlayout);
    vboxlayout->addWidget(label);
    vboxlayout->addWidget(qw);

    if (0) {
        QVariant ret;
        QMetaObject::invokeMethod(
            qw->rootObject(),
            "getPlotPtr",
            Qt::DirectConnection,
            Q_RETURN_ARG(QVariant, ret) // ,Q_ARG(QVariant, arg1)
            );

        CustomPlotItem *item = ret.value<CustomPlotItem *>();

        if (item) {
            QTimer *timer = new QTimer(this);
            timer->start(10);
            item->setYRang(-0.5, 0.5);
            item->setShowTracer(true);
            int cnt = 2048 * 3;
            connect(timer, &QTimer::timeout, this, [ = ]() {
                QVector<double>data;
                fftw3Object::generateSignalData(data,
                                                WaveformType::Sine,
                                                cnt, 440, 0.5, 0.0, 2, 44100
                                                );
                item->addData(data, 0);
                item->setShowLastLen(cnt, 0);
                item->refresh();

                // item->update(item->getCustomPlot()->rect());
                // item->update(QRect(0,0,item->width(),item->height()));
            });
        }
    }
}

CustomPlotQMLWidget::~CustomPlotQMLWidget()
{
    delete m_genSignalWidget;
}

void CustomPlotQMLWidget::addData(QVector<double>& data)
{
    QVariant ret;

    QMetaObject::invokeMethod(
        qw->rootObject(),
        "getPlotPtr",
        Qt::DirectConnection,
        Q_RETURN_ARG(QVariant, ret) // ,Q_ARG(QVariant, arg1)
        );

    CustomPlotItem *item = ret.value<CustomPlotItem *>();

    if (item) {
        item->addData(data, 0);
        item->setShowLastLen(10000, 0);

        item->refresh();
    }
}

void CustomPlotQMLWidget::addData(QVector<double>& data, double l, double r)
{
    QVariant ret;

    QMetaObject::invokeMethod(
        qw->rootObject(),
        "getPlotPtr",
        Qt::DirectConnection,
        Q_RETURN_ARG(QVariant, ret) // ,Q_ARG(QVariant, arg1)
        );

    CustomPlotItem *item = ret.value<CustomPlotItem *>();

    if (item) {
        item->clearData(0);
        item->addData(data, 0);
        item->setXRang(l, r);

        item->refresh();
    }
}
