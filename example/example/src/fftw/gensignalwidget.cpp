#include "gensignalwidget.h"
#include "ui_gensignalwidget.h"
#include "fftw3object.h"
#include <QtEndian>
#include <QDebug>
#include <QTimer>

genSignalWidget::genSignalWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::genSignalWidget)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer,          &QTimer::timeout,
            this, &genSignalWidget::timerOut);

    connect(ui->pushButton, &QPushButton::clicked, this, [ = ]() {
        if (!ui->checkBox_2->isChecked()) {
            genterSignal();
        }
    });

    connect(ui->spinBox_2,
            &QSpinBox::valueChanged,
            this,
            &genSignalWidget::setInfo);
    connect(ui->spinBox_3,
            &QSpinBox::valueChanged,
            this,
            &genSignalWidget::setInfo);
    setInfo();

    // 连续生成
    connect(ui->checkBox_2, &QCheckBox::checkStateChanged, this, [ = ]() {
        while (m_waveformGeneratorList.size() > 0) {
            delete m_waveformGeneratorList.takeFirst();
        }

        while (m_waveformGeneratorZSList.size() > 0) {
            delete m_waveformGeneratorZSList.takeFirst();
        }

        if (ui->checkBox_2->isChecked()) {
            timer->start(100);
        } else {
            timer->stop();
        }
    });
}

genSignalWidget::~genSignalWidget()
{
    delete ui;

    while (m_waveformGeneratorList.size() > 0) {
        delete m_waveformGeneratorList.takeFirst();
    }

    while (m_waveformGeneratorZSList.size() > 0) {
        delete m_waveformGeneratorZSList.takeFirst();
    }
}

void genSignalWidget::on_pushButtonadd_3_clicked()
{
    _params node;

    node.a = ui->doubleSpinBox->value();
    node.b = ui->doubleSpinBox_2->value();
    node.c = ui->doubleSpinBox_3->value();
    _cosparams.append(node);
    ui->comboBox->addItem(QString("%1*cos(2π*%2*x+%3)").
                          arg(node.a).arg(node.b).arg(node.c));
}

void genSignalWidget::on_pushButton_3_clicked()
{
    if ((ui->comboBox->currentIndex() < _cosparams.size()) &&
        (ui->comboBox->currentIndex() >= 0)) {
        _cosparams.removeAt(ui->comboBox->currentIndex());
    }

    if (ui->comboBox->count() > 0) {
        ui->comboBox->removeItem(ui->comboBox->currentIndex());
    }
}

void genSignalWidget::genterSignal()
{
    int channalCount = ui->spinBox->value();
    int sample = ui->spinBox_2->value();
    int freq = ui->spinBox_3->value();
    int pointCount = ui->spinBox_4->value();

    QVector<double> fdata(channalCount * pointCount, 0);
    QVector<double> fdatazs(channalCount * pointCount, 0);
    QVector<double> tmpdata;

    WaveformType sigleType = WaveformType::Sine;

    if (ui->comboBox_2->currentIndex() == 1) {
        sigleType = WaveformType::Square;
    } else if (ui->comboBox_2->currentIndex() == 2) {
        sigleType = WaveformType::Triangle;
    } else if (ui->comboBox_2->currentIndex() == 3) {
        sigleType = WaveformType::Sawtooth;
    } else if (ui->comboBox_2->currentIndex() == 4) {
        sigleType = WaveformType::Noise;
    }

    double _magnitude = 0;

    ///////////////////////////////////////////////////////
    std::random_device rd_;
    std::mt19937 noiseGen_ = std::mt19937(rd_());
    std::uniform_real_distribution<double> noiseDist_ =
        std::uniform_real_distribution<double>(-1.0, 1.0);

    for (int i = 0; i < _cosparamszs.size(); i++) {
        double magnitude = _cosparamszs[i].a;
        _magnitude += magnitude;

        for (int m = 0; m < fdatazs.size(); m++) {
            fdatazs[m] += noiseDist_(noiseGen_) * magnitude;
        }
    }

    if (_cosparamszs.size() > 1) {
        for (int m = 0; m < fdatazs.size(); m++) {
            fdatazs[m] /= _cosparamszs.size();
        }
    }

    //////////////////////////////////////////////////////////
    for (int i = 0; i < _cosparams.size(); i++) {
        double magnitude = _cosparams[i].a;
        int    mfreq = freq * _cosparams[i].b;

        if (mfreq <= 0) mfreq = 1;
        double firstPhase = _cosparams[i].c;
        _magnitude += magnitude;

        while (firstPhase > 2 * M_PI) {
            firstPhase -= 2 * M_PI;
        }

        while (firstPhase < 0) {
            firstPhase += 2 * M_PI;
        }

        fftw3Object::generateSignalData(tmpdata,
                                        sigleType,
                                        pointCount,
                                        mfreq,
                                        magnitude,
                                        firstPhase,
                                        channalCount,
                                        sample);

        for (int m = 0; m < tmpdata.size(); m++) {
            fdata[m] += tmpdata[m];
        }
    }

    /////////////////////多个信号取平均值//////////////////////////
    if (_cosparams.size() > 1) {
        for (int m = 0; m < fdata.size(); m++) {
            fdata[m] /= _cosparams.size();
        }
    }

    if (_cosparamszs.size() > 0) {
        for (int m = 0; m < fdata.size(); m++) {
            fdata[m] = (fdata[m] + fdatazs[m]) / 2;
        }
    }

    /////////////////////无符号信号偏移值//////////////////////////
    if (_cosparams.size() + _cosparamszs.size() > 1) {
        _magnitude /= (_cosparams.size() + _cosparamszs.size());
    }

    toByteArrray(fdata, _magnitude);

    // std::sort(fdatazs.begin(),
    //           fdatazs.end(),
    //           [](const double& a, const double& b) {
    //     return a > b;
    // });
    // qDebug() << fdatazs.first() << fdatazs.last();
}

