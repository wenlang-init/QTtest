#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QTimer>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    initWidget();

    // QTimer *timer = new QTimer(this);
    // timer->start(500);
    // connect(timer,&QTimer::timeout,this,[=](){
    //     customPlot->replot();
    //     customPlotfft->replot();
    // });

    connect(ui->pushButtonin,&QPushButton::clicked,this,&Widget::opensource);
    connect(ui->pushButtoninclose,&QPushButton::clicked,this,&Widget::closesource);
    connect(ui->pushButtonout,&QPushButton::clicked,this,&Widget::opensink);
    connect(ui->pushButtonoutclose,&QPushButton::clicked,this,&Widget::closesink);

    audiop = new AudioProc;
    audiop->setvalue(ui->spinBox->value(),ui->doubleSpinBox->value());
    thread = new QThread(this);
    audiop->moveToThread(thread);
    thread->start();
    connect(this,&Widget::readData,audiop,&AudioProc::readData);
    connect(this,&Widget::clearData,audiop,&AudioProc::clearData);
    //qRegisterMetaType<CurveData>("CurveData");
    connect(audiop,&AudioProc::dataflush,this,&Widget::dataflush);
    //connect(audiop,SIGNAL(dataflush(CurveData, QList<QPointF>, QString)),this,SLOT(dataflush(CurveData, QList<QPointF>, QString)));

    QTimer *timer = new QTimer(this);
    timer->start(500);
    connect(timer,&QTimer::timeout,this,&Widget::timeout);
}

Widget::~Widget()
{
    delete ui;
    thread->quit();
    thread->wait(1000);
    //delete audiop;
    //delete thread;
    closesink();
    closesource();
}

