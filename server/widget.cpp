
#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QMediaDevices>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QDebug>
#include <QTimer>
#include <QTemporaryFile>
#include <QBoxLayout>
#include "codeeditor/codeeditor.h"
#include "codeeditor/highlighter.h"
#include "qmlwidget.h"
#include "listw/listw.h"
#include "graphics/graphicswidget.h"
#include "weather/weatherwidget.h"

#define qdebug qDebug()<<__FILE__<<__LINE__<<__FUNCTION__

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //ui->textEdit_tcp->setReadOnly(true);

    tcpserver = new TcpServer(this);
    connect(tcpserver,&TcpServer::allinfo,this,[&](QString infostr){
        ui->textEdit_tcp->append(infostr);
    });

    udp = new Udpobj(this);
    connect(udp,&Udpobj::allinfo,this,[&](QString infostr){
        ui->textEdit_udp->append(infostr);
    });

    mplayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    mplayer->setAudioOutput(audioOutput);
    //audioOutput->setVolume(0.5);  //调节音频音量
    connect(mplayer, &QMediaPlayer::positionChanged, this, [=](qint64 pos){
        //qdebug<<pos;
        position = pos;
    });
    connect(mplayer, &QMediaPlayer::errorOccurred, this, [=](QMediaPlayer::Error error, const QString &errorString){
        qdebug<<error<<errorString;
        ui->textEdit->append(errorString);
    });

    for(int i=0;i<QMediaDevices::audioInputs().size();i++){
//        qdebug<<QMediaDevices::audioInputs()[i].description()<<QMediaDevices::audioInputs()[i].supportedSampleFormats()<<QMediaDevices::audioInputs()[i].channelConfiguration()
//               <<QMediaDevices::audioInputs()[i].maximumChannelCount()<<QMediaDevices::audioInputs()[i].minimumChannelCount()
//               <<QMediaDevices::audioInputs()[i].maximumSampleRate()<<QMediaDevices::audioInputs()[i].minimumSampleRate()
//               <<QMediaDevices::audioInputs()[i].id()<<QMediaDevices::audioInputs()[i].mode()
//               <<QMediaDevices::audioInputs()[i].preferredFormat();
        //qdebug<<QMediaDevices::audioInputs().at(i).description()<<QMediaDevices::audioInputs().at(i).preferredFormat();
        ui->comboBox1->addItem(QMediaDevices::audioInputs().at(i).description());
    }
    ui->comboBox1->setCurrentText(QMediaDevices::defaultAudioInput().description());

    for(int i=0;i<QMediaDevices::audioOutputs().size();i++){
//        qdebug<<QMediaDevices::audioOutputs()[i].description()<<QMediaDevices::audioOutputs()[i].supportedSampleFormats()<<QMediaDevices::audioOutputs()[i].channelConfiguration()
//               <<QMediaDevices::audioOutputs()[i].maximumChannelCount()<<QMediaDevices::audioOutputs()[i].minimumChannelCount()
//               <<QMediaDevices::audioOutputs()[i].maximumSampleRate()<<QMediaDevices::audioOutputs()[i].minimumSampleRate()
//               <<QMediaDevices::audioOutputs()[i].id()<<QMediaDevices::audioOutputs()[i].mode()
//               <<QMediaDevices::audioOutputs()[i].preferredFormat();
        //qdebug<<QMediaDevices::audioOutputs().at(i).description()<<QMediaDevices::audioOutputs().at(i).preferredFormat();
        ui->comboBoxo1->addItem(QMediaDevices::audioOutputs().at(i).description());
    }
    ui->comboBoxo1->setCurrentText(QMediaDevices::defaultAudioOutput().description());

    mediadevice = new QMediaDevices(this);

    videowidget = new QVideoWidget(this);
    //videowidget->setFullScreen(true);
    QHBoxLayout *hboxlayout = new QHBoxLayout(ui->widget);
    hboxlayout->addWidget(videowidget);

    mplayer->setVideoOutput(videowidget);

    //this->setStyleSheet("border:0px solid blue");

    QHBoxLayout *hboxlayoutt = new QHBoxLayout(this);
    hboxlayoutt->addWidget(ui->tabWidget);
    hboxlayoutt->addWidget(ui->widgetqml);
    //hboxlayoutt->setSpacing(0);

    ui->tabWidget->show();
    ui->widgetqml->hide();

    qmlWidget *qmlw = new qmlWidget(ui->widgetqml);
    QHBoxLayout *hboxlayoutt1 = new QHBoxLayout(ui->widgetqml);
    hboxlayoutt1->addWidget(qmlw);
    connect(qmlw,&qmlWidget::hidewidget,this,[=](){
        ui->tabWidget->show();
        ui->widgetqml->hide();
        emit isrotate90(false);
        this->showNormal();
    });

    //NetWorkwidget *netw = new NetWorkwidget(this);
    //netw->hide();

    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);
    CodeEditor *editor = new CodeEditor(ui->widgetcode);
    editor->setFont(font);
    editor->setPlainText("import QtQuick\n"
                           "Item {\n"
                            "   Rectangle {\n"
                            "       color:\"#00ff00\"\n"
                            "       anchors.fill: parent\n"
                            "   }\n"
                            "}");

    Highlighter *highlighter = new Highlighter(editor->document());
    Q_UNUSED(highlighter)

    connect(ui->pushButtoncodeopen,&QPushButton::clicked,this,[=](){
        QFileDialog filedialog(this);
        filedialog.setNameFilter("文件(*.qml)");
        filedialog.setViewMode(QFileDialog::Detail);
        filedialog.setAcceptMode(QFileDialog::AcceptOpen);
        filedialog.setFileMode(QFileDialog::ExistingFile);
        filedialog.exec();
        QStringList filelist = filedialog.selectedFiles();
        qdebug<<filelist;
        if(filelist.size()>0){
            QFile file(filelist[0]);
            if (file.open(QFile::ReadOnly | QFile::Text))
                editor->setPlainText(file.readAll());
        }
    });
    static QStringList strlist;
    QTextEdit *textedit = new QTextEdit(ui->widgetshow);
    textedit->setReadOnly(true);
    //QLabel *label = new QLabel(ui->widgetshow);
    QString qss;
    //不斜体、加粗、大小25个像素  字体颜色：#F0F0F0 边框宽度：2像素 实线 边框颜色：#AAAAAA  背景色：#000000
    qss.append("QTextEdit{font: normal bold 25px;color:#F0F0F0;border:2px solid #AAAAAA;background:#000000;}");
    //获得焦点时的样式
    qss.append("QTextEdit:focus{border:2px solid #00BB9E;background:#555555;}");
    //鼠标略过时的样式
    qss.append("QTextEdit:hover{color: red;border-color: green;background-color: aqua;}");
    textedit->setStyleSheet(qss);
    textedit->hide();
    QQuickWidget *qw = new QQuickWidget(ui->widgetshow);
    qw->setResizeMode(QQuickWidget::SizeRootObjectToView);
    //qw->engine()->addImportPath("qrc:/");
    qw->rootContext()->setContextProperty("widget",this);
    //qw->setSource(QUrl("qrc:/qml/loaderwgt.qml"));
    qw->showFullScreen();
    connect(qw,&QQuickWidget::sceneGraphError,this,[=](QQuickWindow::SceneGraphError error, const QString &message){
        qdebug<<error<<message;
    });
    connect(qw,&QQuickWidget::statusChanged,this,[=](QQuickWidget::Status status){
        //qdebug<<status<<qw->errors()<<strlist.size();
        if(status == QQuickWidget::Ready || status == QQuickWidget::Error){
            // 加载完成
            if(strlist.size() > 0){
                QFile::remove(strlist[0]);
                //strlist.remove(0);
                strlist.removeFirst();
            }
            if(status == QQuickWidget::Error){
                const auto &errors = qw->errors();
                QString err;
                for(int i=0;i<errors.size();i++){
                    err.append(errors[i].toString());
                }
                textedit->setText(err);
                textedit->show();
                qw->hide();
            }
        }
    });

    connect(editor,&QPlainTextEdit::textChanged,this,[=](){
        QString name = "test.qml." + QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());
        QString filename = QCoreApplication::applicationDirPath() + "/" + name;
        strlist.append(filename);
        QFile file(filename);
        if(file.open(QIODevice::WriteOnly)){
            file.write(editor->toPlainText().toLatin1());
            file.close();
            textedit->hide();
            qw->show();
            qw->setSource(QUrl::fromLocalFile(file.fileName()));
        }
        //set_qmlQString(filename);
    });


    //QFile file("mainwindow.h");if (file.open(QFile::ReadOnly | QFile::Text))editor->setPlainText(file.readAll());
    QHBoxLayout *hboxlayouttcode = new QHBoxLayout(ui->widgetcode);
    hboxlayouttcode->addWidget(editor);
    QHBoxLayout *hboxlayouttshow = new QHBoxLayout(ui->widgetshow);
    ui->widgetshow->setMinimumHeight(300);
    hboxlayouttshow->addWidget(qw);
    hboxlayouttshow->addWidget(textedit);

    QHBoxLayout *hboxlayouttlist = new QHBoxLayout(ui->widgetlist);
    ListW *listw = new ListW(ui->widgetlist);
    hboxlayouttlist->addWidget(listw);

    GraphicsWidget *ghw = new GraphicsWidget(ui->tab_7);
    QHBoxLayout *hboxlayoutghw = new QHBoxLayout(ui->tab_7);
    hboxlayoutghw->addWidget(ghw);

    WeatherWidget *weather = new WeatherWidget(ui->tab_8);
    QHBoxLayout *hboxlayouttweather = new QHBoxLayout(ui->tab_8);
    hboxlayouttweather->addWidget(weather);

    // QFile file(":/qss/QComboBox.qss");
    // if(file.open(QIODevice::ReadOnly)){
    //     ui->tabWidget->setStyleSheet(file.readAll());
    //     file.close();
    // }


    //testwk = new TestWk;
    //testwk->startrun();
}

