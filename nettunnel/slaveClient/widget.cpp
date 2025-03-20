#include "widget.h"
#include "ui_widget.h"
#include "printFunction.h"
#include <QTimer>
#include <QDebug>
#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    //printColour(RED,"xxxxxxxxxxxxxxxxxxxxxxx\n");
    PRINT_LOG(LOG_TYPE_ENUM_DEBUG,"1111111111111\n");
    PRINT_LOG(LOG_TYPE_ENUM_WARRING,"2222222222222\n");
    PRINT_LOG(LOG_TYPE_ENUM_CRITICAL,"333333333\n");
    PRINT_LOG(LOG_TYPE_ENUM_FATAL,"444444444444\n");
    PRINT_LOG(LOG_TYPE_ENUM_INFO,"555555555555\n");
    ui->setupUi(this);
    udpSlave = new UdpSlave(this);
    udpSlave->setIsClient(true);
    connect(udpSlave,SIGNAL(recvUdpData(QByteArray &,const QHostAddress &,const quint16 &)),this,SLOT(recvUdpData(QByteArray &,const QHostAddress &,const quint16 &)));
    // windows下编译为动态dll时连接不上,使用QT的方式(也可以使用插件模式)来生成dll可解决https://blog.csdn.net/zzwdkxx/article/details/105477367
    // 用Q_DECL_EXPORT修饰class以表示导出生成lib,用Q_DECL_IMPORT修饰以表示导入使用lib
    // https://forum.qt.io/topic/102783/qobject-connect-signal-not-found-in-dll-with-pointer-to-member-function
    //connect(udpSlave,&UdpSlave::recvUdpData,this,[=](QByteArray &data){qdebug<<data.toHex().toUpper();});
    QTimer *timer = new QTimer(this);
    timer->start(1000);
    connect(timer,&QTimer::timeout,this,[&](){
        QList<ServerIPInfo> serverIPList = udpSlave->getAllServerIPInfo();
        if(serverIPList.size() != m_serverIPList.size()){
            m_serverIPList = serverIPList;
            reflushIPList();
            return;
        }
        bool ischanged = false;
        for(int i=0;i<m_serverIPList.size();i++){
            bool isfound = false;
            for(int j=0;j<serverIPList.size();j++){
                if(m_serverIPList[i].serverFlag == serverIPList[i].serverFlag){
                    if(m_serverIPList[i].addr != serverIPList[i].addr){
                        m_serverIPList[i].addr = serverIPList[i].addr;
                        ischanged = true;
                    }
                    if(m_serverIPList[i].serverPort != serverIPList[i].serverPort){
                        m_serverIPList[i].serverPort = serverIPList[i].serverPort;
                        ischanged = true;
                    }
                    isfound = true;
                    break;
                }
            }
            if(isfound == false){
                ischanged = true;
                m_serverIPList.remove(i);
                i--;
            }
        }
        if(ischanged == true){
            reflushIPList();
        }
    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::recvUdpData(QByteArray &data,const QHostAddress &addr,const quint16 &port)
{
    QString str = addr.toString() + ":" + QString::number(port) + "\n";
    str += QString::fromUtf8(data);
    QString sstr = ui->textEdit->toPlainText();
    if(sstr.size() > 1024*1024*128){
        ui->textEdit->clear();
        sstr.clear();
    }
    sstr += str + "\n";
    ui->textEdit->setPlainText(sstr);
    //ui->textEdit->setText(sstr);

    //qdebug<<data.toHex().toUpper()<<addr<<port;
}

void Widget::on_pushButton_clicked()
{
    ui->pushButton->setText(QStringLiteral("连接"));
    bool ok;
    QHostAddress serverAddr(ui->lineEdit->text());
    if(serverAddr.isNull())return;
    quint16 serverPort = ui->lineEditport->text().toUShort(&ok);
    if(!ok)return;
    if(false == udpSlave->open(serverAddr,serverPort)){
        udpSlave->close();
        ui->pushButton->setText(QStringLiteral("连接"));
    } else {
        ui->pushButton->setText(QStringLiteral("断开"));
    }
}

void Widget::reflushIPList()
{
    qdebug << "while reflush list";
    ui->comboBox->clear();
    for(int i=0;i<m_serverIPList.size();i++){
        QString str = QString::number(m_serverIPList[i].serverFlag,16);
        str += m_serverIPList[i].addr.toString() + ":" + QString::number(m_serverIPList[i].serverPort);

        ui->comboBox->addItem(str);
        qdebug << str;
    }
    if(ui->comboBox->count() > 0)
        ui->comboBox->setCurrentIndex(0);
}


void Widget::on_pushButtonsend_clicked()
{
    int index = ui->comboBox->currentIndex();
    if(index < 0)return;
    ServerIPInfo serverIPInfo = m_serverIPList[index];

    QByteArray senddata = ui->textEditsend->toPlainText().toUtf8();
    if(senddata.size() <= 0)return;
    qdebug << QString("send to %1:%2 serverFlag:%3 %4byte")
                  .arg(serverIPInfo.addr.toString())
                  .arg(serverIPInfo.serverPort)
                  .arg(QString::number(serverIPInfo.serverFlag,16))
                  .arg(senddata.size());
    qdebug<<udpSlave->sendData(senddata,serverIPInfo.addr,serverIPInfo.serverPort);
}

