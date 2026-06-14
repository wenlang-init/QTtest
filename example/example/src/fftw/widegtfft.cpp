#include "widegtfft.h"
#include "ui_widegtfft.h"
#include <QAudioDevice>
#include <QMediaDevices>
#include <QMetaEnum>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QCheckBox>
#include <QFileDialog>
#include "fftw3object.h"

// #define USE_TWO_XZHOU

template<typename T>
inline static QString enumValueToKey(T tempValue)
{
    auto tempEnum = QMetaEnum::fromType<T>();

    return tempEnum.valueToKey(tempValue);
}

template<typename T>
static int enumKeyToValue(T tempValue, const QString& sKeyName)
{
    auto tempEnum = QMetaEnum::fromType<T>();

    return tempEnum.keyToValue(sKeyName.toStdString().c_str());
}

#include <QMediaRecorder>
#include <QMediaFormat>
#include <QAudioDevice>
#include <QMediaCaptureSession>

// 录制，编码
void widegtFFT::audioCoders()
{
    // 1. 创建录制相关的对象
    QMediaCaptureSession session;

    QMediaRecorder *recorder = new QMediaRecorder();

    session.setRecorder(recorder);

    // 2. 智能选择编码格式
    QMediaFormat format;

    // 检查系统是否支持 MP3，如果不支持则退而求其次使用 AAC 或 PCM
    if (format.supportedAudioCodecs(QMediaFormat::Encode).contains(QMediaFormat::
                                                                   AudioCodec::MP3))
    {
        format.setAudioCodec(QMediaFormat::AudioCodec::MP3);
        format.setFileFormat(QMediaFormat::MP3);
    } else {
        // 这是一个万能的备选方案：通常系统都支持 AAC
        format.setAudioCodec(QMediaFormat::AudioCodec::AAC);
        format.setFileFormat(QMediaFormat::MPEG4);
    }

    recorder->setMediaFormat(format);

    // 3. 设置采样率、比特率等（可选）
    recorder->setAudioSampleRate(44100);
    recorder->setAudioBitRate(128000); // 128 kbps
    recorder->setAudioChannelCount(2); // 立体声

    // 4. 开始录制
    recorder->setOutputLocation(QUrl::fromLocalFile("test_audio.mp3"));
    recorder->record();
}

void widegtFFT::testAudioDecoderERROR(QAudioDecoder::Error error)
{
    QAudioDecoder *p = (QAudioDecoder *)sender();

    qDebug() << error << p->errorString();
}

// audio 解码
void widegtFFT::testAudioDecoder()
{
    QMediaFormat mf;

    qDebug().noquote() << "File Encode:\n" <<
        mf.supportedFileFormats(QMediaFormat::Encode);
    qDebug().noquote() << "File Decode:\n" <<
        mf.supportedFileFormats(QMediaFormat::Decode);
    qDebug().noquote() << "Audio Encode:\n" <<
        mf.supportedAudioCodecs(QMediaFormat::Encode);
    qDebug().noquote() << "Audio Encode:\n" <<
        mf.supportedAudioCodecs(QMediaFormat::Decode);
    qDebug().noquote() << "Video Encode:\n" <<
        mf.supportedVideoCodecs(QMediaFormat::Encode);
    qDebug().noquote() << "Video Encode:\n" <<
        mf.supportedVideoCodecs(QMediaFormat::Decode);

    QEventLoop loop;
    QAudioDecoder audioDecoder;
    QMediaDevices devices;
    QAudioFormat  desiredFormat = devices.defaultAudioOutput().preferredFormat();

    desiredFormat.setChannelCount(2);
    desiredFormat.setSampleRate(48000);
    desiredFormat.setSampleFormat(QAudioFormat::Int16);

    audioDecoder.setAudioFormat(desiredFormat);

    audioDecoder.setSource(QUrl::fromLocalFile("./20260528-204559.wav"));

    // audioDecoder.setSource(QUrl::fromLocalFile("20260528-204552.pcm"));

    // QAudioSource as(QAudioSource(devices.defaultAudioInput()));
    // QIODevice *io = as.start();
    // audioDecoder.setSourceDevice(io);

    QObject::connect(&audioDecoder,
                     &QAudioDecoder::finished,
                     &loop,
                     &QEventLoop::quit);
    QObject::connect(&audioDecoder,
                     SIGNAL(error(QAudioDecoder::Error)),
                     this,
                     SLOT(testAudioDecoderERROR(QAudioDecoder::Error)));
    QByteArray data;
    QBuffer    buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    QObject::connect(&audioDecoder, &QAudioDecoder::bufferReady,
                     [&buffer, &audioDecoder]() {
        const QAudioBuffer& frame = audioDecoder.read();
        buffer.write(frame.constData<char>(), frame.byteCount()); // 向IO设备中写入音频数据
    });
    audioDecoder.start();
    qDebug() << audioDecoder.audioFormat()
             << audioDecoder.source()
             << audioDecoder.isDecoding()
             << audioDecoder.errorString();

    if (audioDecoder.isDecoding()) {
        loop.exec();
    }

    buffer.close();
    qDebug() << buffer.size();

    buffer.open(QIODevice::ReadOnly);
    QAudioSink sink(desiredFormat);
    connect(&sink, &QAudioSink::stateChanged, this, [&loop](QAudio::State state) {
        qDebug() << state;

        if (QAudio::ActiveState != state) {
            loop.quit();
        }
    });
    sink.start(&buffer);
    loop.exec();
}

