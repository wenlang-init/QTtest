#include "nettunnel.h"
#include "edcrypt.h"
#include <QNetworkInterface>
#include <QtEndian>
#include <QTimer>
#include <QDebug>
#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__

NetTunnel::NetTunnel(QObject *parent) : QObject(parent),udpsocket(nullptr)
{
    printNetInfo();

    QTimer *timer = new QTimer(this);
    timer->start(1000);
    connect(timer,&QTimer::timeout,this,[=](){
        for(int i=0;i<m_serverIPNodeList.size();i++){
            if(10000 < m_serverIPNodeList[i].datetime.msecsTo(QDateTime::currentDateTime())){
                m_serverIPNodeList.remove(i);
                i--;
            }
        }
    });
    char data[] = {0x00,0x00,0x00,0x01};
    printEncrypt(data,sizeof(data),0x13);
}

NetTunnel::~NetTunnel()
{
    if(udpsocket != nullptr){
        udpsocket->close();
        udpsocket->deleteLater();
    }
}

bool NetTunnel::open(quint16 port)
{
    if(udpsocket == nullptr){
        udpsocket = new QUdpSocket(this);
        if(false == udpsocket->bind(port,QUdpSocket::ShareAddress)){
            QString str = udpsocket->errorString();
            qdebug<<str;
            udpsocket->deleteLater();
            udpsocket = nullptr;
            return false;
        } else {
            m_port = port;
            //udpsocket->joinMulticastGroup();
            //udpsocket->leaveMulticastGroup();
            connect(udpsocket,&QUdpSocket::readyRead,this,[&](){
                QByteArray data;
                QHostAddress addr;
                quint16 port;
                qint64 rcnt = 0;
                qint64 pendingsize = udpsocket->pendingDatagramSize();
                if(pendingsize <= 0){
                    qdebug<<udpsocket->errorString();
                    return;
                }
                data.resize(pendingsize);
                while (rcnt < pendingsize) {
                    qint64 cnt = udpsocket->readDatagram(data.data() + rcnt,pendingsize - rcnt,&addr,&port);
                    if(cnt <= 0){
                        qdebug<<udpsocket->errorString();
                        break;
                    }
                    rcnt += cnt;
                }
                //qdebug<<socket<<socket->peerAddress()<<socket->peerName()<<socket->peerPort()<<socket->readAll();
                QString str = QString("接收数据:%1:%2 %3Byte,pendingsize=%4Byte\n")
                                  .arg(addr.toString())
                                  .arg(port).arg(rcnt).arg(pendingsize);
                qdebug << str;
                //qdebug << data;
                parseServerIP(data,addr,port);
                parseClientSendServerInfo(data,addr,port);
                //udpsocket->writeDatagram(data,addr,port);
            });
        }
        return true;
    } else {
        qdebug<<QString("have opened,port:%1").arg(m_port);
        return false;
    }
}

bool NetTunnel::close()
{
    if(udpsocket != nullptr){
        udpsocket->close();
        udpsocket->deleteLater();
        udpsocket = nullptr;
    }
    return true;
}

bool NetTunnel::isopen()
{
    if(udpsocket == nullptr){
        return false;
    }
    //qdebug<<QString("port:%1").arg(m_port);
    return true;
}

void NetTunnel::printNetInfo()
{
    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
    //QStringList strlist;
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
        //strlist.append(str);
    }
    //QString str = strlist.join("\n");
    //qdebug << str;
}

