
#ifndef UDPOBJ_H
#define UDPOBJ_H


#include <QObject>
#include <QUdpSocket>

class Udpobj : public QObject
{
    Q_OBJECT
public:
    explicit Udpobj(QObject *parent = nullptr);
    ~Udpobj();
    bool open(quint16 port);
    bool close();
    bool isopen();
signals:
    void allinfo(QString &infostr);
private:
    QUdpSocket *udpsocket;
};

#endif // UDPOBJ_H