Widget::~Widget()
{
    if(testwk){
        delete testwk;
    }
    if(audiosource){
        audiosource->stop();
        delete audiosource;
    }
    if(iodevice){
        iodevice->close();
        delete iodevice;
    }
    if(audiosink){
        audiosink->stop();
        delete audiosink;
    }
    if(iodeviceo){
        iodeviceo->close();
        delete iodeviceo;
    }
    delete ui;
}

void Widget::on_textEdit_tcp_textChanged()
{
    //ui->textEdit_tcp->moveCursor(QTextCursor::End);
}


void Widget::on_textEdit_udp_textChanged()
{
    //ui->textEdit_udp->moveCursor(QTextCursor::End);
}


void Widget::on_pushButton_tcp_clicked()
{
    if(tcpserver->isopen()){
        tcpserver->closeserver();
        ui->pushButton_tcp->setText("打开");
    } else {
        bool ok;
        quint16 port = ui->lineEdit_tcp->text().toUShort(&ok);
        if(ok == false){
            QMessageBox::information(this,"提示","端口错误");
        } else {
            if(true == tcpserver->openserver(port)){
                ui->pushButton_tcp->setText("关闭");
            }
        }
    }
}


void Widget::on_pushButton_udp_clicked()
{
    if(udp->isopen()){
        udp->close();
        ui->pushButton_udp->setText("打开");
    } else {
        bool ok;
        quint16 port = ui->lineEdit_udp->text().toUShort(&ok);
        if(ok == false){
            QMessageBox::information(this,"提示","端口错误");
        } else {
            if(true == udp->open(port)){
                ui->pushButton_udp->setText("关闭");
            }
        }
    }
}


