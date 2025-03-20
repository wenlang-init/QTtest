
#ifndef TCPSERVER_H
#define TCPSERVER_H


#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    ~TcpServer();
    bool isopen();
    bool openserver(quint16 port);
    bool closeserver();
signals:
    void allinfo(QString &infostr);
private slots:
    void SlotnewConnection();
signals:
private:
    QTcpServer *server;
    quint16 m_port;
};

#endif // TCPSERVER_H
