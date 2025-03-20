
#include "tcpserver.h"
#include <QDebug>
#include <QNetworkInterface>
#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__

TcpServer::TcpServer(QObject *parent)
    : QObject{parent},server(nullptr)
{
}

TcpServer::~TcpServer()
{
    if(server != nullptr){
        server->close();
        server->deleteLater();
    }
}

bool TcpServer::isopen()
{
    if(server == nullptr){
        return false;
    }
    return true;
}

bool TcpServer::closeserver()
{
    if(server != nullptr){
        server->close();
        server->deleteLater();
        server = nullptr;
    }
    return true;
}

bool TcpServer::openserver(quint16 port)
{
    if(server == nullptr){
        server = new QTcpServer(this);
        if(false == server->listen(QHostAddress::Any,port)){
            QString str = server->errorString();
            emit allinfo(str);
            qdebug<<str;
            server->deleteLater();
            server = nullptr;
            return false;
        } else {
            QString str("主机网卡信息:");
            emit allinfo(str);
            QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
            QStringList strlist;
            //获取所有网络接口的列表
            foreach(QNetworkInterface interface,list)
            {
                QString str;
                str.append("Device:");
                str.append(interface.name());
                str.append("\t");
                str.append("HardwareAddress:");
                str.append(interface.hardwareAddress());
                QList<QNetworkAddressEntry> entryList = interface.addressEntries();
                //获取IP地址条目列表，每个条目中包含一个IP地址，一个子网掩码和一个广播地址
                foreach(QNetworkAddressEntry entry,entryList)
                {
                    str.append("\n\tIP Address: ");
                    str.append(entry.ip().toString());
                    str.append("\n\tNetmask: ");
                    str.append(entry.netmask().toString());
                    str.append("\n\tBroadcast: ");
                    str.append(entry.broadcast().toString());
                    str.append("\n");
                }
                qdebug<<str;
                strlist.append(str);
            }
            str = strlist.join("\n");
            emit allinfo(str);

            connect(server,SIGNAL(newConnection()),this,SLOT(SlotnewConnection()));
            connect(server,&QTcpServer::acceptError,this,[&](QAbstractSocket::SocketError socketError){
                QTcpServer *server = qobject_cast<QTcpServer *>(sender());
                qdebug<<socketError<<server->errorString()<<server->isListening();
            });
        }
    }
    return true;
}

void TcpServer::SlotnewConnection()
{
    QTcpSocket *socket;
    while((socket = server->nextPendingConnection()) != nullptr){
        QString str = "新连接:";
        str.append(QString("[ip:%1,port:%2]").arg(socket->peerAddress().toString()).arg(socket->peerPort()));
        str.append("\n");
        emit allinfo(str);
        qdebug<<str;
        connect(socket,&QTcpSocket::readyRead,this,
                [&](){
                    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
                    QByteArray data = socket->readAll();
                    //qdebug<<socket<<socket->peerAddress()<<socket->peerName()<<socket->peerPort()<<socket->readAll();
                    QString str = QString("接收数据:%1:%2 %3Byte")
                                      .arg(socket->peerAddress().toString())
                                      .arg(socket->peerPort()).arg(data.size());
                    if(data.size() < 1024){
                        str += QString("\n数据:%1").arg(QString(data));
                    }
                    str.append("\n");
                    emit allinfo(str);
                    qdebug<<str;
                    socket->write(data);
                });
        connect(socket,&QTcpSocket::disconnected,this,
                [&](){
                    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
                    QString str = QString("连接断开:%1:%2\n")
                                      .arg(socket->peerAddress().toString())
                                      .arg(socket->peerPort());
                    emit allinfo(str);
                    qdebug<<str;
                    socket->deleteLater();
                });
    }
}