widegtFFT::widegtFFT(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widegtFFT)
{
    ui->setupUi(this);

    // testAudioDecoder();

    m_audioObject = new audioObject;
    thread = new QThread(this);
    m_audioObject->moveToThread(thread);

    m_udpObject.bindAll(12342);

    // connect(&m_udpObject,&udpObject::readData,this,[=](QByteArray data){});

    connect(m_audioObject, &audioObject::audioSourceStatus, this,
            [ = ](bool isopen) {
        m_audioIsOpen = isopen;
    });

    connect(m_audioObject, &audioObject::readData, this, &widegtFFT::readData);
    connect(m_audioObject, &audioObject::fftwData, this, &widegtFFT::fftwData);

    thread->start();

    init();
    initPlot();

    connect(this,           &widegtFFT::writeAudioSig,
            m_audioObject, &audioObject::writeAudioData);

    connect(ui->pushButton, &QPushButton::clicked, this, [ = ]() {
        if (m_audioIsOpen) return;

        QString fileName = QFileDialog::getOpenFileName(this,
                                                        QStringLiteral("打开"),
                                                        "./",
                                                        QStringLiteral("(*)"));

        if (fileName.isEmpty()) return;

        QFile f(fileName);

        if (!f.open(QIODevice::ReadOnly)) {
            qDebug() << f.errorString();
            return;
        }

        m_xcount = 0;
        m_xcount1 = 0;
        m_customPlotCurve->m_customPlot->graph(
            0)->data().data()->clear();
        m_customPlotCurveSData->m_customPlot->graph(
            0)->data().data()->clear();

        QByteArray data = f.readAll();

        QVector<double>dsdata;
        QVector<double>fftdata;
        QVector<double>radiandata;
        int channalCount = ui->spinBox->value();
        int byteRate = ui->spinBox2->value() / 8;
        m_AudioSourceSample = ui->spinBox3->value();
        m_windowSize = 2048;

        fftw3Object::signaType bitType = fftw3Object::INT16;

        if (byteRate == 1) {
            bitType = fftw3Object::UINT8;
        } else if (byteRate == 4) {
            bitType = fftw3Object::FLOAT;
        } else {
            bitType = fftw3Object::INT16;
        }

        fftw3Object::fft(data,
                         dsdata,
                         fftdata,
                         radiandata,
                         channalCount,
                         bitType,
                         m_windowSize,
                         m_windowSize / 2,
                         ui->checkBoxIsLittle->isChecked());

        {
            QString strtime = fileName.split("/").last() + " 时长: ";
            qint64 times = data.size() /
                           (1.0 * byteRate * m_AudioSourceSample * channalCount) * 1000;
            strtime += QTime::fromMSecsSinceStartOfDay(times)
                       .toString("hh:mm:ss.zzz");
            ui->label->setText(strtime);

            // QMediaDevices devices;
            // QAudioFormat format =
            // devices.defaultAudioOutput().preferredFormat();
            QAudioFormat format;
            format.setChannelCount(channalCount);
            format.setSampleRate(m_AudioSourceSample);

            if (byteRate == 2) {
                format.setSampleFormat(QAudioFormat::Int16);
            } else if (byteRate == 4) {
                format.setSampleFormat(QAudioFormat::Float);
            } else {
                format.setSampleFormat(QAudioFormat::UInt8);
            }
#if 1

            // 播放pcm
            emit writeAudioSig(data, format);
#else // if 1
            QAudioSink audioSink(format);
            QBuffer buffer(&data);
            buffer.open(QIODevice::ReadOnly);
            QEventLoop loop;
            connect(&audioSink, &QAudioSink::stateChanged, this,
                    [&loop](QAudio::State state) {
                if (QAudio::ActiveState != state) {
                    loop.quit();
                }
            });
            audioSink.start(&buffer);
            loop.exec();

#endif // if 1
        }

        readData(data);
        QList<QVector<double> >_fftdata;
        _fftdata.append(fftdata);
        QList<QVector<double> >_radiandata;
        _radiandata.append(radiandata);
        fftwData(dsdata, _fftdata, _radiandata);
    });

    connect(ui->checkBox, &QCheckBox::checkStateChanged, this, [ = ]() {
        if (ui->checkBox->isChecked()) {
            ui->checkBoxsavepcm->setEnabled(false);

            if (!m_audioIsOpen) return;

            if (ui->checkBoxsavepcm->isChecked()) {
                m_saveFileName =
                    QApplication::applicationDirPath() + "/" +
                    QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")
                    + ".pcm";
                m_savePCMFile.setFileName(m_saveFileName);

                if (!m_savePCMFile.open(QIODevice::WriteOnly)) {
                    qDebug() << m_savePCMFile.errorString();
                }
            } else {
                m_saveFileName =
                    QApplication::applicationDirPath() + "/" +
                    QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")
                    + ".wav";
                int bitRate = 8;
                QAudioFormat::SampleFormat fmat = m_format.sampleFormat();

                switch (fmat) {
                case QAudioFormat::SampleFormat::UInt8:
                    bitRate = 8;
                    break;

                case QAudioFormat::SampleFormat::Int16:
                    bitRate = 16;
                    break;

                case QAudioFormat::SampleFormat::Int32:
                    bitRate = 32;
                    break;

                case QAudioFormat::SampleFormat::Float:
                    bitRate = 32;
                    break;

                default:
                    break;
                }
                m_ffmpegObject.saveToWavStart(m_saveFileName,
                                              m_format.sampleRate(),
                                              bitRate,
                                              m_format.channelCount());
            }
            ui->label->setText(m_saveFileName);
        } else {
            if (ui->checkBoxsavepcm->isChecked()) {
                m_savePCMFile.close();
            } else {
                m_ffmpegObject.saveToWavEnd();
            }

            ui->checkBoxsavepcm->setEnabled(true);
        }
    });

    // FfmpegObject::getInfo();
}

widegtFFT::~widegtFFT()
{
    thread->quit();
    thread->wait();
    delete thread;

    // delete m_audioObject;
    m_audioObject->deleteLater();
    delete ui;

    delete m_genSignalWidget;
}

void widegtFFT::readData(QByteArray data)
{
    if (checkBox4->isChecked()) {
#if 0
        double max = data[0], min = data[0];

        for (int i = 0; i < data.size(); i++)
        {
            if (max < data[i]) max = data[i];

            if (min > data[i]) min = data[i];
        }
        m_customPlotCurveSData->m_customPlot->yAxis->setRange(min,
                                                              max);
#endif // if 0

        if (m_xcount1 < 0) m_xcount1 = 0;

        if ((m_xcount1 > m_maxPointCount) && (m_xcount > spinBox2->value())) {
            m_customPlotCurveSData->m_customPlot->graph(
                0)->data().data()->clear();
            m_xcount1 = 0;
        }

        for (int i = 0; i < data.size(); i++) {
            m_customPlotCurveSData->m_customPlot->graph(0)->
            addData(m_xcount1++, data[i]);
        }

        if (m_dateTime4.msecsTo(
                QDateTime::currentDateTime()) > m_plotFlushTime) {
            m_dateTime4 = QDateTime::currentDateTime();
            m_customPlotCurveSData->m_customPlot->xAxis->
            setRange(m_xcount1 - spinBox2->value(), m_xcount1);
            m_customPlotCurveSData->m_customPlot->replot();
        }
    }

    {
        // 数据速度
        static QDateTime datetime = QDateTime::currentDateTime();
        static qint64    _size = 0;
        _size += data.size();
        qint64 timeout = datetime.msecsTo(QDateTime::currentDateTime());

        if (timeout > 999) {
            QString str;
            static const int stek = 1024;
            static const int stem = stek * stek;
            static const int steg = stek * stek * stek;

            if (_size < stek) {
                double val = _size;
                str = QString::number(val, 'f', 2) + "B/s";
            } else if (_size < stem) {
                double val = 1.0 * _size / stek;
                str = QString::number(val, 'f', 2) + "KB/s";
            } else if (_size < steg) {
                double val = 1.0 * _size / stem;
                str = QString::number(val, 'f', 2) + "MB/s";
            } else {
                double val = 1.0 * _size / steg;
                str = QString::number(val, 'f', 2) + "GB/s";
            }

            ui->labelspeed->setText(str);

            datetime = QDateTime::currentDateTime();
            _size = 0;
        }
    }

    m_udpObject.writeData(data, QHostAddress("127.0.0.1"), 12345);

    {
        if (ui->checkBox->isChecked()) {
            if (ui->checkBoxsavepcm->isChecked()) {
                if (m_savePCMFile.isOpen()) m_savePCMFile.write(data);
            } else {
                m_ffmpegObject.saveToWav(data);
            }
        }
    }
}