void Widget::initWidget()
{
    connect(ui->comboBoxin,&QComboBox::currentTextChanged,this,[=](const QString &text){
        for(int i=0;i<QMediaDevices::audioInputs().size();i++){
            if(text == QMediaDevices::audioInputs().at(i).description()){
                ui->comboBoxinbit->clear();
                QList<QAudioFormat::SampleFormat> sfmat = QMediaDevices::audioInputs().at(i).supportedSampleFormats();
                for(int j=0;j<sfmat.size();j++){
                    switch (sfmat[j]) {
                    case QAudioFormat::Unknown:
                        ui->comboBoxinbit->addItem("Unknown");
                        break;
                    case QAudioFormat::UInt8:
                        ui->comboBoxinbit->addItem("UInt8");
                        break;
                    case QAudioFormat::Int16:
                        ui->comboBoxinbit->addItem("Int16");
                        break;
                    case QAudioFormat::Int32:
                        ui->comboBoxinbit->addItem("Int32");
                        break;
                    case QAudioFormat::Float:
                        ui->comboBoxinbit->addItem("Float");
                        break;
                    case QAudioFormat::NSampleFormats:
                        ui->comboBoxinbit->addItem("NSampleFormats");
                        break;
                    default:
                        break;
                    }
                }

                ui->comboBoxinch->clear();
                ui->comboBoxinch->addItem("ChannelConfigUnknown");
                ui->comboBoxinch->addItem("ChannelConfigMono");
                ui->comboBoxinch->addItem("ChannelConfigStereo");
                ui->comboBoxinch->addItem("ChannelConfig2Dot1");
                ui->comboBoxinch->addItem("ChannelConfig3Dot0");
                ui->comboBoxinch->addItem("ChannelConfig3Dot1");
                ui->comboBoxinch->addItem("ChannelConfigSurround5Dot0");
                ui->comboBoxinch->addItem("ChannelConfigSurround5Dot1");
                ui->comboBoxinch->addItem("ChannelConfigSurround7Dot0");
                ui->comboBoxinch->addItem("ChannelConfigSurround7Dot1");

                ui->spinBoxin->setMaximum(QMediaDevices::audioInputs().at(i).maximumSampleRate());
                ui->spinBoxin->setMinimum(QMediaDevices::audioInputs().at(i).minimumSampleRate());

                ui->spinBoxinch->setMaximum(QMediaDevices::audioInputs().at(i).maximumChannelCount());
                ui->spinBoxinch->setMinimum(QMediaDevices::audioInputs().at(i).minimumChannelCount());

                ui->label_11->setText(QString("采样率(Hz):%1~%2").arg(ui->spinBoxin->minimum()).arg(ui->spinBoxin->maximum()));
                ui->label_12->setText(QString("通道数:%1~%2").arg(ui->spinBoxinch->minimum()).arg(ui->spinBoxinch->maximum()));

                switch (QMediaDevices::audioInputs().at(i).preferredFormat().sampleFormat()) {
                case QAudioFormat::Unknown:
                    ui->comboBoxinbit->setCurrentText("Unknown");
                    break;
                case QAudioFormat::UInt8:
                    ui->comboBoxinbit->setCurrentText("UInt8");
                    break;
                case QAudioFormat::Int16:
                    ui->comboBoxinbit->setCurrentText("Int16");
                    break;
                case QAudioFormat::Int32:
                    ui->comboBoxinbit->setCurrentText("Int32");
                    break;
                case QAudioFormat::Float:
                    ui->comboBoxinbit->setCurrentText("Float");
                    break;
                case QAudioFormat::NSampleFormats:
                    ui->comboBoxinbit->setCurrentText("NSampleFormats");
                    break;
                default:
                    break;
                }

                switch (QMediaDevices::audioInputs().at(i).channelConfiguration()) {
                case QAudioFormat::ChannelConfigUnknown:
                    ui->comboBoxinch->setCurrentText("ChannelConfigUnknown");
                    break;
                case QAudioFormat::ChannelConfigMono:
                    ui->comboBoxinch->setCurrentText("ChannelConfigMono");
                    break;
                case QAudioFormat::ChannelConfigStereo:
                    ui->comboBoxinch->setCurrentText("ChannelConfigStereo");
                    break;
                case QAudioFormat::ChannelConfig2Dot1:
                    ui->comboBoxinch->setCurrentText("ChannelConfig2Dot1");
                    break;
                case QAudioFormat::ChannelConfig3Dot0:
                    ui->comboBoxinch->setCurrentText("ChannelConfig3Dot0");
                    break;
                case QAudioFormat::ChannelConfigSurround5Dot0:
                    ui->comboBoxinch->setCurrentText("ChannelConfigSurround5Dot0");
                    break;
                case QAudioFormat::ChannelConfigSurround5Dot1:
                    ui->comboBoxinch->setCurrentText("ChannelConfigSurround5Dot1");
                    break;
                case QAudioFormat::ChannelConfigSurround7Dot0:
                    ui->comboBoxinch->setCurrentText("ChannelConfigSurround7Dot0");
                    break;
                case QAudioFormat::ChannelConfig3Dot1:
                    ui->comboBoxinch->setCurrentText("ChannelConfig3Dot1");
                    break;
                case QAudioFormat::ChannelConfigSurround7Dot1:
                    ui->comboBoxinch->setCurrentText("ChannelConfigSurround7Dot1");
                    break;
                default:
                    break;
                }

                ui->spinBoxin->setValue(QMediaDevices::audioInputs().at(i).preferredFormat().sampleRate());
                ui->spinBoxinch->setValue(QMediaDevices::audioInputs().at(i).preferredFormat().channelCount());
                break;
            }
        }
    });
    connect(ui->comboBoxout,&QComboBox::currentTextChanged,this,[=](const QString &text){
        for(int i=0;i<QMediaDevices::audioOutputs().size();i++){
            if(text == QMediaDevices::audioOutputs().at(i).description()){
                ui->comboBoxoutbit->clear();
                QList<QAudioFormat::SampleFormat> sfmat = QMediaDevices::audioOutputs().at(i).supportedSampleFormats();
                for(int j=0;j<sfmat.size();j++){
                    switch (sfmat[j]) {
                    case QAudioFormat::Unknown:
                        ui->comboBoxoutbit->addItem("Unknown");
                        break;
                    case QAudioFormat::UInt8:
                        ui->comboBoxoutbit->addItem("UInt8");
                        break;
                    case QAudioFormat::Int16:
                        ui->comboBoxoutbit->addItem("Int16");
                        break;
                    case QAudioFormat::Int32:
                        ui->comboBoxoutbit->addItem("Int32");
                        break;
                    case QAudioFormat::Float:
                        ui->comboBoxoutbit->addItem("Float");
                        break;
                    case QAudioFormat::NSampleFormats:
                        ui->comboBoxoutbit->addItem("NSampleFormats");
                        break;
                    default:
                        break;
                    }
                }

                ui->comboBoxoutch->clear();
                ui->comboBoxoutch->addItem("ChannelConfigUnknown");
                ui->comboBoxoutch->addItem("ChannelConfigMono");
                ui->comboBoxoutch->addItem("ChannelConfigStereo");
                ui->comboBoxoutch->addItem("ChannelConfig2Dot1");
                ui->comboBoxoutch->addItem("ChannelConfig3Dot0");
                ui->comboBoxoutch->addItem("ChannelConfig3Dot1");
                ui->comboBoxoutch->addItem("ChannelConfigSurround5Dot0");
                ui->comboBoxoutch->addItem("ChannelConfigSurround5Dot1");
                ui->comboBoxoutch->addItem("ChannelConfigSurround7Dot0");
                ui->comboBoxoutch->addItem("ChannelConfigSurround7Dot1");

                ui->spinBoxout->setMaximum(QMediaDevices::audioOutputs().at(i).maximumSampleRate());
                ui->spinBoxout->setMinimum(QMediaDevices::audioOutputs().at(i).minimumSampleRate());

                ui->spinBoxoutch->setMaximum(QMediaDevices::audioOutputs().at(i).maximumChannelCount());
                ui->spinBoxoutch->setMinimum(QMediaDevices::audioOutputs().at(i).minimumChannelCount());

                ui->label_3->setText(QString("采样率(Hz):%1~%2").arg(ui->spinBoxout->minimum()).arg(ui->spinBoxout->maximum()));
                ui->label_4->setText(QString("通道数:%1~%2").arg(ui->spinBoxoutch->minimum()).arg(ui->spinBoxoutch->maximum()));

                switch (QMediaDevices::audioOutputs().at(i).preferredFormat().sampleFormat()) {
                case QAudioFormat::Unknown:
                    ui->comboBoxoutbit->setCurrentText("Unknown");
                    break;
                case QAudioFormat::UInt8:
                    ui->comboBoxoutbit->setCurrentText("UInt8");
                    break;
                case QAudioFormat::Int16:
                    ui->comboBoxoutbit->setCurrentText("Int16");
                    break;
                case QAudioFormat::Int32:
                    ui->comboBoxoutbit->setCurrentText("Int32");
                    break;
                case QAudioFormat::Float:
                    ui->comboBoxoutbit->setCurrentText("Float");
                    break;
                case QAudioFormat::NSampleFormats:
                    ui->comboBoxoutbit->setCurrentText("NSampleFormats");
                    break;
                default:
                    break;
                }

                switch (QMediaDevices::audioOutputs().at(i).channelConfiguration()) {
                case QAudioFormat::ChannelConfigUnknown:
                    ui->comboBoxoutch->setCurrentText("ChannelConfigUnknown");
                    break;
                case QAudioFormat::ChannelConfigMono:
                    ui->comboBoxoutch->setCurrentText("ChannelConfigMono");
                    break;
                case QAudioFormat::ChannelConfigStereo:
                    ui->comboBoxoutch->setCurrentText("ChannelConfigStereo");
                    break;
                case QAudioFormat::ChannelConfig2Dot1:
                    ui->comboBoxoutch->setCurrentText("ChannelConfig2Dot1");
                    break;
                case QAudioFormat::ChannelConfig3Dot0:
                    ui->comboBoxoutch->setCurrentText("ChannelConfig3Dot0");
                    break;
                case QAudioFormat::ChannelConfigSurround5Dot0:
                    ui->comboBoxoutch->setCurrentText("ChannelConfigSurround5Dot0");
                    break;
                case QAudioFormat::ChannelConfigSurround5Dot1:
                    ui->comboBoxoutch->setCurrentText("ChannelConfigSurround5Dot1");
                    break;
                case QAudioFormat::ChannelConfigSurround7Dot0:
                    ui->comboBoxoutch->setCurrentText("ChannelConfigSurround7Dot0");
                    break;
                case QAudioFormat::ChannelConfig3Dot1:
                    ui->comboBoxoutch->setCurrentText("ChannelConfig3Dot1");
                    break;
                case QAudioFormat::ChannelConfigSurround7Dot1:
                    ui->comboBoxoutch->setCurrentText("ChannelConfigSurround7Dot1");
                    break;
                default:
                    break;
                }

                ui->spinBoxout->setValue(QMediaDevices::audioOutputs().at(i).preferredFormat().sampleRate());
                ui->spinBoxoutch->setValue(QMediaDevices::audioOutputs().at(i).preferredFormat().channelCount());

                break;
            }
        }
    });

    for(int i=0;i<QMediaDevices::audioOutputs().size();i++){
        // qDebug()<<QMediaDevices::audioOutputs()[i].description()
        // <<QMediaDevices::audioOutputs()[i].supportedSampleFormats()
        // <<QMediaDevices::audioOutputs()[i].channelConfiguration()
        // <<QMediaDevices::audioOutputs()[i].maximumChannelCount()
        // <<QMediaDevices::audioOutputs()[i].minimumChannelCount()
        // <<QMediaDevices::audioOutputs()[i].maximumSampleRate()
        // <<QMediaDevices::audioOutputs()[i].minimumSampleRate()
        // <<QMediaDevices::audioOutputs()[i].id()
        // <<QMediaDevices::audioOutputs()[i].mode()
        // <<QMediaDevices::audioOutputs()[i].preferredFormat();

        qDebug()<<QMediaDevices::audioOutputs().at(i).description()<<QMediaDevices::audioOutputs().at(i).preferredFormat();

        ui->comboBoxout->addItem(QMediaDevices::audioOutputs().at(i).description());
    }
    ui->comboBoxout->setCurrentText(QMediaDevices::defaultAudioOutput().description());

    for(int i=0;i<QMediaDevices::audioInputs().size();i++){
        qDebug()<<QMediaDevices::audioInputs().at(i).description()<<QMediaDevices::audioInputs().at(i).preferredFormat();
        ui->comboBoxin->addItem(QMediaDevices::audioInputs().at(i).description());
    }
    ui->comboBoxin->setCurrentText(QMediaDevices::defaultAudioInput().description());

    initcustomolot();
}

