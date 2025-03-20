
#include "udpobj.h"
#include <QDebug>

#define qdebug qDebug()<<__FILE__<<__LINE__<<__FUNCTION__

Udpobj::Udpobj(QObject *parent)
    : QObject{parent},udpsocket(nullptr)
{

}

Udpobj::~Udpobj()
{
    if(udpsocket != nullptr){
        udpsocket->close();
        udpsocket->deleteLater();
    }
}

bool Udpobj::open(quint16 port)
{
    if(udpsocket == nullptr){
        udpsocket = new QUdpSocket(this);
        if(false == udpsocket->bind(port,QUdpSocket::ShareAddress)){
            QString str = udpsocket->errorString();
            emit allinfo(str);
            qdebug<<str;
            udpsocket->deleteLater();
            udpsocket = nullptr;
            return false;
        } else {
            //udpsocket->joinMulticastGroup();
            //udpsocket->leaveMulticastGroup();
            connect(udpsocket,&QUdpSocket::readyRead,this,
                    [&](){
                        QByteArray data;
                        data.resize(udpsocket->pendingDatagramSize());
                        QHostAddress addr;
                        quint16 port;
                        udpsocket->readDatagram(data.data(),udpsocket->pendingDatagramSize(),&addr,&port);
                        //qdebug<<socket<<socket->peerAddress()<<socket->peerName()<<socket->peerPort()<<socket->readAll();
                        QString str = QString("接收数据:%1:%2 %3Byte\n")
                                          .arg(addr.toString())
                                          .arg(port).arg(data.size());
                        emit allinfo(str);
                        qdebug<<str;
                        udpsocket->writeDatagram(data,addr,port);
                    });
        }
    }
    return true;
}

bool Udpobj::close()
{
    if(udpsocket != nullptr){
        udpsocket->close();
        udpsocket->deleteLater();
        udpsocket = nullptr;
    }
    return true;
}

bool Udpobj::isopen()
{
    if(udpsocket == nullptr){
        return false;
    }
    return true;
}