bool NetTunnel::parseServerIP(QByteArray &data,const QHostAddress &addr,const quint16 &port)
{
    if(data.size() < 8){
        return false;
    }
    static quint8 headflag[2] = {0xBA,0X7C};
    if((quint8)data[0] != headflag[0] || (quint8)data[1] != headflag[1]){
        return false;
    }
    quint8 dekey = data[2];
    quint8 lenght = data[3];
    if(lenght != data.size()){
        return false;
    }

    decrypt((unsigned char *)(&data[4]),lenght - 4,dekey);

    quint32 serverFlag = *((quint32 *)&data[4]);
    serverFlag = qFromBigEndian(serverFlag);

    bool isfound = false;
    for(int i=0;i<m_serverIPNodeList.size();i++){
        if(m_serverIPNodeList[i].sIPInfo.serverFlag == serverFlag){
            isfound = true;

            m_serverIPNodeList[i].sIPInfo.serverFlag = serverFlag;
            m_serverIPNodeList[i].sIPInfo.addr = addr;
            m_serverIPNodeList[i].sIPInfo.serverPort = port;
            m_serverIPNodeList[i].datetime = QDateTime::currentDateTime();

            break;
        }
    }

    if(false == isfound){
        ServerIpNode node;
        node.sIPInfo.serverFlag = serverFlag;
        node.sIPInfo.addr = addr;
        node.sIPInfo.serverPort = port;
        node.datetime = QDateTime::currentDateTime();

        m_serverIPNodeList.append(node);
    }

    //qdebug<<serverFlag<<addr<<port<<QDateTime::currentDateTime();

    return true;
}

bool NetTunnel::parseClientSendServerInfo(QByteArray &data,const QHostAddress &addr,const quint16 &port)
{
    if(false == isopen()){
        return false;
    }

    if(data.size() < 2){
        return false;
    }
    static quint8 headflag[2] = {0xAB,0xC7};
    if((quint8)data[0] != headflag[0] || (quint8)data[1] != headflag[1]){
        return false;
    }
    quint8 dekey = 0xCF;
    quint8 lenght = 4 + 4 + 2 + 4 + 16;

    for(int i=0;i<m_serverIPNodeList.size();i++){
        QByteArray senddata;
        senddata.resize(lenght);
        memcpy(senddata.data(),headflag,sizeof(headflag));
        //senddata[2] = dekey;
        int index = 3;
        senddata[index] = lenght;
        index++;

        quint32 serverFlag = qToBigEndian(m_serverIPNodeList[i].sIPInfo.serverFlag);
        memcpy(&senddata[index],&serverFlag,sizeof(serverFlag));
        index += 4;

        quint16 sport = qToBigEndian(m_serverIPNodeList[i].sIPInfo.serverPort);
        memcpy(&senddata[index],&sport,sizeof(sport));
        index += 2;

        quint32 ipv4 = m_serverIPNodeList[i].sIPInfo.addr.toIPv4Address();
        //memcpy(&senddata[index],&ipv4,sizeof(ipv4));
        senddata[index] = (char)((ipv4 >> 24) & 0xff);
        senddata[index+1] = (char)((ipv4 >> 16) & 0xff);
        senddata[index+2] = (char)((ipv4 >> 8) & 0xff);
        senddata[index+3] = (char)((ipv4 >> 0) & 0xff);
        index += 4;

        Q_IPV6ADDR ipv6 = m_serverIPNodeList[i].sIPInfo.addr.toIPv6Address();
        memcpy(&senddata[index],ipv6.c,16);
        index += 10;

        dekey = encrypt((unsigned char *)(&senddata[4]),lenght - 4,dekey);
        senddata[2] = dekey;

        //qdebug << senddata.toHex().toUpper();
        //qdebug << m_serverIPNodeList[i].sIPInfo.addr << m_serverIPNodeList[i].sIPInfo.serverPort;

        udpsocket->writeDatagram(senddata,addr,port);
    }
    return true;
}

void NetTunnel::printEncrypt(const void *data, unsigned int len,unsigned char key)
{
    QByteArray bdata((char*)data,len);
    qdebug<<bdata.toHex().toUpper();

    unsigned char dekey = encrypt((unsigned char *)(&bdata[0]),len,key);
    qdebug<<QString("enkey:0x%1").arg(QString::number(key,16));
    qdebug<<QString("dekey:0x%1").arg(QString::number(dekey,16));
    qdebug<<bdata.toHex().toUpper();

    decrypt((unsigned char *)(&bdata[0]),len,dekey);
    qdebug<<bdata.toHex().toUpper();
}
