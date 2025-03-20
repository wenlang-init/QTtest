#ifndef QMLINTURN_H
#define QMLINTURN_H

#include <QObject>
#include "propertyHelper.h"
#include "udpslave.h"

class QmlInTurn : public QObject
{
    Q_OBJECT
    AUTO_PROPERTY_ALWAYS(QString,recvText,"");
    //AUTO_PROPERTY_ALWAYS(QStringList,weatherValue,QStringList());
public:
    explicit QmlInTurn(QObject *parent = nullptr);
    Q_INVOKABLE void sendData(QString value);
    Q_INVOKABLE bool connectServer();
    Q_INVOKABLE void setConfig(QString addr,QString port,QString flag);
    Q_INVOKABLE QStringList getCityList();
    Q_INVOKABLE void queryWeather(QString value);
private slots:
    void recvUdpData(QByteArray &data,const QHostAddress &addr,const quint16 &port);
signals:
    void queryWeatherValue(const QStringList &listValue);
private:
    UdpSlave *udpSlave;
    QHostAddress m_addr;
    quint16 m_port;

    QHostAddress m_masterAddr;
    quint16 m_masterPort;
    quint32 m_serverFlag;
};

#endif // QMLINTURN_H