void Widget::initcustomolot()
{
    #ifndef USED_XXW
    customPlot = new QCustomPlot;
    #else
    customPlot = new XxwCustomPlot;
    customPlot->showTracer(true);
    #endif
    //customPlot->setOpenGl(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(ui->widget);
    mainLayout->addWidget(customPlot);

    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::blue));
    //customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // 设置曲线下方的背景颜色
    customPlot->graph(0)->setName("数据");

    customPlot->xAxis2->setVisible(true);
    customPlot->xAxis2->setTickLabels(false); // 隐藏x轴刻度线
    customPlot->yAxis2->setVisible(true);
    customPlot->yAxis2->setTickLabels(false);

    //connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    //connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // 鼠标拖动，鼠标为中心缩放，点选图形
    customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);//|QCP::iSelectPlottables);
    // 鼠标滚动缩放倍率，默认0.85
    //customPlot->axisRect()->setRangeZoomFactor(0.5);
    // 设置鼠标滚轮轴方向
    customPlot->axisRect()->setRangeZoom(Qt::Vertical|Qt::Horizontal);
    // 显示图例
    customPlot->legend->setVisible(true);
    customPlot->legend->setBrush(QBrush(QColor(0, 0, 255, 20)));
    customPlot->legend->setTextColor(QColor(Qt::white));

    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignCenter);

    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    //customPlot->rescaleAxes(true);

    //QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    //timeTicker->setTimeFormat("%d\n 9%h:%m:%s.%z");
    //customPlot->xAxis->setTicker(timeTicker);

    //    QVector<double> x(251), y(251);
    //    for (int i=0; i<251; ++i)
    //    {
    //        x[i] = i;
    //        y[i] = qExp(-i/150.0)*qCos(i/10.0);
    //    }
    //    customPlot->graph(0)->setData(x, y);
    //    customPlot->replot();
    #ifndef USED_XXW
    customPlotfft = new QCustomPlot;
    #else
    customPlotfft = new XxwCustomPlot;
    customPlotfft->showTracer(true);
    #endif
    //customPlotfft->setOpenGl(true);

    QVBoxLayout *mainLayout1 = new QVBoxLayout(ui->widgetfft);
    mainLayout1->addWidget(customPlotfft);

    customPlotfft->addGraph();
    customPlotfft->graph(0)->setPen(QPen(Qt::blue));
    //customPlotfft->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // 设置曲线下方的背景颜色
    customPlotfft->graph(0)->setName("数据");

    customPlotfft->xAxis2->setVisible(true);
    customPlotfft->xAxis2->setTickLabels(false); // 隐藏x轴刻度线
    customPlotfft->yAxis2->setVisible(true);
    customPlotfft->yAxis2->setTickLabels(false);

    //connect(customPlotfft->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlotfft->xAxis2, SLOT(setRange(QCPRange)));
    //connect(customPlotfft->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlotfft->yAxis2, SLOT(setRange(QCPRange)));

    // 鼠标拖动，鼠标为中心缩放，点选图形
    customPlotfft->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);//|QCP::iSelectPlottables);
    // 鼠标滚动缩放倍率，默认0.85
    //customPlotfft->axisRect()->setRangeZoomFactor(0.5);
    // 设置鼠标滚轮轴方向
    customPlotfft->axisRect()->setRangeZoom(Qt::Vertical|Qt::Horizontal);
    // 显示图例
    customPlotfft->legend->setVisible(true);
    customPlotfft->legend->setBrush(QBrush(QColor(0, 0, 255, 20)));
    customPlotfft->legend->setTextColor(QColor(Qt::white));

    customPlotfft->xAxis->setRange(0,1);

    customPlotfft->xAxis->setLabel("归一化值");
    customPlotfft->yAxis->setLabel("幅值");
    //customPlotfft->rescaleAxes(true);