void Widget::on_pushButton_clicked()
{
    mplayer->stop();
    QFileDialog filedialog(this);
    filedialog.setNameFilter("音乐(*)");
    filedialog.setViewMode(QFileDialog::Detail);
    filedialog.setAcceptMode(QFileDialog::AcceptOpen);
    filedialog.setFileMode(QFileDialog::ExistingFile);
    //filedialog.setDirectory("D:/杂项/下载");
    filedialog.exec();
    QStringList filelist = filedialog.selectedFiles();
    qdebug<<filelist;
    QString str = "D:/work/GitHub/whisper/whisper.cpp/samples/1.mp3";
    if(filelist.size()>0){
        str = filelist[0];
    }
    ui->lineEdit->setText(str);
    /* 这是qt6.5新增的bug，在6.5.1修复了支持中文的路径，
     * 因为6.5开始多媒体模块内核默认切换到ffmpeg，而没有对ffmpeg加入中文支持，
     * 路径怎么转也没用。只要增加一句即可。
     * main函数最前面增加 qputenv("QT_MEDIA_BACKEND", "windows");
     * */
    mplayer->setSource(str);
}


void Widget::on_pushButton_2_clicked()
{
    mplayer->play();
}


void Widget::on_pushButton_4_clicked()
{
    mplayer->pause();
    //mplayer->setPosition(position);
}


