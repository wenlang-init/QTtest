#ifndef NETTUNNEL_H
#define NETTUNNEL_H

#include <QObject>
#include <QUdpSocket>
#include <QDateTime>

class NetTunnel : public QObject
{
    Q_OBJECT
public:
    static NetTunnel& getInstance(){
        static NetTunnel nt;
        return nt;
    }
protected:
    explicit NetTunnel(QObject *parent = nullptr);
public:
    virtual ~NetTunnel();
    bool open(quint16 port);
    bool close();
    bool isopen();
    void printNetInfo();
protected:
    bool parseServerIP(QByteArray &data,const QHostAddress &addr,const quint16 &port);
    bool parseClientSendServerInfo(QByteArray &data,const QHostAddress &addr,const quint16 &port);

    void printEncrypt(const void *data,unsigned int len,unsigned char key);
private:
    struct ServerIPInfo
    {
        quint32 serverFlag; // 服务器标志
        quint16 serverPort; // 服务器端口
        QHostAddress addr; // 服务器地址
        ServerIPInfo() {}
    };
    struct ServerIpNode
    {
        ServerIPInfo sIPInfo;
        QDateTime datetime;
        ServerIpNode() {
            datetime = QDateTime::currentDateTime();
        }
    };

    unsigned char enKey = 0xC2;

    QUdpSocket *udpsocket;
    quint16 m_port;
    QList<ServerIpNode> m_serverIPNodeList;
};

#endif // NETTUNNEL_H