#ifndef USED_XXW
    // 对鼠标所在的点进行判断，该点的x、y是否在距离曲线的2的像素点范围内，则显示坐标
    customPlot->installEventFilter(this);
    customPlotfft->installEventFilter(this);
#endif
}

#ifndef USED_XXW
bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == customPlot || watched == customPlotfft)
    {
        if(event->type() == QEvent::MouseMove)
        {
            QMouseEvent *pMouseEvent = (QMouseEvent*)event;
            handlePositionToolTip(watched,pMouseEvent);
        }
    }
    return QWidget::eventFilter(watched, event);
}

void Widget::handlePositionToolTip(QObject *watched,QMouseEvent *event)
{
    QCustomPlot *m_pPlot = static_cast<QCustomPlot*>(watched);
    QList<QCPGraph*> m_vecGraph;
    //m_pPlot->xAxis->graphs();
    for(int i=0;i<m_pPlot->graphCount();i++){
        m_vecGraph.append(m_pPlot->graph(i));
    }
    //鼠标坐标转化为CustomPlot内部坐标
    double dValueX = m_pPlot->xAxis->pixelToCoord(event->pos().x());
    double dValueY = m_pPlot->yAxis->pixelToCoord(event->pos().y());
    //判断是否超出坐标轴范围
    if(dValueX < m_pPlot->xAxis->range().lower || dValueX > m_pPlot->xAxis->range().upper
        || dValueY < m_pPlot->yAxis->range().lower || dValueY > m_pPlot->yAxis->range().upper)
        return;
    //逐个图像判断符合条件的曲线点（x,y坐标均相差两个像素点内）
    for(int i = 0; i < m_vecGraph.count(); i++)
    {
        if(m_vecGraph.at(i)->dataCount() < 1)continue;
        if(!m_vecGraph.at(i)->visible())
            continue;
        if(getGraphMinDistance(m_pPlot,m_vecGraph[i],i, dValueX,dValueY))
        {
            break;
        }
    }
}

