#include "udpslave.h"
#include "edcrypt.h"
#include <QNetworkInterface>
#include <QTimer>
#include <QtEndian>
#include <QDebug>
#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__

UdpSlave::UdpSlave(QObject *parent)
    : QObject{parent},udpsocket(nullptr)
{
    QTimer *timer = new QTimer(this);
    timer->start(3000);
    connect(timer,&QTimer::timeout,this,[=](){
        if(isopen() == false)return;
        if(m_isClient){
            QByteArray senddata;
            senddata.resize(2);
            senddata[0]= 0xAB;
            senddata[1] = 0xC7;
            udpsocket->writeDatagram(senddata,m_serverAddr,m_serverPort);
        }
        if(m_isServer){
            QByteArray senddata;
            senddata.resize(8);
            senddata[0]= 0xBA;
            senddata[1] = 0X7C;
            quint8 dekey = 0x1F;
            quint8 lenght = 8;
            senddata[3] = lenght;
            quint32 serverFlag = qToBigEndian(m_serverFlag);
            memcpy(&senddata[4],&serverFlag,4);
            dekey = encrypt((unsigned char *)(&senddata[4]),lenght - 4,dekey);
            senddata[2] = dekey;
            udpsocket->writeDatagram(senddata,m_serverAddr,m_serverPort);
        }
    });

    QTimer *timer1 = new QTimer(this);
    timer1->start(1000);
    connect(timer1,&QTimer::timeout,this,[=](){
        for(int i=0;i<m_serverIPNodeList.size();i++){
            if(10000 < m_serverIPNodeList[i].datetime.msecsTo(QDateTime::currentDateTime())){
                m_serverIPNodeList.remove(i);
                i--;
            }
        }
    });
}

UdpSlave::~UdpSlave()
{
    if(udpsocket != nullptr){
        udpsocket->close();
        udpsocket->deleteLater();
    }
}

bool UdpSlave::open(QHostAddress serverAddr,quint16 serverPort,quint16 localPort)
{
    if(udpsocket == nullptr){
        udpsocket = new QUdpSocket(this);
        if(false == udpsocket->bind(localPort,QUdpSocket::ShareAddress)){
            QString str = udpsocket->errorString();
            qdebug<<str;
            udpsocket->deleteLater();
            udpsocket = nullptr;
            return false;
        } else {
            qdebug<<QString("open success,localPort:%1").arg(m_localport);
            m_serverAddr = serverAddr;
            m_serverPort = serverPort;
            m_localport = localPort;
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
                parseServerIP(data);
                //udpsocket->writeDatagram(data,addr,port);
            });
        }
        return true;
    } else {
        qdebug<<QString("have opened,port:%1").arg(m_localport);
        return false;
    }
}

bool UdpSlave::close()
{
    if(udpsocket != nullptr){
        udpsocket->close();
        udpsocket->deleteLater();
        udpsocket = nullptr;
    }
    return true;
}

bool UdpSlave::isopen()
{
    if(udpsocket == nullptr){
        return false;
    }
    //qdebug<<QString("port:%1").arg(m_port);
    return true;
}

void UdpSlave::setIsServer(bool flag,quint32 serverFlag)
{
    m_isServer = flag;
    m_serverFlag = serverFlag;
}

void UdpSlave::setIsClient(bool flag)
{
    m_isClient = flag;
}

void UdpSlave::printNetInfo()
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

QList<ServerIPInfo> UdpSlave::getAllServerIPInfo()
{
    QList<ServerIPInfo> sipinfo;
    for(int i=0;i<m_serverIPNodeList.size();i++){
        sipinfo.append(m_serverIPNodeList[i].sIPInfo);
    }
    return sipinfo;
}

bool UdpSlave::parseServerIP(QByteArray &data)
{
    if(data.size() < 30){
        return false;
    }
    static quint8 headflag[2] = {0xAB,0xC7};
    if((quint8)data[0] != headflag[0] || (quint8)data[1] != headflag[1]){
        return false;
    }
    quint8 dekey = data[2];
    quint8 lenght = data[3];

    if(lenght != data.size()){
        return false;
    }

    decrypt((unsigned char *)(&data[4]),lenght - 4,dekey);

    int index = 4;
    quint32 serverFlag = *((quint32 *)&data[index]);
    serverFlag = qFromBigEndian(serverFlag);
    index += 4;

    bool isfound = false;
    for(int i=0;i<m_serverIPNodeList.size();i++){
        if(m_serverIPNodeList[i].sIPInfo.serverFlag == serverFlag){
            isfound = true;

            m_serverIPNodeList[i].sIPInfo.serverFlag = serverFlag;

            m_serverIPNodeList[i].sIPInfo.serverPort = qFromBigEndian(*(quint16*)&data[index]);
            index += 2;

            QHostAddress addr;
            quint32 ipv4 = qFromBigEndian(*(quint32*)&data[index]);
            addr.setAddress(ipv4);
            index += 4;
            Q_IPV6ADDR ipv6;
            memcpy(ipv6.c,&data[index],16);
            index += 16;
            addr.setAddress(ipv6);
            m_serverIPNodeList[i].sIPInfo.addr = addr;

            m_serverIPNodeList[i].datetime = QDateTime::currentDateTime();

            break;
        }
    }

    if(false == isfound){
        ServerIpNode node;

        node.sIPInfo.serverFlag = serverFlag;

        node.sIPInfo.serverPort = qFromBigEndian(*(quint16*)&data[index]);
        index += 2;

        QHostAddress addr;
        quint32 ipv4 = qFromBigEndian(*(quint32*)&data[index]);
        addr.setAddress(ipv4);
        index += 4;
        Q_IPV6ADDR ipv6;
        memcpy(ipv6.c,&data[index],16);
        index += 16;
        addr.setAddress(ipv6);
        node.sIPInfo.addr = addr;

        node.datetime = QDateTime::currentDateTime();

        m_serverIPNodeList.append(node);
    }

    printALLServerIp();

    return true;
}

void UdpSlave::printALLServerIp()
{
    for(int i=0;i<m_serverIPNodeList.size();i++){
        qdebug << QString::number(m_serverIPNodeList[i].sIPInfo.serverFlag,16)
        << m_serverIPNodeList[i].sIPInfo.serverPort
        << m_serverIPNodeList[i].sIPInfo.addr;
    }
}