void Widget::on_pushButton_3_clicked()
{
    mplayer->stop();
}


void Widget::on_pushButton_5_clicked()
{
    volume += 0.1;
    if(volume > 1){
        volume = 1;
    }
    audioOutput->setVolume(volume);
}


void Widget::on_pushButton_6_clicked()
{
    volume -= 0.1;
    if(volume < 0){
        volume = 0;
    }
    audioOutput->setVolume(volume);
}


void Widget::on_pushButton_7_clicked()
{
    if(audiosource){
        return;
    }
    //audiosource->stop();
    //iodevice->close();

    QAudioFormat format;
    format.setSampleRate(ui->spinBox1->value());
    format.setChannelCount(ui->spinBox2->value());
    if(ui->comboBox2->currentText() == "Unknown"){
        format.setSampleFormat(QAudioFormat::Unknown);
    } else if(ui->comboBox2->currentText() == "UInt8"){
        format.setSampleFormat(QAudioFormat::UInt8);
    } else if(ui->comboBox2->currentText() == "Int16"){
        format.setSampleFormat(QAudioFormat::Int16);
    } else if(ui->comboBox2->currentText() == "Int32"){
        format.setSampleFormat(QAudioFormat::Int32);
    } else if(ui->comboBox2->currentText() == "Float"){
        format.setSampleFormat(QAudioFormat::Float);
    } else if(ui->comboBox2->currentText() == "NSampleFormats"){
        format.setSampleFormat(QAudioFormat::NSampleFormats);
    } else {
        qdebug<<QString("not found format:%1").arg(ui->comboBox2->currentText());
        return;
    }

    if(ui->comboBox3->currentText() == "ChannelConfigUnknown"){
        format.setChannelConfig(QAudioFormat::ChannelConfigUnknown);
    } else if(ui->comboBox3->currentText() == "ChannelConfigMono"){
        format.setChannelConfig(QAudioFormat::ChannelConfigMono);
    } else if(ui->comboBox3->currentText() == "ChannelConfigStereo"){
        format.setChannelConfig(QAudioFormat::ChannelConfigStereo);
    } else if(ui->comboBox3->currentText() == "ChannelConfig2Dot1"){
        format.setChannelConfig(QAudioFormat::ChannelConfig2Dot1);
    } else if(ui->comboBox3->currentText() == "ChannelConfig3Dot0"){
        format.setChannelConfig(QAudioFormat::ChannelConfig3Dot0);
    } else if(ui->comboBox3->currentText() == "ChannelConfigSurround5Dot0"){
        format.setChannelConfig(QAudioFormat::ChannelConfigSurround5Dot0);
    } else if(ui->comboBox3->currentText() == "ChannelConfigSurround5Dot1"){
        format.setChannelConfig(QAudioFormat::ChannelConfigSurround5Dot1);
    } else if(ui->comboBox3->currentText() == "ChannelConfigSurround7Dot0"){
        format.setChannelConfig(QAudioFormat::ChannelConfigSurround7Dot0);
    } else if(ui->comboBox3->currentText() == "ChannelConfig3Dot1"){
        format.setChannelConfig(QAudioFormat::ChannelConfig3Dot1);
    } else if(ui->comboBox3->currentText() == "ChannelConfigSurround7Dot1"){
        format.setChannelConfig(QAudioFormat::ChannelConfigSurround7Dot1);
    } else {
        qdebug<<QString("not found ChannelConfig:%1").arg(ui->comboBox3->currentText());
        return;
    }

    qsizetype i;
    QString devname = ui->comboBox1->currentText();
    for(i=0;i<QMediaDevices::audioInputs().size();i++){
        if(devname == QMediaDevices::audioInputs().at(i).description()){
            break;
        }
    }
    if(i >= QMediaDevices::audioInputs().size()){
        qdebug<<QString("not found device:%1").arg(devname);
        return;
    }

    QAudioDevice info(QMediaDevices::audioInputs().at(i));
    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }

    audiosource = new QAudioSource(format,this);
    iodevice = new AudioIODevice(this);
    connect(iodevice,&AudioIODevice::sigrxaudiodata,this,[=](QByteArray &data){
        //qdebug<<data.size();
        if(iodeviceo != nullptr && iodeviceo->isOpen() && ui->checkBox->isChecked()){
            iodeviceo->addData(data);
            if(ioout)ioout->write(data);
        }
    });
    connect(audiosource, &QAudioSource::stateChanged, this, [=](QAudio::State newState){
        qdebug<<newState;
    });
    iodevice->open(QIODevice::WriteOnly | QIODevice::Truncate);
    audiosource->start(iodevice);
}


