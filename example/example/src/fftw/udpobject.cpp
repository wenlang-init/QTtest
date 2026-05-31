#include "udpobject.h"
#include <QDebug>

udpObject::udpObject(QObject *parent)
    : QObject{parent}
{
    udpsocket = new QUdpSocket(this);
    connect(udpsocket, &QUdpSocket::readyRead, this, [&]() {
        QByteArray data;
        QHostAddress addr;
        quint16 port;
        qint64 rcnt = 0;
        qint64 pendingsize = udpsocket->pendingDatagramSize();

        if (pendingsize < 0) {
            qDebug() << pendingsize << udpsocket->errorString();
            return;
        }

        if (pendingsize == 0) {
            return;
        }
        data.resize(pendingsize);

        while (rcnt < pendingsize) {
            qint64 cnt =
                udpsocket->readDatagram(data.data() + rcnt, pendingsize - rcnt,
                                        &addr,
                                        &port);

            if (cnt <= 0) {
                qDebug() << udpsocket->errorString();
                break;
            }
            QString str = "recv data:";
            str += addr.toString() + ":" + QString::number(port) +
                   " size:" + QString::number(cnt);
            qDebug() << str;
            rcnt += cnt;
        }
        emit readData(data);
    });
}

bool udpObject::bindAll(qint16 port)
{
    if (isbind) return true;

    if (false == udpsocket->bind(QHostAddress::Any, port)) {
        return false;
    }
    isbind = true;
    qDebug() << QString("have opened,%1:%2").
        arg(udpsocket->localAddress().toString()).
        arg(udpsocket->localPort());
    return true;
}

int udpObject::writeData(const QByteArray  & data,
                         const QHostAddress& host,
                         const quint16     & port)
{
    if (!isbind) return -1;

    return udpsocket->writeDatagram(data, host, port);
}