bool Widget::getGraphMinDistance(QCustomPlot *m_pPlot,QCPGraph* m_vecGraph,int iIndex, double &dValueX,double &dValueY)
{
    //通过鼠标所在点X值查找离曲线最近的一个key值索引
    int index_left = m_vecGraph->findBegin(dValueX, true); //左边最近的一个key值索引
    int index_right = m_vecGraph->findEnd(dValueX, true);  //右边最近的一个key值索引
    double dif_left = fabs(m_vecGraph->data()->at(index_left)->key - dValueX);
    double dif_right = fabs(m_vecGraph->data()->at(index_right)->key - dValueX);
    int iPointIdx = ((dif_left < dif_right) ? index_left : index_right);

    double x = m_vecGraph->data()->at(iPointIdx)->key;
    double y = m_vecGraph->data()->at(iPointIdx)->value;

    double dRatioX = m_pPlot->xAxis->axisRect()->width() / (m_pPlot->xAxis->range().upper - m_pPlot->xAxis->range().lower); //求得X轴像素比
    double dRatioY = m_pPlot->yAxis->axisRect()->height() / (m_pPlot->yAxis->range().upper - m_pPlot->yAxis->range().lower); //计算Y轴像素比

    double dx = fabs(x - dValueX);
    double dy = fabs(y - dValueY);

    //    qDebug()<<"index:"<<iIndex<<"=pointx:"<<QString::number(dValueX, 'f', 8)<<",pointy:"<<QString::number(dValueY, 'f', 8)<<"===x:"<<QString::number(x, 'f', 8)
    //             <<",y:"<<QString::number(y, 'f', 8)<<"==dx:"<<QString::number(dx, 'f', 8)<<",dy:"<<QString::number(dy, 'f', 8);
    //判断是否在允许的范围内
    if(dx * dRatioX <= 2 && dy * dRatioY <= 10)
    {
        QString strToolTip = QString("CH%1\nx=%2\ny=%3").arg(iIndex).arg(x).arg(y);
        QToolTip::showText(cursor().pos(), strToolTip, m_pPlot);
        return true;
    }
    if(QToolTip::isVisible())
        QToolTip::hideText();
    return false;
}
#endif