void Widget::on_pushButton_8_clicked()
{
    if(nullptr != audiosource){
        audiosource->stop();
        delete audiosource;
        audiosource = nullptr;
    }
    if(iodevice != nullptr){
        iodevice->close();
        delete iodevice;
        iodevice = nullptr;
    }
}


void Widget::on_comboBox1_currentTextChanged(const QString &arg1)
{
    for(int i=0;i<QMediaDevices::audioInputs().size();i++){
        if(arg1 == QMediaDevices::audioInputs().at(i).description()){
            ui->comboBox2->clear();
            QList<QAudioFormat::SampleFormat> sfmat = QMediaDevices::audioInputs().at(i).supportedSampleFormats();
            for(int j=0;j<sfmat.size();j++){
                switch (sfmat[j]) {
                case QAudioFormat::Unknown:
                    ui->comboBox2->addItem("Unknown");
                    break;
                case QAudioFormat::UInt8:
                    ui->comboBox2->addItem("UInt8");
                    break;
                case QAudioFormat::Int16:
                    ui->comboBox2->addItem("Int16");
                    break;
                case QAudioFormat::Int32:
                    ui->comboBox2->addItem("Int32");
                    break;
                case QAudioFormat::Float:
                    ui->comboBox2->addItem("Float");
                    break;
                case QAudioFormat::NSampleFormats:
                    ui->comboBox2->addItem("NSampleFormats");
                    break;
                default:
                    break;
                }
            }

            ui->comboBox3->clear();
            ui->comboBox3->addItem("ChannelConfigUnknown");
            ui->comboBox3->addItem("ChannelConfigMono");
            ui->comboBox3->addItem("ChannelConfigStereo");
            ui->comboBox3->addItem("ChannelConfig2Dot1");
            ui->comboBox3->addItem("ChannelConfig3Dot0");
            ui->comboBox3->addItem("ChannelConfig3Dot1");
            ui->comboBox3->addItem("ChannelConfigSurround5Dot0");
            ui->comboBox3->addItem("ChannelConfigSurround5Dot1");
            ui->comboBox3->addItem("ChannelConfigSurround7Dot0");
            ui->comboBox3->addItem("ChannelConfigSurround7Dot1");

            ui->spinBox1->setMaximum(QMediaDevices::audioInputs().at(i).maximumSampleRate());
            ui->spinBox1->setMinimum(QMediaDevices::audioInputs().at(i).minimumSampleRate());

            ui->spinBox2->setMaximum(QMediaDevices::audioInputs().at(i).maximumChannelCount());
            ui->spinBox2->setMinimum(QMediaDevices::audioInputs().at(i).minimumChannelCount());

            switch (QMediaDevices::audioInputs().at(i).preferredFormat().sampleFormat()) {
            case QAudioFormat::Unknown:
                ui->comboBox2->setCurrentText("Unknown");
                break;
            case QAudioFormat::UInt8:
                ui->comboBox2->setCurrentText("UInt8");
                break;
            case QAudioFormat::Int16:
                ui->comboBox2->setCurrentText("Int16");
                break;
            case QAudioFormat::Int32:
                ui->comboBox2->setCurrentText("Int32");
                break;
            case QAudioFormat::Float:
                ui->comboBox2->setCurrentText("Float");
                break;
            case QAudioFormat::NSampleFormats:
                ui->comboBox2->setCurrentText("NSampleFormats");
                break;
            default:
                break;
            }

            switch (QMediaDevices::audioInputs().at(i).channelConfiguration()) {
            case QAudioFormat::ChannelConfigUnknown:
                ui->comboBox3->setCurrentText("ChannelConfigUnknown");
                break;
            case QAudioFormat::ChannelConfigMono:
                ui->comboBox3->setCurrentText("ChannelConfigMono");
                break;
            case QAudioFormat::ChannelConfigStereo:
                ui->comboBox3->setCurrentText("ChannelConfigStereo");
                break;
            case QAudioFormat::ChannelConfig2Dot1:
                ui->comboBox3->setCurrentText("ChannelConfig2Dot1");
                break;
            case QAudioFormat::ChannelConfig3Dot0:
                ui->comboBox3->setCurrentText("ChannelConfig3Dot0");
                break;
            case QAudioFormat::ChannelConfigSurround5Dot0:
                ui->comboBox3->setCurrentText("ChannelConfigSurround5Dot0");
                break;
            case QAudioFormat::ChannelConfigSurround5Dot1:
                ui->comboBox3->setCurrentText("ChannelConfigSurround5Dot1");
                break;
            case QAudioFormat::ChannelConfigSurround7Dot0:
                ui->comboBox3->setCurrentText("ChannelConfigSurround7Dot0");
                break;
            case QAudioFormat::ChannelConfig3Dot1:
                ui->comboBox3->setCurrentText("ChannelConfig3Dot1");
                break;
            case QAudioFormat::ChannelConfigSurround7Dot1:
                ui->comboBox3->setCurrentText("ChannelConfigSurround7Dot1");
                break;
            default:
                break;
            }

            ui->spinBox1->setValue(QMediaDevices::audioInputs().at(i).preferredFormat().sampleRate());
            ui->spinBox2->setValue(QMediaDevices::audioInputs().at(i).preferredFormat().channelCount());

            break;
        }
    }
}