void genSignalWidget::on_pushButtonaddzs_clicked()
{
    _params node;

    node.a = ui->doubleSpinBoxzs->value();
    _cosparamszs.append(node);
    ui->comboBoxzs->addItem(QString("%1*cos(x)").arg(node.a));
}

void genSignalWidget::on_pushButtonremovezs_clicked()
{
    if ((ui->comboBoxzs->currentIndex() < _cosparamszs.size()) &&
        (ui->comboBoxzs->currentIndex() >= 0)) {
        _cosparamszs.removeAt(ui->comboBoxzs->currentIndex());
    }

    if (ui->comboBoxzs->count() > 0) {
        ui->comboBoxzs->removeItem(ui->comboBoxzs->currentIndex());
    }
}

void genSignalWidget::setInfo()
{
    QString str;

    int cnt = ui->spinBox_2->value() / ui->spinBox_3->value();

    str += QString("每个周期的点数为:%1\n").arg(cnt);
    str += QString("采样点数和计算FFT的采样数不能低于这个数\n");
    str += QString("采样率不能小于频率的2倍\n");
    str += QString("采样点数不能小于计算FFT的采样数的2倍\n");

    ui->label->setText(str);
}

void genSignalWidget::timerOut()
{
    int channalCount = ui->spinBox->value();
    int sample = ui->spinBox_2->value();
    int freq = ui->spinBox_3->value();
    int pointCount = sample * (timer->interval() / 1000.0) * 1;
    WaveformType sigleType = WaveformType::Sine;

    if (ui->comboBox_2->currentIndex() == 1) {
        sigleType = WaveformType::Square;
    } else if (ui->comboBox_2->currentIndex() == 2) {
        sigleType = WaveformType::Triangle;
    } else if (ui->comboBox_2->currentIndex() == 3) {
        sigleType = WaveformType::Sawtooth;
    } else if (ui->comboBox_2->currentIndex() == 4) {
        sigleType = WaveformType::Noise;
    }

    QVector<double> data(channalCount * pointCount, 0);
    QVector<double> dataZS(channalCount * pointCount, 0);
    double _magnitude = 0;

    if (m_waveformGeneratorZSList.size() != _cosparamszs.size()) {
        while (m_waveformGeneratorZSList.size() > 0) {
            delete m_waveformGeneratorZSList.takeFirst();
        }

        for (int i = 0; i < _cosparamszs.size(); i++) {
            double magnitude = _cosparamszs[i].a;
            int    mfreq = 1000;
            double firstPhase = 0;
            _magnitude += magnitude;

            while (firstPhase > 2 * M_PI) {
                firstPhase -= 2 * M_PI;
            }

            while (firstPhase < 0) {
                firstPhase += 2 * M_PI;
            }

            WaveformGenerator<double> *eGen = new WaveformGenerator<double>(
                sample,
                mfreq,
                magnitude,
                firstPhase,
                channalCount,
                WaveformType::Noise);
            m_waveformGeneratorZSList.append(eGen);
        }
    }

    if (m_waveformGeneratorList.size() != _cosparams.size()) {
        while (m_waveformGeneratorList.size() > 0) {
            delete m_waveformGeneratorList.takeFirst();
        }

        for (int i = 0; i < _cosparams.size(); i++) {
            double magnitude = _cosparams[i].a;
            int    mfreq = freq * _cosparams[i].b;

            if (mfreq <= 0) mfreq = 1;
            double firstPhase = _cosparams[i].c;
            _magnitude += magnitude;

            while (firstPhase > 2 * M_PI) {
                firstPhase -= 2 * M_PI;
            }

            while (firstPhase < 0) {
                firstPhase += 2 * M_PI;
            }

            WaveformGenerator<double> *eGen = new WaveformGenerator<double>(
                sample,
                mfreq,
                magnitude,
                firstPhase,
                channalCount,
                sigleType);
            m_waveformGeneratorList.append(eGen);
        }
    }

    for (int i = 0; i < _cosparamszs.size(); i++) {
        for (int j = 0; j < pointCount; j++) {
            std::vector<double> node =
                m_waveformGeneratorZSList[i]->nextFrame();

            for (int k = 0; k < node.size(); k++) {
                dataZS[j * channalCount + k] += node[k];
            }
        }
    }

    for (int i = 0; i < _cosparams.size(); i++) {
        for (int j = 0; j < pointCount; j++) {
            std::vector<double> node =
                m_waveformGeneratorList[i]->nextFrame();

            for (int k = 0; k < node.size(); k++) {
                data[j * channalCount + k] += node[k];
            }
        }
    }

    /////////////////////无符号信号偏移值//////////////////////////
    if (_cosparams.size() + _cosparamszs.size() > 1) {
        _magnitude /= (_cosparams.size() + _cosparamszs.size());
    }

    /////////////////////数据取平均值//////////////////////////
    if (_cosparamszs.size() > 1) {
        for (int j = 0; j < dataZS.size(); j++) {
            dataZS[j] /= _cosparamszs.size();
        }
    }

    if (_cosparams.size() > 1) {
        for (int j = 0; j < data.size(); j++) {
            data[j] /= _cosparams.size();
        }
    }

    if (_cosparamszs.size() > 0) {
        for (int m = 0; m < data.size(); m++) {
            data[m] = (data[m] + dataZS[m]) / 2;
        }
    }

    toByteArrray(data, _magnitude);
}