void widegtFFT::fftwData(QVector<double>        dsdata,
                         QList<QVector<double> >fftdata,
                         QList<QVector<double> >radiandata)
{
    if ((dsdata.size() < 1) || (fftdata.size() < 1) || (radiandata.size() < 1) ||
        (fftdata[fftdata.size() - 1].size() < 1) ||
        (radiandata[radiandata.size() - 1].size() < 1)) return;

    if (checkBox1->isChecked()) {
#if 0
        double max = dsdata[0], min = dsdata[0];

        for (int i = 0; i < dsdata.size(); i++)
        {
            if (max < dsdata[i]) max = dsdata[i];

            if (min > dsdata[i]) min = dsdata[i];
        }
        m_customPlotCurve->m_customPlot->yAxis->setRange(min,
                                                         max);
#endif // if 1

        if (m_xcount < 0) m_xcount = 0;

        if ((m_xcount > m_maxPointCount) && (m_xcount > spinBox1->value())) {
            m_customPlotCurve->m_customPlot->graph(0)->data().data()->clear();
            m_xcount = 0;
        }

        for (int i = 0; i < dsdata.size(); i++) {
            m_customPlotCurve->m_customPlot->graph(0)->
            addData(m_xcount++, dsdata[i]);
        }

        if (m_dateTime1.msecsTo(
                QDateTime::currentDateTime()) > m_plotFlushTime) {
            m_dateTime1 = QDateTime::currentDateTime();
            m_customPlotCurve->m_customPlot->xAxis->
            setRange(m_xcount - spinBox1->value(), m_xcount);
            m_customPlotCurve->m_customPlot->replot();
        }
    }

    if (checkBox2->isChecked() &&
        (m_dateTime2.msecsTo(
             QDateTime::currentDateTime()) > m_plotFlushTime)) {
        m_dateTime2 = QDateTime::currentDateTime();
        double max = fftdata[fftdata.size() - 1][0],
               min = fftdata[fftdata.size() - 1][0];

        for (int i = 0; i < fftdata[fftdata.size() - 1].size(); i++)
        {
            if (max < fftdata[fftdata.size() - 1][i]) {
                max = fftdata[fftdata.size() - 1][i];
            }

            if (min > fftdata[fftdata.size() - 1][i]) {
                min = fftdata[fftdata.size() - 1][i];
            }
        }

        bool ishow = true;

        if (checkBox22->isChecked()) {
            if (max < doubleSpinBox22->value()) {
                ishow = false;
            }
        }

        if (ishow) {
            // if (max < 0.1) max = 1;
            // if (min > 0) min = 0;
            // m_customPlotCurveFFT->m_customPlot->yAxis->setRange(min,
            //                                                     max);
            m_customPlotCurveFFT->m_customPlot->graph(
                0)->data().data()->clear();
            m_customPlotCurveFFT->m_customPlot->xAxis->
            setRange(0, fftdata[fftdata.size() - 1].size());

#ifdef USE_TWO_XZHOU
            {
                double xmin = 0, xmax = 1;
                int    _count = fftdata[fftdata.size() - 1].size();

                for (int i = 0; i < _count; i++) {
                    double sample = m_AudioSourceSample; // ui->spinBox01->value()
                    double fs = 1.0 * i * sample / (m_windowSize / 2.0);
                    fs /= 2;

                    if (i == 0) xmin = fs;

                    if (i == _count - 1) xmax = fs;
                }
                m_customPlotCurveFFT->m_customPlot->axisRect()->
                axis(QCPAxis::atBottom, 1)->setRange(xmin, xmax);
            }
#endif // ifdef USE_TWO_XZHOU

            for (int i = 0; i < fftdata[fftdata.size() - 1].size(); i++) {
                m_customPlotCurveFFT->m_customPlot->graph(0)->
                addData(i, fftdata[fftdata.size() - 1][i]);
            }

            m_customPlotCurveFFT->m_customPlot->replot();
        }
    }

    if ((checkBox3->isChecked() || checkBox5->isChecked()) &&
        (m_dateTime3.msecsTo(
             QDateTime::currentDateTime()) > m_plotFlushTime)) {
        m_dateTime3 = QDateTime::currentDateTime();
        double max = 20 * log10(fftdata[fftdata.size() - 1][0]) + 1e-12,
               min = 20 * log10(fftdata[fftdata.size() - 1][0]) + 1e-12;

        for (int i = 0; i < fftdata[fftdata.size() - 1].size(); i++)
        {
            fftdata[fftdata.size() - 1][i] =
                20 * log10(fftdata[fftdata.size() - 1][i]) + 1e-12;

            if (max < fftdata[fftdata.size() - 1][i]) {
                max = fftdata[fftdata.size() - 1][i];
            }

            if (min > fftdata[fftdata.size() - 1][i]) {
                min = fftdata[fftdata.size() - 1][i];
            }
        }
        bool ishow1 = true;

        if (checkBox23->isChecked()) {
            if (max < doubleSpinBox23->value()) {
                ishow1 = false;
            }
        }

        if (!checkBox3->isChecked()) {
            ishow1 = false;
        }

        bool ishow2 = true;

        if (checkBox25->isChecked()) {
            if (max < doubleSpinBox25->value()) {
                ishow2 = false;
            }
        }

        if (!checkBox5->isChecked()) {
            ishow2 = false;
        }


        {
            // if (max < 0) max = 0;
            // if (min > -160) min = -160;
            // m_customPlotCurveFFTDb->m_customPlot->yAxis->setRange(min, max);
            if (ishow1) m_customPlotCurveFFTDb->m_customPlot->graph(
                    0)->data().data()->clear();

            // m_lineChartWidget->getLineSeries()->clear();
            // m_lineChartWidget->getAxisY()->setRange(min, max);

            double xmin = 0, xmax = 1;
            int    _count = fftdata[fftdata.size() - 1].size();

            QList<QPointF> points;

            for (int i = 0; i < _count; i++) {
                double sample = m_AudioSourceSample; // ui->spinBox01->value()
                double fs = 1.0 * i * sample / (m_windowSize / 2.0);
                fs /= 2;

                if (i == 0) xmin = fs;

                if (i == _count - 1) xmax = fs;

                if (ishow1) m_customPlotCurveFFTDb->m_customPlot->graph(0)->
                    addData(fs, fftdata[fftdata.size() - 1][i]);

                if (ishow2) points.append(QPointF(fs,
                                                  fftdata[fftdata.size() -
                                                          1][i]));
            }

            if (ishow2) {
                m_lineChartWidget->replace(points);
                m_lineChartWidget->getAxisX()->setRange(xmin, xmax);
            }

            if (ishow1) {
                m_customPlotCurveFFTDb->m_customPlot->xAxis->setRange(xmin, xmax);
                m_customPlotCurveFFTDb->m_customPlot->replot();
            }
        }
    }

    if (checkBox6->isChecked() &&
        (m_dateTime5.msecsTo(QDateTime::currentDateTime()) > m_plotFlushTime)) {
        m_dateTime5 = QDateTime::currentDateTime();
        m_customPlotCurveRadian->m_customPlot->
        graph(0)->data().data()->clear();

        double xmin = 0, xmax = 1;
        int    _count = radiandata[radiandata.size() - 1].size();

        QList<QPointF> points;

        double tmp = 180 / M_PI;

        for (int i = 0; i < _count; i++) {
            double sample = m_AudioSourceSample;
            double fs = 1.0 * i * sample / (m_windowSize / 2.0);
            fs /= 2;

            if (i == 0) xmin = fs;

            if (i == _count - 1) xmax = fs;

            m_customPlotCurveRadian->m_customPlot->graph(0)->
            addData(fs, radiandata[radiandata.size() - 1][i] * tmp);
        }
        m_customPlotCurveRadian->m_customPlot->xAxis->setRange(xmin, xmax);
        m_customPlotCurveRadian->m_customPlot->replot();
    }
}