void Widget::on_comboBoxo1_currentTextChanged(const QString &arg1)
{
    for(int i=0;i<QMediaDevices::audioOutputs().size();i++){
        if(arg1 == QMediaDevices::audioOutputs().at(i).description()){
            ui->comboBoxo2->clear();
            QList<QAudioFormat::SampleFormat> sfmat = QMediaDevices::audioOutputs().at(i).supportedSampleFormats();
            for(int j=0;j<sfmat.size();j++){
                switch (sfmat[j]) {
                case QAudioFormat::Unknown:
                    ui->comboBoxo2->addItem("Unknown");
                    break;
                case QAudioFormat::UInt8:
                    ui->comboBoxo2->addItem("UInt8");
                    break;
                case QAudioFormat::Int16:
                    ui->comboBoxo2->addItem("Int16");
                    break;
                case QAudioFormat::Int32:
                    ui->comboBoxo2->addItem("Int32");
                    break;
                case QAudioFormat::Float:
                    ui->comboBoxo2->addItem("Float");
                    break;
                case QAudioFormat::NSampleFormats:
                    ui->comboBoxo2->addItem("NSampleFormats");
                    break;
                default:
                    break;
                }
            }
            ui->comboBoxo3->clear();
            ui->comboBoxo3->addItem("ChannelConfigUnknown");
            ui->comboBoxo3->addItem("ChannelConfigMono");
            ui->comboBoxo3->addItem("ChannelConfigStereo");
            ui->comboBoxo3->addItem("ChannelConfig2Dot1");
            ui->comboBoxo3->addItem("ChannelConfig3Dot0");
            ui->comboBoxo3->addItem("ChannelConfig3Dot1");
            ui->comboBoxo3->addItem("ChannelConfigSurround5Dot0");
            ui->comboBoxo3->addItem("ChannelConfigSurround5Dot1");
            ui->comboBoxo3->addItem("ChannelConfigSurround7Dot0");
            ui->comboBoxo3->addItem("ChannelConfigSurround7Dot1");

            ui->spinBoxo1->setMaximum(QMediaDevices::audioOutputs().at(i).maximumSampleRate());
            ui->spinBoxo1->setMinimum(QMediaDevices::audioOutputs().at(i).minimumSampleRate());

            ui->spinBoxo2->setMaximum(QMediaDevices::audioOutputs().at(i).maximumChannelCount());
            ui->spinBoxo2->setMinimum(QMediaDevices::audioOutputs().at(i).minimumChannelCount());

            switch (QMediaDevices::audioOutputs().at(i).preferredFormat().sampleFormat()) {
            case QAudioFormat::Unknown:
                ui->comboBoxo2->setCurrentText("Unknown");
                break;
            case QAudioFormat::UInt8:
                ui->comboBoxo2->setCurrentText("UInt8");
                break;
            case QAudioFormat::Int16:
                ui->comboBoxo2->setCurrentText("Int16");
                break;
            case QAudioFormat::Int32:
                ui->comboBoxo2->setCurrentText("Int32");
                break;
            case QAudioFormat::Float:
                ui->comboBoxo2->setCurrentText("Float");
                break;
            case QAudioFormat::NSampleFormats:
                ui->comboBoxo2->setCurrentText("NSampleFormats");
                break;
            default:
                break;
            }

            switch (QMediaDevices::audioOutputs().at(i).channelConfiguration()) {
            case QAudioFormat::ChannelConfigUnknown:
                ui->comboBoxo3->setCurrentText("ChannelConfigUnknown");
                break;
            case QAudioFormat::ChannelConfigMono:
                ui->comboBoxo3->setCurrentText("ChannelConfigMono");
                break;
            case QAudioFormat::ChannelConfigStereo:
                ui->comboBoxo3->setCurrentText("ChannelConfigStereo");
                break;
            case QAudioFormat::ChannelConfig2Dot1:
                ui->comboBoxo3->setCurrentText("ChannelConfig2Dot1");
                break;
            case QAudioFormat::ChannelConfig3Dot0:
                ui->comboBoxo3->setCurrentText("ChannelConfig3Dot0");
                break;
            case QAudioFormat::ChannelConfigSurround5Dot0:
                ui->comboBoxo3->setCurrentText("ChannelConfigSurround5Dot0");
                break;
            case QAudioFormat::ChannelConfigSurround5Dot1:
                ui->comboBoxo3->setCurrentText("ChannelConfigSurround5Dot1");
                break;
            case QAudioFormat::ChannelConfigSurround7Dot0:
                ui->comboBoxo3->setCurrentText("ChannelConfigSurround7Dot0");
                break;
            case QAudioFormat::ChannelConfig3Dot1:
                ui->comboBoxo3->setCurrentText("ChannelConfig3Dot1");
                break;
            case QAudioFormat::ChannelConfigSurround7Dot1:
                ui->comboBoxo3->setCurrentText("ChannelConfigSurround7Dot1");
                break;
            default:
                break;
            }

            ui->spinBoxo1->setValue(QMediaDevices::audioOutputs().at(i).preferredFormat().sampleRate());
            ui->spinBoxo2->setValue(QMediaDevices::audioOutputs().at(i).preferredFormat().channelCount());

            break;
        }
    }
}


