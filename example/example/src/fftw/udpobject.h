#ifndef UDPOBJECT_H
#define UDPOBJECT_H

#include <QObject>
#include <QUdpSocket>

class udpObject : public QObject {
    Q_OBJECT

public:

    explicit udpObject(QObject *parent = nullptr);
    bool bindAll(qint16 port = 0);

    int  writeData(const QByteArray  & data,
                   const QHostAddress& host,
                   const quint16     & port,
                   const qsizetype   & sendPackSize = 512);

signals:

    void readData(QByteArray data);

private:

    QUdpSocket *udpsocket;
    bool isbind = false;
};

#endif // UDPOBJECT_H