void widegtFFT::initPlot() {
    m_customPlotCurve = new customPlotCurve(this);
    m_customPlotCurveFFT = new customPlotCurve(this);
    m_customPlotCurveFFTDb = new customPlotCurve(this);
    m_customPlotCurveRadian = new customPlotCurve(this);
    m_customPlotCurveSData = new customPlotCurve(this);
    m_lineChartWidget = new LineChartWidget(this);
    m_lineChartWidget->getLineSeries()->setName("db/频率");
    m_lineChartWidget->getAxisX()->setTitleText("频率");
    m_lineChartWidget->getAxisY()->setTitleText("db");

    checkBox1 = new QCheckBox("自动刷新", this);
    checkBox2 = new QCheckBox("自动刷新", this);
    checkBox3 = new QCheckBox("自动刷新", this);
    checkBox4 = new QCheckBox("自动刷新", this);
    checkBox5 = new QCheckBox("自动刷新", this);
    checkBox6 = new QCheckBox("自动刷新", this);

    QCheckBox *checkBox11 =  new QCheckBox("显示追踪线", this);
    QCheckBox *checkBox12 =  new QCheckBox("显示追踪线", this);
    QCheckBox *checkBox13 =  new QCheckBox("显示追踪线", this);
    QCheckBox *checkBox14 =  new QCheckBox("显示追踪线", this);
    QCheckBox *checkBox15 =  new QCheckBox("显示追踪线", this);
    QLabel    *label1 = new QLabel("量程长度：", this);
    QLabel    *label2 = new QLabel("量程长度：", this);
    spinBox1 = new QSpinBox(this);
    spinBox2 = new QSpinBox(this);
    spinBox1->setRange(0, 0x7fffffff);
    spinBox2->setRange(0, 0x7fffffff);
    spinBox1->setValue(1000);
    spinBox2->setValue(1000);

    checkBox21 = new QCheckBox("设置阈值", this);
    checkBox22 = new QCheckBox("设置阈值", this);
    checkBox23 = new QCheckBox("设置阈值", this);
    checkBox24 = new QCheckBox("设置阈值", this);
    checkBox25 = new QCheckBox("设置阈值", this);
    doubleSpinBox21 = new QDoubleSpinBox(this);
    doubleSpinBox22 = new QDoubleSpinBox(this);
    doubleSpinBox23 = new QDoubleSpinBox(this);
    doubleSpinBox24 = new QDoubleSpinBox(this);
    doubleSpinBox25 = new QDoubleSpinBox(this);
    doubleSpinBox21->setRange(-9999999999, 9999999999);
    doubleSpinBox22->setRange(-9999999999, 9999999999);
    doubleSpinBox23->setRange(-9999999999, 9999999999);
    doubleSpinBox24->setRange(-9999999999, 9999999999);
    doubleSpinBox25->setRange(-9999999999, 9999999999);
    doubleSpinBox21->setDecimals(16);
    doubleSpinBox22->setDecimals(16);
    doubleSpinBox23->setDecimals(16);
    doubleSpinBox24->setDecimals(16);
    doubleSpinBox25->setDecimals(16);
    doubleSpinBox21->setValue(0);
    doubleSpinBox22->setValue(0.001);
    doubleSpinBox23->setValue(-60);
    doubleSpinBox24->setValue(0);
    doubleSpinBox25->setValue(-60);
    doubleSpinBox25->setValue(0);

    checkBox21->hide();
    doubleSpinBox21->hide();
    checkBox24->hide();
    doubleSpinBox24->hide();

    QHBoxLayout *hboxLayout1 = new QHBoxLayout;
    hboxLayout1->setContentsMargins(0, 0, 0, 0);
    hboxLayout1->addWidget(checkBox1);
    hboxLayout1->addWidget(checkBox11);
    hboxLayout1->addWidget(label1);
    hboxLayout1->addWidget(spinBox1);
    hboxLayout1->addWidget(checkBox21);
    hboxLayout1->addWidget(doubleSpinBox21);
    hboxLayout1->addStretch();
    QHBoxLayout *hboxLayout2 = new QHBoxLayout;
    hboxLayout2->setContentsMargins(0, 0, 0, 0);
    hboxLayout2->addWidget(checkBox2);
    hboxLayout2->addWidget(checkBox12);
    hboxLayout2->addWidget(checkBox22);
    hboxLayout2->addWidget(doubleSpinBox22);
    hboxLayout2->addStretch();
    QHBoxLayout *hboxLayout3 = new QHBoxLayout;
    hboxLayout3->setContentsMargins(0, 0, 0, 0);
    hboxLayout3->addWidget(checkBox3);
    hboxLayout3->addWidget(checkBox13);
    hboxLayout3->addWidget(checkBox23);
    hboxLayout3->addWidget(doubleSpinBox23);
    hboxLayout3->addStretch();
    QHBoxLayout *hboxLayout4 = new QHBoxLayout;
    hboxLayout4->setContentsMargins(0, 0, 0, 0);
    hboxLayout4->addWidget(checkBox4);
    hboxLayout4->addWidget(checkBox14);
    hboxLayout4->addWidget(label2);
    hboxLayout4->addWidget(spinBox2);
    hboxLayout4->addWidget(checkBox24);
    hboxLayout4->addWidget(doubleSpinBox24);
    hboxLayout4->addStretch();
    QHBoxLayout *hboxLayout5 = new QHBoxLayout;
    hboxLayout5->setContentsMargins(0, 0, 0, 0);
    hboxLayout5->addWidget(checkBox5);
    hboxLayout5->addWidget(checkBox25);
    hboxLayout5->addWidget(doubleSpinBox25);
    hboxLayout5->addStretch();
    QHBoxLayout *hboxLayout6 = new QHBoxLayout;
    hboxLayout6->setContentsMargins(0, 0, 0, 0);
    hboxLayout6->addWidget(checkBox6);
    hboxLayout6->addWidget(checkBox15);
    hboxLayout6->addStretch();

    QWidget *widget1 = new QWidget(this);
    QWidget *widget2 = new QWidget(this);
    QWidget *widget3 = new QWidget(this);
    QWidget *widget4 = new QWidget(this);
    QWidget *widget5 = new QWidget(this);
    QWidget *widget6 = new QWidget(this);
    QVBoxLayout *vLayout1 = new QVBoxLayout(widget1);
    QVBoxLayout *vLayout2 = new QVBoxLayout(widget2);
    QVBoxLayout *vLayout3 = new QVBoxLayout(widget3);
    QVBoxLayout *vLayout4 = new QVBoxLayout(widget4);
    QVBoxLayout *vLayout5 = new QVBoxLayout(widget5);
    QVBoxLayout *vLayout6 = new QVBoxLayout(widget6);
    vLayout1->setContentsMargins(0, 0, 0, 0);
    vLayout1->addLayout(hboxLayout1);
    vLayout1->addWidget(m_customPlotCurve);
    vLayout2->setContentsMargins(0, 0, 0, 0);
    vLayout2->addLayout(hboxLayout2);
    vLayout2->addWidget(m_customPlotCurveFFT);
    vLayout3->setContentsMargins(0, 0, 0, 0);
    vLayout3->addLayout(hboxLayout3);
    vLayout3->addWidget(m_customPlotCurveFFTDb);
    vLayout4->setContentsMargins(0, 0, 0, 0);
    vLayout4->addLayout(hboxLayout4);
    vLayout4->addWidget(m_customPlotCurveSData);
    vLayout5->setContentsMargins(0, 0, 0, 0);
    vLayout5->addLayout(hboxLayout5);
    vLayout5->addWidget(m_lineChartWidget);
    vLayout6->setContentsMargins(0, 0, 0, 0);
    vLayout6->addLayout(hboxLayout6);
    vLayout6->addWidget(m_customPlotCurveRadian);

    QTabWidget *tabwidget = new QTabWidget(this);
    tabwidget->setContentsMargins(0, 0, 0, 0);
    tabwidget->addTab(widget1, "原数据");
    tabwidget->addTab(widget2, "FFT数据");
    tabwidget->addTab(widget3, "FFT显示DBFS/FS");
    tabwidget->addTab(widget6, "FFT显示弧度/FS");
    tabwidget->addTab(widget4, "原始采集数据(未转换)");
    tabwidget->addTab(widget5, "QChart");

    QVBoxLayout *mainLayout = new QVBoxLayout(ui->widget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(tabwidget);

    // m_customPlotCurve->setLabel("x", "y");
    m_customPlotCurve->m_customPlot->xAxis->setLabel("x");
    m_customPlotCurve->m_customPlot->yAxis->setLabel("y");
    m_customPlotCurve->m_customPlot->legend->setVisible(false);
    m_customPlotCurve->setShowTracer(true);
    m_customPlotCurve->addGraph("音频数据波形", QPen(QColor(255, 255, 255, 255)),
                                QBrush(QColor(122, 13, 232, 200)));

#ifdef USE_TWO_XZHOU

    // 添加1个X轴
    m_customPlotCurveFFT->m_customPlot->axisRect()->addAxes(QCPAxis::atBottom);
    m_customPlotCurveFFT->m_customPlot->axisRect()->
    axis(QCPAxis::atBottom, 0)->setPadding(0);
    m_customPlotCurveFFT->m_customPlot->axisRect()->
    axis(QCPAxis::atBottom, 1)->setLabel("频率(Hz)");
#endif // ifdef USE_TWO_XZHOU
    // m_customPlotCurveFFT->m_customPlot->yAxis->setPadding(100);
    // m_customPlotCurveFFT->m_customPlot->yAxis->setLabelPadding(10);
    m_customPlotCurveFFT->m_customPlot->xAxis->setLabel("x");
    m_customPlotCurveFFT->m_customPlot->yAxis->setLabel("y");
    m_customPlotCurveFFT->m_customPlot->legend->setVisible(false);
    m_customPlotCurveFFT->setShowTracer(true);
    m_customPlotCurveFFT->addGraph("音频数据FFT波形", QPen(QColor(0, 255, 255, 255)),
                                   QBrush(QColor(0, 0, 255, 0)));

    m_customPlotCurveFFTDb->m_customPlot->xAxis->setLabel("fs");
    m_customPlotCurveFFTDb->m_customPlot->yAxis->setLabel("dbfs");
    m_customPlotCurveFFTDb->m_customPlot->legend->setVisible(false);
    m_customPlotCurveFFTDb->setShowTracer(true);
    m_customPlotCurveFFTDb->addGraph("音频数据DBFS波形",
                                     QPen(QColor(255, 255, 0, 255)),
                                     QBrush(QColor(0, 0, 255, 0)));

    m_customPlotCurveRadian->m_customPlot->xAxis->setLabel("频率");
    m_customPlotCurveRadian->m_customPlot->yAxis->setLabel("弧度");
    m_customPlotCurveRadian->m_customPlot->legend->setVisible(false);
    m_customPlotCurveRadian->setShowTracer(true);
    m_customPlotCurveRadian->addGraph("音频数据弧度波形",
                                      QPen(QColor(123, 231, 10, 255)),
                                      QBrush(QColor(0, 0, 255, 0)));

    m_customPlotCurveSData->m_customPlot->xAxis->setLabel("x");
    m_customPlotCurveSData->m_customPlot->yAxis->setLabel("y");
    m_customPlotCurveSData->m_customPlot->legend->setVisible(false);
    m_customPlotCurveSData->setShowTracer(true);
    m_customPlotCurveSData->addGraph("音频采集原始数据", QPen(Qt::blue),
                                     QBrush(QColor(0, 0, 255, 0)));

    connect(checkBox11, &QCheckBox::checkStateChanged, this, [ = ]() {
        m_customPlotCurve->setShowTracer(checkBox11->isChecked());
    });
    connect(checkBox12, &QCheckBox::checkStateChanged, this, [ = ]() {
        m_customPlotCurveFFT->setShowTracer(checkBox12->isChecked());
    });
    connect(checkBox13, &QCheckBox::checkStateChanged, this, [ = ]() {
        m_customPlotCurveFFTDb->setShowTracer(checkBox13->isChecked());
    });
    connect(checkBox14, &QCheckBox::checkStateChanged, this, [ = ]() {
        m_customPlotCurveSData->setShowTracer(checkBox14->isChecked());
    });
    connect(checkBox15, &QCheckBox::checkStateChanged, this, [ = ]() {
        m_customPlotCurveRadian->setShowTracer(checkBox15->isChecked());
    });
    checkBox11->setChecked(true);
    checkBox12->setChecked(true);
    checkBox13->setChecked(true);
    checkBox14->setChecked(true);
    checkBox15->setChecked(true);

    m_genSignalWidget = new genSignalWidget;

    // m_genSignalWidget->show();
    connect(ui->pushButton_2, &QPushButton::clicked, this, [ = ]() {
        if (m_genSignalWidget->isHidden()) {
            m_genSignalWidget->show();
        }
    });

    connect(m_genSignalWidget, &genSignalWidget::updateData, this, [ = ](
                QByteArray data,
                int        channalCount,
                int        audioSample,
                int        byteType,
                bool       isLittle,
                int        fftwindow,
                bool       isupdate)
    {
        if (m_audioIsOpen) return;

        fftw3Object::signaType _bitType = (fftw3Object::signaType)byteType;
        int byteRate = 1;

        switch (_bitType) {
        case fftw3Object::UINT8:
        case fftw3Object::INT8:
            byteRate = 1;
            break;

        case fftw3Object::UINT16:
        case fftw3Object::INT16:
            byteRate = 2;
            break;

        case fftw3Object::UINT32:
        case fftw3Object::INT32:
        case fftw3Object::FLOAT:
            byteRate = 4;
            break;

        default:
            break;
        }

        if (!isupdate) {
            m_xcount = 0;
            m_xcount1 = 0;
            m_customPlotCurve->m_customPlot->graph(
                0)->data().data()->clear();
            m_customPlotCurveSData->m_customPlot->graph(
                0)->data().data()->clear();
        }

        QVector<double>dsdata;
        QVector<double>fftdata;
        QVector<double>radiandata;
        m_AudioSourceSample = audioSample;
        m_windowSize = fftwindow;
        QString strtime;

        {
            // save
            if (ui->checkBox->isChecked() && !isupdate) {
                QString FileName =
                    QApplication::applicationDirPath() + "/" +
                    QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");

                if (ui->checkBoxsavepcm->isChecked()) {
                    FileName += ".pcm";
                    QFile saveFile(FileName);

                    if (saveFile.open(QIODevice::WriteOnly)) {
                        saveFile.write(data);
                    } else {
                        qDebug() << saveFile.errorString();
                    }
                } else {
// if(isLittle == false)// error;
                    FileName += ".wav";
                    int bitRate = 8 * byteRate;
                    FfmpegObject fmo;
                    fmo.saveToWavStart(FileName,
                                       audioSample, bitRate, channalCount);
                    fmo.saveToWav(data);
                    fmo.saveToWavEnd();
                }
                ui->label->setText(FileName);
                strtime = FileName.split("/").last();
            }
        }

        strtime += " 时长: ";
        qint64 times = data.size() /
                       (1.0 * byteRate * m_AudioSourceSample * channalCount) * 1000;
        strtime += QTime::fromMSecsSinceStartOfDay(times)
                   .toString("hh:mm:ss.zzz");
        ui->label->setText(strtime);

        fftw3Object::fft(data,
                         dsdata,
                         fftdata,
                         radiandata,
                         channalCount,
                         _bitType,
                         m_windowSize,
                         m_windowSize / 2,
                         isLittle);

        readData(data);
        QList<QVector<double> >_fftdata;
        _fftdata.append(fftdata);
        QList<QVector<double> >_radiandata;
        _radiandata.append(radiandata);
        fftwData(dsdata, _fftdata, _radiandata);

        if (0) {
            QAudioFormat format;
            format.setChannelCount(channalCount);
            format.setSampleRate(m_AudioSourceSample);

            if (byteRate == 2) {
                format.setSampleFormat(QAudioFormat::Int16);
            } else if (byteRate == 4) {
                format.setSampleFormat(QAudioFormat::Float);
            } else {
                format.setSampleFormat(QAudioFormat::UInt8);
            }

            // 播放pcm
            emit writeAudioSig(data, format);
        }
    });
}

void widegtFFT::init()
{
    connect(ui->comboBox01, &QComboBox::currentTextChanged, this,
            [ = ](const QString& text) {
        for (int i = 0; i < QMediaDevices::audioInputs().size(); i++) {
            if (text == QMediaDevices::audioInputs().at(i).description()) {
                ui->comboBox02->clear();
                const QAudioDevice& dev = QMediaDevices::audioInputs().at(
                    i);
                const QAudioFormat& format = dev.preferredFormat();
                QList<QAudioFormat::SampleFormat>sfmat = dev.supportedSampleFormats();

                for (int j = 0; j < sfmat.size(); j++) {
                    switch (sfmat[j]) {
                    case QAudioFormat::Unknown:
                        ui->comboBox02->addItem("Unknown");
                        break;

                    case QAudioFormat::UInt8:
                        ui->comboBox02->addItem("UInt8");
                        break;

                    case QAudioFormat::Int16:
                        ui->comboBox02->addItem("Int16");
                        break;

                    case QAudioFormat::Int32:
                        ui->comboBox02->addItem("Int32");
                        break;

                    case QAudioFormat::Float:
                        ui->comboBox02->addItem("Float");
                        break;

                    case QAudioFormat::NSampleFormats:
                        ui->comboBox02->addItem("NSampleFormats");
                        break;

                    default:
                        break;
                    }
                }

                switch (format.sampleFormat()) {
                case QAudioFormat::Unknown:
                    ui->comboBox02->setCurrentText("Unknown");
                    break;

                case QAudioFormat::UInt8:
                    ui->comboBox02->setCurrentText("UInt8");
                    break;

                case QAudioFormat::Int16:
                    ui->comboBox02->setCurrentText("Int16");
                    break;

                case QAudioFormat::Int32:
                    ui->comboBox02->setCurrentText("Int32");
                    break;

                case QAudioFormat::Float:
                    ui->comboBox02->setCurrentText("Float");
                    break;

                case QAudioFormat::NSampleFormats:
                    ui->comboBox02->setCurrentText("NSampleFormats");
                    break;

                default:
                    break;
                }

                ui->comboBox03->clear();
                ui->comboBox03->addItem("ChannelConfigUnknown");
                ui->comboBox03->addItem("ChannelConfigMono");
                ui->comboBox03->addItem("ChannelConfigStereo");
                ui->comboBox03->addItem("ChannelConfig2Dot1");
                ui->comboBox03->addItem("ChannelConfig3Dot0");
                ui->comboBox03->addItem("ChannelConfig3Dot1");
                ui->comboBox03->addItem("ChannelConfigSurround5Dot0");
                ui->comboBox03->addItem("ChannelConfigSurround5Dot1");
                ui->comboBox03->addItem("ChannelConfigSurround7Dot0");
                ui->comboBox03->addItem("ChannelConfigSurround7Dot1");

                switch (format.channelConfig()) {
                case QAudioFormat::ChannelConfigUnknown:
                    ui->comboBox03->setCurrentText("ChannelConfigUnknown");
                    break;

                case QAudioFormat::ChannelConfigMono:
                    ui->comboBox03->setCurrentText("ChannelConfigMono");
                    break;

                case QAudioFormat::ChannelConfigStereo:
                    ui->comboBox03->setCurrentText("ChannelConfigStereo");
                    break;

                case QAudioFormat::ChannelConfig2Dot1:
                    ui->comboBox03->setCurrentText("ChannelConfig2Dot1");
                    break;

                case QAudioFormat::ChannelConfig3Dot0:
                    ui->comboBox03->setCurrentText("ChannelConfig3Dot0");
                    break;

                case QAudioFormat::ChannelConfig3Dot1:
                    ui->comboBox03->setCurrentText("ChannelConfig3Dot1");
                    break;

                case QAudioFormat::ChannelConfigSurround5Dot0:
                    ui->comboBox03->setCurrentText("ChannelConfigSurround5Dot0");
                    break;

                case QAudioFormat::ChannelConfigSurround5Dot1:
                    ui->comboBox03->setCurrentText("ChannelConfigSurround5Dot1");
                    break;

                case QAudioFormat::ChannelConfigSurround7Dot0:
                    ui->comboBox03->setCurrentText("ChannelConfigSurround7Dot0");
                    break;

                case QAudioFormat::ChannelConfigSurround7Dot1:
                    ui->comboBox03->setCurrentText("ChannelConfigSurround7Dot1");
                    break;

                default:
                    break;
                }

                ui->spinBox01->setMaximum(dev.maximumSampleRate());
                ui->spinBox01->setMinimum(dev.minimumSampleRate());
                ui->spinBox02->setMaximum(dev.maximumChannelCount());
                ui->spinBox02->setMinimum(dev.minimumChannelCount());
                ui->label_3->setText(QString("采样率(Hz):%1~%2").
                                     arg(ui->spinBox01->minimum()).
                                     arg(ui->spinBox01->maximum()));
                ui->label_4->setText(QString("通道数:%1~%2").
                                     arg(ui->spinBox02->minimum()).
                                     arg(ui->spinBox02->maximum()));
                ui->spinBox01->setValue(format.sampleRate());
                ui->spinBox02->setValue(format.channelCount());
                break;
            }
        }
    });

    connect(ui->comboBox11, &QComboBox::currentTextChanged, this,
            [ = ](const QString& text) {
        for (int i = 0; i < QMediaDevices::audioOutputs().size(); i++) {
            if (text == QMediaDevices::audioOutputs().at(i).description()) {
                ui->comboBox12->clear();
                const QAudioDevice& dev = QMediaDevices::audioOutputs().at(
                    i);
                const QAudioFormat& format = dev.preferredFormat();
                QList<QAudioFormat::SampleFormat>sfmat = dev.supportedSampleFormats();

                for (int j = 0; j < sfmat.size(); j++) {
                    switch (sfmat[j]) {
                    case QAudioFormat::Unknown:
                        ui->comboBox12->addItem("Unknown");
                        break;

                    case QAudioFormat::UInt8:
                        ui->comboBox12->addItem("UInt8");
                        break;

                    case QAudioFormat::Int16:
                        ui->comboBox12->addItem("Int16");
                        break;

                    case QAudioFormat::Int32:
                        ui->comboBox12->addItem("Int32");
                        break;

                    case QAudioFormat::Float:
                        ui->comboBox12->addItem("Float");
                        break;

                    case QAudioFormat::NSampleFormats:
                        ui->comboBox12->addItem("NSampleFormats");
                        break;

                    default:
                        break;
                    }
                }

                switch (format.sampleFormat()) {
                case QAudioFormat::Unknown:
                    ui->comboBox12->setCurrentText("Unknown");
                    break;

                case QAudioFormat::UInt8:
                    ui->comboBox12->setCurrentText("UInt8");
                    break;

                case QAudioFormat::Int16:
                    ui->comboBox12->setCurrentText("Int16");
                    break;

                case QAudioFormat::Int32:
                    ui->comboBox12->setCurrentText("Int32");
                    break;

                case QAudioFormat::Float:
                    ui->comboBox12->setCurrentText("Float");
                    break;

                case QAudioFormat::NSampleFormats:
                    ui->comboBox12->setCurrentText("NSampleFormats");
                    break;

                default:
                    break;
                }

                ui->comboBox13->clear();
                ui->comboBox13->addItem("ChannelConfigUnknown");
                ui->comboBox13->addItem("ChannelConfigMono");
                ui->comboBox13->addItem("ChannelConfigStereo");
                ui->comboBox13->addItem("ChannelConfig2Dot1");
                ui->comboBox13->addItem("ChannelConfig3Dot0");
                ui->comboBox13->addItem("ChannelConfig3Dot1");
                ui->comboBox13->addItem("ChannelConfigSurround5Dot0");
                ui->comboBox13->addItem("ChannelConfigSurround5Dot1");
                ui->comboBox13->addItem("ChannelConfigSurround7Dot0");
                ui->comboBox13->addItem("ChannelConfigSurround7Dot1");

                switch (format.channelConfig()) {
                case QAudioFormat::ChannelConfigUnknown:
                    ui->comboBox13->setCurrentText("ChannelConfigUnknown");
                    break;

                case QAudioFormat::ChannelConfigMono:
                    ui->comboBox13->setCurrentText("ChannelConfigMono");
                    break;

                case QAudioFormat::ChannelConfigStereo:
                    ui->comboBox13->setCurrentText("ChannelConfigStereo");
                    break;

                case QAudioFormat::ChannelConfig2Dot1:
                    ui->comboBox13->setCurrentText("ChannelConfig2Dot1");
                    break;

                case QAudioFormat::ChannelConfig3Dot0:
                    ui->comboBox13->setCurrentText("ChannelConfig3Dot0");
                    break;

                case QAudioFormat::ChannelConfig3Dot1:
                    ui->comboBox13->setCurrentText("ChannelConfig3Dot1");
                    break;

                case QAudioFormat::ChannelConfigSurround5Dot0:
                    ui->comboBox13->setCurrentText("ChannelConfigSurround5Dot0");
                    break;

                case QAudioFormat::ChannelConfigSurround5Dot1:
                    ui->comboBox13->setCurrentText("ChannelConfigSurround5Dot1");
                    break;

                case QAudioFormat::ChannelConfigSurround7Dot0:
                    ui->comboBox13->setCurrentText("ChannelConfigSurround7Dot0");
                    break;

                case QAudioFormat::ChannelConfigSurround7Dot1:
                    ui->comboBox13->setCurrentText("ChannelConfigSurround7Dot1");
                    break;

                default:
                    break;
                }

                ui->spinBox11->setMaximum(dev.maximumSampleRate());
                ui->spinBox11->setMinimum(dev.minimumSampleRate());
                ui->spinBox12->setMaximum(dev.maximumChannelCount());
                ui->spinBox12->setMinimum(dev.minimumChannelCount());
                ui->label_5->setText(QString("采样率(Hz):%1~%2").
                                     arg(ui->spinBox11->minimum()).
                                     arg(ui->spinBox11->maximum()));
                ui->label_6->setText(QString("通道数:%1~%2").
                                     arg(ui->spinBox12->minimum()).
                                     arg(ui->spinBox12->maximum()));
                ui->spinBox11->setValue(format.sampleRate());
                ui->spinBox12->setValue(format.channelCount());
                break;
            }
        }
    });

    connect(ui->pushButtonopen, &QPushButton::clicked, this,
            [ = ]() {
        QAudioFormat format;
        format.setSampleRate(ui->spinBox01->value());

        // format.setChannelCount(ui->spinBox02->value());

        if (ui->comboBox02->currentText() == "Unknown") {
            format.setSampleFormat(QAudioFormat::Unknown);
        } else if (ui->comboBox02->currentText() == "UInt8") {
            format.setSampleFormat(QAudioFormat::UInt8);
        } else if (ui->comboBox02->currentText() == "Int16") {
            format.setSampleFormat(QAudioFormat::Int16);
        } else if (ui->comboBox02->currentText() == "Int32") {
            format.setSampleFormat(QAudioFormat::Int32);
        } else if (ui->comboBox02->currentText() == "Float") {
            format.setSampleFormat(QAudioFormat::Float);
        } else if (ui->comboBox02->currentText() == "NSampleFormats") {
            format.setSampleFormat(QAudioFormat::NSampleFormats);
        } else {
            qDebug() << QString("not found format:%1").
            arg(ui->comboBox02->currentText());
            return;
        }

        if (ui->comboBox03->currentText() == "ChannelConfigUnknown") {
            format.setChannelConfig(QAudioFormat::ChannelConfigUnknown);
        } else if (ui->comboBox03->currentText() == "ChannelConfigMono") {
            format.setChannelConfig(QAudioFormat::ChannelConfigMono);
        } else if (ui->comboBox03->currentText() == "ChannelConfigStereo") {
            format.setChannelConfig(QAudioFormat::ChannelConfigStereo);
        } else if (ui->comboBox03->currentText() == "ChannelConfig2Dot1") {
            format.setChannelConfig(QAudioFormat::ChannelConfig2Dot1);
        } else if (ui->comboBox03->currentText() == "ChannelConfig3Dot0") {
            format.setChannelConfig(QAudioFormat::ChannelConfig3Dot0);
        } else if (ui->comboBox03->currentText() == "ChannelConfigSurround5Dot0") {
            format.setChannelConfig(QAudioFormat::ChannelConfigSurround5Dot0);
        } else if (ui->comboBox03->currentText() == "ChannelConfigSurround5Dot1") {
            format.setChannelConfig(QAudioFormat::ChannelConfigSurround5Dot1);
        } else if (ui->comboBox03->currentText() == "ChannelConfigSurround7Dot0") {
            format.setChannelConfig(QAudioFormat::ChannelConfigSurround7Dot0);
        } else if (ui->comboBox03->currentText() == "ChannelConfig3Dot1") {
            format.setChannelConfig(QAudioFormat::ChannelConfig3Dot1);
        } else if (ui->comboBox03->currentText() == "ChannelConfigSurround7Dot1") {
            format.setChannelConfig(QAudioFormat::ChannelConfigSurround7Dot1);
        } else {
            qDebug() << QString("not found ChannelConfig:%1").
            arg(ui->comboBox03->currentText());
            return;
        }

        // 与setChannelConfig冲突
        format.setChannelCount(ui->spinBox02->value());
        qsizetype i;
        QString   devname = ui->comboBox01->currentText();

        for (i = 0; i < QMediaDevices::audioInputs().size(); i++) {
            if (devname == QMediaDevices::audioInputs().at(i).description()) {
                break;
            }
        }

        if (i >= QMediaDevices::audioInputs().size()) {
            qDebug() << QString("not found device:%1").arg(devname);
            return;
        }
        QAudioDevice info(QMediaDevices::audioInputs().at(i));

        if (!info.isFormatSupported(format)) {
            qWarning() <<
            "Raw audio format not supported by backend, cannot play audio.";
            return;
        }

        m_format = format;
        m_AudioSourceSample = format.sampleRate();
        m_windowSize = m_audioObject->getFFTPointCount();
        m_audioObject->openSource(format, info, 1);

        m_customPlotCurve->m_customPlot->yAxis->setRange(-0.01, 0.01);
        m_customPlotCurveFFT->m_customPlot->yAxis->setRange(0, 0.1);
        m_customPlotCurveFFTDb->m_customPlot->yAxis->setRange(-160, 0);
        m_customPlotCurveRadian->m_customPlot->yAxis->setRange(-180,
                                                               180);
        m_customPlotCurveSData->m_customPlot->yAxis->setRange(-1000, 1000);
        m_lineChartWidget->getAxisY()->setRange(-160, 0);
    });

    connect(ui->pushButtonopen_2, &QPushButton::clicked, this,
            [ = ]() {
        QAudioFormat format;
        format.setSampleRate(ui->spinBox11->value());

        // format.setChannelCount(ui->spinBox12->value());

        if (ui->comboBox12->currentText() == "Unknown") {
            format.setSampleFormat(QAudioFormat::Unknown);
        } else if (ui->comboBox12->currentText() == "UInt8") {
            format.setSampleFormat(QAudioFormat::UInt8);
        } else if (ui->comboBox12->currentText() == "Int16") {
            format.setSampleFormat(QAudioFormat::Int16);
        } else if (ui->comboBox12->currentText() == "Int32") {
            format.setSampleFormat(QAudioFormat::Int32);
        } else if (ui->comboBox12->currentText() == "Float") {
            format.setSampleFormat(QAudioFormat::Float);
        } else if (ui->comboBox12->currentText() == "NSampleFormats") {
            format.setSampleFormat(QAudioFormat::NSampleFormats);
        } else {
            qDebug() << QString("not found format:%1").
            arg(ui->comboBox12->currentText());
            return;
        }

        if (ui->comboBox13->currentText() == "ChannelConfigUnknown") {
            format.setChannelConfig(QAudioFormat::ChannelConfigUnknown);
        } else if (ui->comboBox13->currentText() == "ChannelConfigMono") {
            format.setChannelConfig(QAudioFormat::ChannelConfigMono);
        } else if (ui->comboBox13->currentText() == "ChannelConfigStereo") {
            format.setChannelConfig(QAudioFormat::ChannelConfigStereo);
        } else if (ui->comboBox13->currentText() == "ChannelConfig2Dot1") {
            format.setChannelConfig(QAudioFormat::ChannelConfig2Dot1);
        } else if (ui->comboBox13->currentText() == "ChannelConfig3Dot0") {
            format.setChannelConfig(QAudioFormat::ChannelConfig3Dot0);
        } else if (ui->comboBox13->currentText() == "ChannelConfigSurround5Dot0") {
            format.setChannelConfig(QAudioFormat::ChannelConfigSurround5Dot0);
        } else if (ui->comboBox13->currentText() == "ChannelConfigSurround5Dot1") {
            format.setChannelConfig(QAudioFormat::ChannelConfigSurround5Dot1);
        } else if (ui->comboBox13->currentText() == "ChannelConfigSurround7Dot0") {
            format.setChannelConfig(QAudioFormat::ChannelConfigSurround7Dot0);
        } else if (ui->comboBox13->currentText() == "ChannelConfig3Dot1") {
            format.setChannelConfig(QAudioFormat::ChannelConfig3Dot1);
        } else if (ui->comboBox13->currentText() == "ChannelConfigSurround7Dot1") {
            format.setChannelConfig(QAudioFormat::ChannelConfigSurround7Dot1);
        } else {
            qDebug() << QString("not found ChannelConfig:%1").
            arg(ui->comboBox13->currentText());
            return;
        }

        format.setChannelCount(ui->spinBox12->value());
        qsizetype i;
        QString   devname = ui->comboBox11->currentText();

        for (i = 0; i < QMediaDevices::audioOutputs().size(); i++) {
            if (devname == QMediaDevices::audioOutputs().at(i).description()) {
                break;
            }
        }

        if (i >= QMediaDevices::audioOutputs().size()) {
            qDebug() << QString("not found device:%1").arg(devname);
            return;
        }
        QAudioDevice info(QMediaDevices::audioOutputs().at(i));

        if (!info.isFormatSupported(format)) {
            qWarning() <<
            "Raw audio format not supported by backend, cannot play audio.";
            return;
        }

        m_audioObject->openSink(format, info);
    });
    connect(ui->pushButtonclose, &QPushButton::clicked, this, [ = ]() {
        m_audioObject->colseSource();
    });
    connect(ui->pushButtonclose_2, &QPushButton::clicked, this, [ = ]() {
        m_audioObject->colseSink();
    });

    for (int i = 0; i < QMediaDevices::audioInputs().size(); i++) {
        qDebug() << QMediaDevices::audioInputs().at(i).description() <<
            QMediaDevices::audioInputs().at(i).preferredFormat();
        ui->comboBox01->addItem(QMediaDevices::audioInputs().at(i).description());
    }
    ui->comboBox01->setCurrentText(
        QMediaDevices::defaultAudioInput().description());

    for (int i = 0; i < QMediaDevices::audioOutputs().size(); i++) {
        qDebug() << QMediaDevices::audioOutputs().at(i).description() <<
            QMediaDevices::audioOutputs().at(i).preferredFormat();
        ui->comboBox11->addItem(QMediaDevices::audioOutputs().at(i).description());
    }
    ui->comboBox11->setCurrentText(
        QMediaDevices::defaultAudioOutput().description());


    QMediaDevices *mdev = new QMediaDevices(this);
    connect(mdev, &QMediaDevices::audioOutputsChanged, this, [ = ]() {
        ui->comboBox11->clear();

        for (int i = 0; i < QMediaDevices::audioOutputs().size(); i++) {
            qDebug() << QMediaDevices::audioOutputs().at(i).description() <<
            QMediaDevices::audioOutputs().at(i).preferredFormat();
            ui->comboBox11->addItem(QMediaDevices::audioOutputs().at(i).
                                    description());
        }
        ui->comboBox11->setCurrentText(
            QMediaDevices::defaultAudioOutput().description());
    });
    connect(mdev, &QMediaDevices::audioInputsChanged, this, [ = ]() {
        ui->comboBox01->clear();

        for (int i = 0; i < QMediaDevices::audioInputs().size(); i++) {
            qDebug() << QMediaDevices::audioInputs().at(i).description() <<
            QMediaDevices::audioInputs().at(i).preferredFormat();
            ui->comboBox01->addItem(QMediaDevices::audioInputs().at(
                                        i).description());
        }
        ui->comboBox01->setCurrentText(
            QMediaDevices::defaultAudioInput().description());
    });
    connect(mdev, &QMediaDevices::videoInputsChanged, this, [ = ]() {
        //
    });
}

#include <QOpenGLWidget>
#include <QGraphicsWidget>
customPlotCurve::customPlotCurve(QWidget *parent)
{
#ifdef USEDXXWPLOT
    m_customPlot = new XxwCustomPlot(this);
    m_customPlot->showTracer(true);
#else // ifdef USEDXXWPLOT
    m_customPlot = new QCustomPlotEx(this);
#endif // ifdef USEDXXWPLOT

#if (QT_VERSION <= QT_VERSION_CHECK(6, 5, 0))
    m_customPlot->setOpenGl(true);
    qDebug() << m_customPlot->openGl();
#endif // if (QT_VERSION <= QT_VERSION_CHECK(6, 5, 0))

    m_customPlot->xAxis->setVisible(true);
    m_customPlot->xAxis->setTickLabels(true);
    m_customPlot->yAxis->setVisible(true);
    m_customPlot->yAxis->setTickLabels(true);

    m_customPlot->xAxis2->setVisible(false);
    m_customPlot->xAxis2->setTickLabels(false); // 隐藏x轴刻度线
    m_customPlot->yAxis2->setVisible(false);
    m_customPlot->yAxis2->setTickLabels(false);

#if 0

    // m_customPlot->xAxis->setTickLabels(false);
    m_customPlot->axisRect()->addAxes(QCPAxis::atBottom);
    m_customPlot->axisRect()->axis(QCPAxis::atBottom, 0)->setPadding(30);

    // m_customPlot->xAxis->setRange(0, 1);
    // m_customPlot->axisRect()->axis(QCPAxis::atBottom, 0)->setRange(0,1);

    // // create graphs:
    // QCPGraph *mGraph1 = m_customPlot->addGraph(m_customPlot->xAxis,
    // m_customPlot->axisRect()->axis(QCPAxis::atBottom, 0));
    // mGraph1->setPen(QPen(QColor(250, 120, 0)));
    // // create tags with newly introduced AxisTag class (see axistag.h/.cpp):
    // AxisTag *mTag1 = new AxisTag(mGraph1->valueAxis());
    // mTag1->setPen(mGraph1->pen());
#endif // if 1

    // 背景色
    m_customPlot->setBackground(QColor(0, 0, 0, 0));

    m_customPlot->xAxis->setBasePen(QPen(Qt::darkGray, 2)); // x轴主线
    m_customPlot->yAxis->setBasePen(QPen(Qt::darkGray, 2)); // y轴主线
    // 网格线
    m_customPlot->xAxis->grid()->setVisible(false);
    m_customPlot->yAxis->grid()->setVisible(false);

    // m_customPlot->xAxis->grid()->
    // setPen(QPen(QColor(180, 180, 180), 1, Qt::DashLine));
    // m_customPlot->yAxis->grid()->
    // setPen(QPen(QColor(180, 180, 180), 1, Qt::DashLine));

    // 设置多选键
    m_customPlot->setMultiSelectModifier(Qt::KeyboardModifier::ControlModifier);

    // 可拖动、可缩放、轴可选、图例可选、绘图可选,可多选
    m_customPlot->setInteractions(
        QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
        QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iMultiSelect);

    // 鼠标滚动缩放倍率，默认0.85
    // customPlot->axisRect()->setRangeZoomFactor(0.5);
    // 设置鼠标滚轮轴方向
    m_customPlot->axisRect()->setRangeZoom(Qt::Vertical | Qt::Horizontal);

    // 默认情况下，图例位于主轴矩形的插入布局中，所以这就是我们如何访问它来改变图例的位置：
    m_customPlot->axisRect()->insetLayout()->setInsetAlignment(0,
                                                               Qt::AlignTop |
                                                               Qt::AlignCenter);

    m_customPlot->legend->setSelectableParts(QCPLegend::spItems);

    // 使左轴和下轴始终将其范围转移到右轴和上轴:
    connect(m_customPlot->xAxis,
            SIGNAL(rangeChanged(QCPRange)),
            m_customPlot->xAxis2,
            SLOT(setRange(QCPRange)));
    connect(m_customPlot->yAxis,
            SIGNAL(rangeChanged(QCPRange)),
            m_customPlot->yAxis2,
            SLOT(setRange(QCPRange)));

    m_customPlot->xAxis->setLabel("x");
    m_customPlot->yAxis->setLabel("y");

    // 显示图例
    m_customPlot->legend->setVisible(true);
    m_customPlot->legend->setBrush(QBrush(QColor(0, 0, 255, 20)));
    m_customPlot->legend->setTextColor(QColor(Qt::lightGray));
    m_customPlot->legend->setBorderPen(Qt::NoPen);          // 无边框
    m_customPlot->legend->setMargins(QMargins(0, 0, 0, 0)); // 文字与边框的距离
    // m_customPlot->legend->setFillOrder(QCPLayoutGrid::foRowsFirst);
    // m_customPlot->legend->setWrap(2); // 设置2个图例自动换行
    // 设置图例位置，这里选择显示在QCPAxisRect下方
    // m_customPlot->plotLayout()->addElement(1, 0, customPlot->legend);
    // 设置显示比例
    // m_customPlot->plotLayout()->setRowStretchFactor(1, 0.001);

    // m_customPlot->xAxis->setRange(0, 1);
    // m_customPlot->addGraph();
    // m_customPlot->graph(0)->setPen(QPen(Qt::blue));
    // m_customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // 设置曲线下方的背景颜色
    // m_customPlot->graph(0)->setName("数据");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_customPlot);
}

customPlotCurve::~customPlotCurve()
{}

void customPlotCurve::setShowTracer(bool isShow)
{
#ifdef USEDXXWPLOT
    m_customPlot->showTracer(isShow);
    m_customPlot->replot();
#endif // ifdef USEDXXWPLOT
}

void customPlotCurve::setLabel(const QString& xStr, const QString& yStr)
{
    m_customPlot->xAxis->setLabel(xStr);
    m_customPlot->yAxis->setLabel(yStr);
}

void customPlotCurve::setLegendShow(bool isShow)
{
    m_customPlot->legend->setVisible(isShow);
}

void customPlotCurve::addGraph(const QString& name,
                               const QPen   & pen,
                               const QBrush & brush)
{
    m_customPlot->addGraph();
    m_customPlot->graph(m_customPlot->graphCount() - 1)->setPen(pen);

    // 设置曲线下方的背景颜色
    // m_customPlot->graph(m_customPlot->graphCount() - 1)->setBrush(brush);

    m_customPlot->graph(m_customPlot->graphCount() - 1)->setName(name);
}

void customPlotCurve::setXRang(double lower, double upper)
{
    m_customPlot->xAxis->setRange(lower, upper);
}

void customPlotCurve::setYRang(double lower, double upper)
{
    m_customPlot->yAxis->setRange(lower, upper);
}