void Widget::on_checkBox_stateChanged(int arg1)
{
    qdebug<<arg1;
    if(!arg1){
        if(audiosink != nullptr){
            audiosink->stop();
            delete audiosink;
            audiosink = nullptr;
        }
        if(iodeviceo != nullptr){
            iodeviceo->close();
            delete iodeviceo;
            iodeviceo = nullptr;
        }
        return;
    }

    QAudioFormat format;
    format.setSampleRate(ui->spinBoxo1->value());
    format.setChannelCount(ui->spinBoxo2->value());
    if(ui->comboBoxo2->currentText() == "Unknown"){
        format.setSampleFormat(QAudioFormat::Unknown);
    } else if(ui->comboBoxo2->currentText() == "UInt8"){
        format.setSampleFormat(QAudioFormat::UInt8);
    } else if(ui->comboBoxo2->currentText() == "Int16"){
        format.setSampleFormat(QAudioFormat::Int16);
    } else if(ui->comboBoxo2->currentText() == "Int32"){
        format.setSampleFormat(QAudioFormat::Int32);
    } else if(ui->comboBoxo2->currentText() == "Float"){
        format.setSampleFormat(QAudioFormat::Float);
    } else if(ui->comboBoxo2->currentText() == "NSampleFormats"){
        format.setSampleFormat(QAudioFormat::NSampleFormats);
    } else {
        qdebug<<QString("not found format:%1").arg(ui->comboBoxo2->currentText());
        return;
    }

    if(ui->comboBoxo3->currentText() == "ChannelConfigUnknown"){
        format.setChannelConfig(QAudioFormat::ChannelConfigUnknown);
    } else if(ui->comboBoxo3->currentText() == "ChannelConfigMono"){
        format.setChannelConfig(QAudioFormat::ChannelConfigMono);
    } else if(ui->comboBoxo3->currentText() == "ChannelConfigStereo"){
        format.setChannelConfig(QAudioFormat::ChannelConfigStereo);
    } else if(ui->comboBoxo3->currentText() == "ChannelConfig2Dot1"){
        format.setChannelConfig(QAudioFormat::ChannelConfig2Dot1);
    } else if(ui->comboBoxo3->currentText() == "ChannelConfig3Dot0"){
        format.setChannelConfig(QAudioFormat::ChannelConfig3Dot0);
    } else if(ui->comboBoxo3->currentText() == "ChannelConfigSurround5Dot0"){
        format.setChannelConfig(QAudioFormat::ChannelConfigSurround5Dot0);
    } else if(ui->comboBoxo3->currentText() == "ChannelConfigSurround5Dot1"){
        format.setChannelConfig(QAudioFormat::ChannelConfigSurround5Dot1);
    } else if(ui->comboBoxo3->currentText() == "ChannelConfigSurround7Dot0"){
        format.setChannelConfig(QAudioFormat::ChannelConfigSurround7Dot0);
    } else if(ui->comboBoxo3->currentText() == "ChannelConfig3Dot1"){
        format.setChannelConfig(QAudioFormat::ChannelConfig3Dot1);
    } else if(ui->comboBoxo3->currentText() == "ChannelConfigSurround7Dot1"){
        format.setChannelConfig(QAudioFormat::ChannelConfigSurround7Dot1);
    } else {
        qdebug<<QString("not found ChannelConfig:%1").arg(ui->comboBoxo3->currentText());
        return;
    }

    qsizetype i;
    QString devname = ui->comboBoxo1->currentText();
    for(i=0;i<QMediaDevices::audioOutputs().size();i++){
        if(devname == QMediaDevices::audioOutputs().at(i).description()){
            break;
        }
    }
    if(i >= QMediaDevices::audioOutputs().size()){
        qdebug<<QString("not found device:%1").arg(devname);
        return;
    }

    QAudioDevice info(QMediaDevices::audioOutputs().at(i));
    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }
