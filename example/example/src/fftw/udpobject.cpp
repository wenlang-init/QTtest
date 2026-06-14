#include "udpobject.h"
#include <QDebug>
#include <QDateTime>

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
            qWarning() << pendingsize << udpsocket->errorString();
            return;
        }

        if (pendingsize == 0) {
            return;
        }

        if (0) {
            // 数据速度
            static QDateTime datetime = QDateTime::currentDateTime();
            static qint64    _size = 0;
            _size += pendingsize;
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

                qDebug() << "recv:" << str;

                datetime = QDateTime::currentDateTime();
                _size = 0;
            }
        }

        data.resize(pendingsize);

        while (rcnt < pendingsize) {
            qint64 cnt =
                udpsocket->readDatagram(data.data() + rcnt, pendingsize - rcnt,
                                        &addr,
                                        &port);

            if (cnt <= 0) {
                qWarning() << udpsocket->errorString();
                break;
            }
            QString str = "recv data:";
            str += addr.toString() + ":" + QString::number(port) +
                   " size:" + QString::number(cnt);
            qInfo() << str;
            rcnt += cnt;
        }
        emit readData(data);
    });
}

bool udpObject::bindAll(qint16 port)
{
    if (isbind) return true;

    if (false == udpsocket->bind(QHostAddress::Any, port)) {
        qWarning() << QString("bind failed,%1:%2,%3").
            arg(udpsocket->localAddress().toString()).
            arg(udpsocket->localPort()).
            arg(udpsocket->errorString());
        return false;
    }
    isbind = true;
    qInfo() << QString("have opened,%1:%2").
        arg(udpsocket->localAddress().toString()).
        arg(udpsocket->localPort());
    return true;
}

int udpObject::writeData(const QByteArray  & data,
                         const QHostAddress& host,
                         const quint16     & port,
                         const qsizetype   & sendPackSize)
{
    if (!isbind) return -1;

    qsizetype cnt = 0;
    qsizetype len;
    qsizetype ret = 0;

    while (cnt < data.size()) {
        len = data.size() - cnt;

        if (len > sendPackSize) len = sendPackSize;
        ret = udpsocket->writeDatagram(data.data() + cnt, len, host, port);

        if (ret < 0) {
            qWarning() << udpsocket->errorString();
            return cnt;
        }
        cnt += ret;
    }
    return cnt;
}
