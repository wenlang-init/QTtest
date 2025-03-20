#ifndef UDPSLAVE_H
#define UDPSLAVE_H

#include <QObject>
#include <QUdpSocket>
#include <QDateTime>

struct ServerIPInfo
{
    quint32 serverFlag; // 服务器标志
    quint16 serverPort; // 服务器端口
    QHostAddress addr; // 服务器地址
    ServerIPInfo() {}
};

class UdpSlave : public QObject
{
    Q_OBJECT
// public:
//     static UdpSlave& getInstance(){
//         static UdpSlave us;
//         return us;
//     }
public:
    explicit UdpSlave(QObject *parent = nullptr);
public:
    ~UdpSlave();

    bool open(QHostAddress serverAddr,quint16 serverPort,quint16 localPort = 13573);
    bool close();
    bool isopen();
    void setIsServer(bool flag,quint32 serverFlag);
    void setIsClient(bool flag);
    void printNetInfo();
    QList<ServerIPInfo> getAllServerIPInfo();

protected:
    bool parseServerIP(QByteArray &data);
    void printALLServerIp();
signals:

private:
    struct ServerIpNode
    {
        ServerIPInfo sIPInfo;
        QDateTime datetime;
        ServerIpNode() {
            datetime = QDateTime::currentDateTime();
        }
    };
    bool m_isServer = false;
    bool m_isClient = false;
    quint32 m_serverFlag;
    QUdpSocket *udpsocket;
    QHostAddress m_serverAddr;
    quint16 m_serverPort;
    quint16 m_localport;

    QList<ServerIpNode> m_serverIPNodeList;
};

#endif // UDPSLAVE_H