void genSignalWidget::toByteArrray(QVector<double>& fdata,
                                   double           _magnitude)
{
    QByteArray data;
    int  channalCount = ui->spinBox->value();
    int  sample = ui->spinBox_2->value();
    bool isLittle = ui->checkBox->isChecked();
    int  fftwindow = ui->spinBoxfft->value();

    fftw3Object::signaType bitType = fftw3Object::INT16;

    if (ui->comboBox_3->currentIndex() == 0) {
        // uint8
        bitType = fftw3Object::UINT8;
        double maxVal_ = std::numeric_limits<char>::max();

        for (int i = 0; i < fdata.size(); i++) {
            fdata[i] = (fdata[i] + _magnitude) * maxVal_;
            data.append((unsigned char)fdata[i]);
        }
    } else if (ui->comboBox_3->currentIndex() == 1) {
        // int8
        bitType = fftw3Object::INT8;
        double maxVal_ = std::numeric_limits<char>::max();

        for (int i = 0; i < fdata.size(); i++) {
            fdata[i] *= maxVal_;
            data.append((char)fdata[i]);
        }
    } else if (ui->comboBox_3->currentIndex() == 2) {
        // uint16
        bitType = fftw3Object::UINT16;
        double maxVal_ = std::numeric_limits<short>::max();

        for (int i = 0; i < fdata.size(); i++) {
            fdata[i] = (fdata[i] + _magnitude) * maxVal_;
            unsigned short v = fdata[i];

            if (isLittle) {
                v = qToLittleEndian(v);
            } else {
                v = qToBigEndian(v);
            }
            data.append((char *)&v, sizeof(v));
        }
    } else if (ui->comboBox_3->currentIndex() == 4) {
        // uint32
        bitType = fftw3Object::UINT32;
        double maxVal_ = std::numeric_limits<int>::max();

        for (int i = 0; i < fdata.size(); i++) {
            fdata[i] = (fdata[i] + _magnitude) * maxVal_;
            unsigned int v = fdata[i];

            if (isLittle) {
                v = qToLittleEndian(v);
            } else {
                v = qToBigEndian(v);
            }
            data.append((char *)&v, sizeof(v));
        }
    } else if (ui->comboBox_3->currentIndex() == 5) {
        // int32
        bitType = fftw3Object::INT32;
        double maxVal_ = std::numeric_limits<int>::max();

        for (int i = 0; i < fdata.size(); i++) {
            fdata[i] *= maxVal_;
            int v = fdata[i];

            if (isLittle) {
                v = qToLittleEndian(v);
            } else {
                v = qToBigEndian(v);
            }
            data.append((char *)&v, sizeof(v));
        }
    } else if (ui->comboBox_3->currentIndex() == 6) {
        // float
        bitType = fftw3Object::FLOAT;

        for (int i = 0; i < fdata.size(); i++) {
            float v = fdata[i];

            if (isLittle) {
                v = qToLittleEndian(v);
            } else {
                v = qToBigEndian(v);
            }
            data.append((char *)&v, sizeof(v));
        }
    } else {
        // int16
        bitType = fftw3Object::INT16;
        double maxVal_ = std::numeric_limits<short>::max();

        for (int i = 0; i < fdata.size(); i++) {
            fdata[i] *= maxVal_;
            short v = fdata[i];

            if (isLittle) {
                v = qToLittleEndian(v);
            } else {
                v = qToBigEndian(v);
            }
            data.append((char *)&v, sizeof(v));
        }
    }

    if (ui->checkBox_2->isChecked()) {
        emit updateData(data, channalCount, sample, bitType,
                        isLittle, fftwindow, true);
    } else {
        emit updateData(data, channalCount, sample, bitType,
                        isLittle, fftwindow, false);
    }
}