void Widget::opensource()
{
    if(nullptr != audioin){
        return;
    }

    //QAudioFormat format;
    format.setSampleRate(ui->spinBoxin->value());
    format.setChannelCount(ui->spinBoxinch->value());
    if(ui->comboBoxinbit->currentText() == "Unknown"){
        format.setSampleFormat(QAudioFormat::Unknown);
    } else if(ui->comboBoxinbit->currentText() == "UInt8"){
        format.setSampleFormat(QAudioFormat::UInt8);
    } else if(ui->comboBoxinbit->currentText() == "Int16"){
        format.setSampleFormat(QAudioFormat::Int16);
    } else if(ui->comboBoxinbit->currentText() == "Int32"){
        format.setSampleFormat(QAudioFormat::Int32);
    } else if(ui->comboBoxinbit->currentText() == "Float"){
        format.setSampleFormat(QAudioFormat::Float);
    } else if(ui->comboBoxinbit->currentText() == "NSampleFormats"){
        format.setSampleFormat(QAudioFormat::NSampleFormats);
    } else {
        qDebug()<<QString("not found format:%1").arg(ui->comboBoxinbit->currentText());
        return;
    }

    if(ui->comboBoxinch->currentText() == "ChannelConfigUnknown"){
        format.setChannelConfig(QAudioFormat::ChannelConfigUnknown);
    } else if(ui->comboBoxinch->currentText() == "ChannelConfigMono"){
        format.setChannelConfig(QAudioFormat::ChannelConfigMono);
    } else if(ui->comboBoxinch->currentText() == "ChannelConfigStereo"){
        format.setChannelConfig(QAudioFormat::ChannelConfigStereo);
    } else if(ui->comboBoxinch->currentText() == "ChannelConfig2Dot1"){
        format.setChannelConfig(QAudioFormat::ChannelConfig2Dot1);
    } else if(ui->comboBoxinch->currentText() == "ChannelConfig3Dot0"){
        format.setChannelConfig(QAudioFormat::ChannelConfig3Dot0);
    } else if(ui->comboBoxinch->currentText() == "ChannelConfigSurround5Dot0"){
        format.setChannelConfig(QAudioFormat::ChannelConfigSurround5Dot0);
    } else if(ui->comboBoxinch->currentText() == "ChannelConfigSurround5Dot1"){
        format.setChannelConfig(QAudioFormat::ChannelConfigSurround5Dot1);
    } else if(ui->comboBoxinch->currentText() == "ChannelConfigSurround7Dot0"){
        format.setChannelConfig(QAudioFormat::ChannelConfigSurround7Dot0);
    } else if(ui->comboBoxinch->currentText() == "ChannelConfig3Dot1"){
        format.setChannelConfig(QAudioFormat::ChannelConfig3Dot1);
    } else if(ui->comboBoxinch->currentText() == "ChannelConfigSurround7Dot1"){
        format.setChannelConfig(QAudioFormat::ChannelConfigSurround7Dot1);
    } else {
        qDebug()<<QString("not found ChannelConfig:%1").arg(ui->comboBoxinch->currentText());
        return;
    }
#if 0
    qsizetype i;
    QString devname = ui->comboBoxin->currentText();
    for(i=0;i<QMediaDevices::audioInputs().size();i++){
        if(devname == QMediaDevices::audioInputs().at(i).description()){
            break;
        }
    }
    if(i >= QMediaDevices::audioInputs().size()){
        qDebug()<<QString("not found device:%1").arg(devname);
        return;
    }
    QAudioDevice info(QMediaDevices::audioInputs().at(i));
    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }
#endif

    audioin = new QAudioSource(format,this);
    connect(audioin, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
    QIODevice *io = audioin->start();
    connect(io,&QIODevice::readyRead,this,[=](){
        QByteArray data = io->readAll();
        //qDebug()<<data.size();
        //processData(data,format);
        emit readData(data,format);
    });
}

void Widget::closesource()
{
    if(nullptr != audioin){
        audioin->stop();
        delete audioin;
        audioin = nullptr;
        emit clearData();
    }
}