#if 0
    QAudioDevice device = mediadevice->defaultAudioOutput();
    audiosink = new QAudioSink(device,device.preferredFormat());
#else
    audiosink = new QAudioSink(format,this);
#endif
    connect(audiosink, &QAudioSink::stateChanged, this, [=](QAudio::State newState){
        //qdebug<<newState;
        switch (newState) {
        case QAudio::IdleState:
            // Finished playing (no more data)
            //audiosink->stop();
            break;

        case QAudio::StoppedState:
            // Stopped for other reasons
            if (audiosink->error() != QAudio::NoError) {
                // Error handling
                qdebug<<audiosink->error();
            }
            break;

        default:
            // ... other cases as appropriate
            break;
        }
    });

    iodeviceo = new AudioIODevice(this);
    iodeviceo->open(QIODevice::ReadOnly);
    //audiosink->start(iodeviceo);
    ioout = audiosink->start();
}


void Widget::on_tabWidget_tabBarDoubleClicked(int index)
{
    if(index == 3){
        ui->tabWidget->hide();
        ui->widgetqml->show();
        this->showFullScreen();
    }
}


void Widget::on_pushButtonmw_clicked()
{
    ui->tabWidget->hide();
    ui->widgetqml->show();
    emit isrotate90(true);
    this->showFullScreen();
}