void Widget::opensink()
{
    if(audioout != nullptr){
        return;
    }
    QAudioFormat format;
    format.setSampleRate(ui->spinBoxout->value());
    format.setChannelCount(ui->spinBoxoutch->value());
    if(ui->comboBoxoutbit->currentText() == "Unknown"){
        format.setSampleFormat(QAudioFormat::Unknown);
    } else if(ui->comboBoxoutbit->currentText() == "UInt8"){
        format.setSampleFormat(QAudioFormat::UInt8);
    } else if(ui->comboBoxoutbit->currentText() == "Int16"){
        format.setSampleFormat(QAudioFormat::Int16);
    } else if(ui->comboBoxoutbit->currentText() == "Int32"){
        format.setSampleFormat(QAudioFormat::Int32);
    } else if(ui->comboBoxoutbit->currentText() == "Float"){
        format.setSampleFormat(QAudioFormat::Float);
    } else if(ui->comboBoxoutbit->currentText() == "NSampleFormats"){
        format.setSampleFormat(QAudioFormat::NSampleFormats);
    } else {
        qDebug()<<QString("not found format:%1").arg(ui->comboBoxoutbit->currentText());
        return;
    }

    if(ui->comboBoxoutch->currentText() == "ChannelConfigUnknown"){
        format.setChannelConfig(QAudioFormat::ChannelConfigUnknown);
    } else if(ui->comboBoxoutch->currentText() == "ChannelConfigMono"){
        format.setChannelConfig(QAudioFormat::ChannelConfigMono);
    } else if(ui->comboBoxoutch->currentText() == "ChannelConfigStereo"){
        format.setChannelConfig(QAudioFormat::ChannelConfigStereo);
    } else if(ui->comboBoxoutch->currentText() == "ChannelConfig2Dot1"){
        format.setChannelConfig(QAudioFormat::ChannelConfig2Dot1);
    } else if(ui->comboBoxoutch->currentText() == "ChannelConfig3Dot0"){
        format.setChannelConfig(QAudioFormat::ChannelConfig3Dot0);
    } else if(ui->comboBoxoutch->currentText() == "ChannelConfigSurround5Dot0"){
        format.setChannelConfig(QAudioFormat::ChannelConfigSurround5Dot0);
    } else if(ui->comboBoxoutch->currentText() == "ChannelConfigSurround5Dot1"){
        format.setChannelConfig(QAudioFormat::ChannelConfigSurround5Dot1);
    } else if(ui->comboBoxoutch->currentText() == "ChannelConfigSurround7Dot0"){
        format.setChannelConfig(QAudioFormat::ChannelConfigSurround7Dot0);
    } else if(ui->comboBoxoutch->currentText() == "ChannelConfig3Dot1"){
        format.setChannelConfig(QAudioFormat::ChannelConfig3Dot1);
    } else if(ui->comboBoxoutch->currentText() == "ChannelConfigSurround7Dot1"){
        format.setChannelConfig(QAudioFormat::ChannelConfigSurround7Dot1);
    } else {
        qDebug()<<QString("not found ChannelConfig:%1").arg(ui->comboBoxoutch->currentText());
        return;
    }

    audioout = new QAudioSink(format,this);
    connect(audioout, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
    //QIODevice *io = audioout->start();
    //io->write("qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
}

void Widget::closesink()
{
    if(nullptr != audioout){
        audioout->stop();
        delete audioout;
        audioout = nullptr;
    }
}

void Widget::handleStateChanged(QAudio::State newState)
{
    switch (newState) {
    case QAudio::IdleState:
        // Finished playing (no more data)
        if(audioout == sender()){
            // audioout->stop();
            // delete audioout;
            // audioout = nullptr;
        } else if(audioin == sender()){
            // audioin->stop();
            // delete audioin;
            // audioin = nullptr;
            // emit clearData();
        }
        break;

    case QAudio::StoppedState:
        // Stopped for other reasons
        if(audioout == sender()){
            qDebug()<<audioout->error();
            audioout->stop();
            delete audioout;
            audioout = nullptr;
        } else if(audioin == sender()){
            qDebug()<<audioin->error();
            audioin->stop();
            delete audioin;
            audioin = nullptr;
            emit clearData();
        }
        break;

    default:
        // ... other cases as appropriate
        break;
    }
}

void Widget::dataflush(CurveData timeData,CurveData frequencyDomainData,QString str)
{
    for(int i=0;i<timeData.valVec.size();i++){
        customPlot->graph(0)->addData(index++,timeData.valVec[i]);
    }
    if(customPlot->graph(0)->data().data()->size() > 1000*1000*10){
        customPlot->graph(0)->data().data()->clear();
        index = 0;
    }
    customPlot->xAxis->setRange(index-1 - (customPlot->xAxis->range().upper - customPlot->xAxis->range().lower),index-1);
    customPlot->replot();

    customPlotfft->graph(0)->data().data()->clear();
    for(int i=0;i<frequencyDomainData.keyVec.size();i++){
        customPlotfft->graph(0)->addData(frequencyDomainData.keyVec[i],frequencyDomainData.valVec[i]);
    }
    customPlotfft->replot();

    ui->labelinfo->setText(str);
}

void Widget::on_pushButtonadd_clicked()
{
    _params node;
    node.a = ui->doubleSpinBoxa->value();
    node.b = ui->doubleSpinBoxb->value();
    node.c = ui->doubleSpinBoxc->value();
    _cosparams.append(node);
    ui->comboBox->addItem(QString("%1*cos(%2*x+%3)").arg(node.a).arg(node.b).arg(node.c));
}

void Widget::on_pushButton_clicked()
{
    if(ui->comboBox->currentIndex() < _cosparams.size() &&
        ui->comboBox->currentIndex() >= 0){
        _cosparams.removeAt(ui->comboBox->currentIndex());
    }
    if(ui->comboBox->count() > 0){
        ui->comboBox->removeItem(ui->comboBox->currentIndex());
    }
}

void Widget::on_pushButtonrand_clicked()
{
    _params node;
    node.a = ui->doubleSpinBoxa1->value();
    node.b = 0;
    node.c = 0;
    _cosrandparams.append(node);
    ui->comboBoxrand->addItem(QString("%1*cos(rand())").arg(node.a));
}


void Widget::on_pushButton1_clicked()
{
    if(ui->comboBoxrand->currentIndex() < _cosrandparams.size() &&
        ui->comboBoxrand->currentIndex() >= 0){
        _cosrandparams.removeAt(ui->comboBoxrand->currentIndex());
    }
    if(ui->comboBoxrand->count() > 0){
        ui->comboBoxrand->removeItem(ui->comboBoxrand->currentIndex());
    }
}



void Widget::timeout()
{
    if(!ui->checkBox->isChecked())return;
    if(nullptr != audioin){
        return;
    }
    if(_cosparams.size() < 1 && _cosrandparams.size() < 1)return;
    if(_cosrandparams.size() > 0){
        srand(QTime::currentTime().msec());
    }

    double maxb = _cosparams[0].b;
    double minb = _cosparams[0].b;
    for(int i=0;i<_cosparams.size();i++){
        if(qAbs(maxb) > qAbs(_cosparams[i].b)){
            maxb = _cosparams[i].b;
        }
        if(qAbs(minb) < qAbs(_cosparams[i].b)){
            minb = _cosparams[i].b;
        }
    }

    CurveData timeData;
    CurveData frequencyDomainData;

    int fs = ui->spinBox->value(); // 2^fs
    static const int pointcount = qPow(2,fs+1);
    int index = 0;
    double step = qAbs(2*M_PI/pointcount);
    for(int i=0;i<pointcount;i++){
        double v=0;
        for(int coscnt=0;coscnt<_cosparams.size();coscnt++){
            v += _cosparams[coscnt].a*cos(step*(i+1) * _cosparams[coscnt].b + _cosparams[coscnt].c);
        }
        for(int coscnt=0;coscnt<_cosrandparams.size();coscnt++){
            v += _cosrandparams[coscnt].a*cos(rand());
        }
        timeData.keyVec.append(index++);
        timeData.valVec.append(v);
    }
    ///////////////////////////////////////////////
    fft::timeDomainToFreDomain(timeData,frequencyDomainData,fs);
    struct node{
        double key;
        double val;
        qsizetype index;
    };
    #define MAXPOINTCNT 10
    int m=0;
    node max[MAXPOINTCNT];
    max[m].key = 0;
    max[m].val = 0;
    max[m].index = 0;
    for(int i=0;i<frequencyDomainData.keyVec.size();i++){
        if(max[m].val < frequencyDomainData.valVec[i]){
            max[m].key = frequencyDomainData.keyVec[i];
            max[m].val = frequencyDomainData.valVec[i];
            max[m].index = i;
        }
    }
    m++;
    while(1){
        max[m].key = 0;
        max[m].val = 0;
        bool isnew = false;
        for(int i=0;i<frequencyDomainData.keyVec.size();i++){
            if(max[m].val < frequencyDomainData.valVec[i]){
                if(frequencyDomainData.valVec[i]<=max[m-1].val && i!=max[m-1].index){
                    max[m].key = frequencyDomainData.keyVec[i];
                    max[m].val = frequencyDomainData.valVec[i];
                    max[m].index = i;
                    isnew = true;
                }
            }
        }
        if(isnew == false)break;
        // 放弃小数据
        if(max[m].val < ui->doubleSpinBox->value()){
            break;
        }
        m++;
        if(m >= MAXPOINTCNT)break;
    }
    node tmp{0,0};
    for(int i=0;i<m-1;i++){
        for(int j=i;j<m;j++){
            if(max[i].key > max[j].key){
                tmp = max[j];
                max[j] = max[i];
                max[i] = tmp;
            }
        }
    }

    QString str1="幅值:";
    QString str2="归一化比例:";
    QString str3="频率(HZ):";
    for(int i=0;i<m;i++){
        str1 += " " + QString::number(max[i].val,'g',6);
        str3 += " " + QString::number(max[i].key*qPow(2,fs),'g',6);
        str2 += " " + QString::number(max[i].key,'g',6);
    }
    QString str = str1 + "\n" + str2 + "\n" + str3;

    dataflush(timeData,frequencyDomainData,str);
}


void Widget::on_spinBox_valueChanged(int arg1)
{
    Q_UNUSED(arg1)
    audiop->setvalue(ui->spinBox->value(),ui->doubleSpinBox->value());
}


void Widget::on_doubleSpinBox_valueChanged(double arg1)
{
    Q_UNUSED(arg1)
    audiop->setvalue(ui->spinBox->value(),ui->doubleSpinBox->value());
}

