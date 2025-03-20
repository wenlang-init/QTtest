#ifndef PUBLICIP_H
#define PUBLICIP_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

struct IpAddressItem {
    QString ipv4;
    QString ipv6;
};

class PublicIP : public QObject
{
    Q_OBJECT
public:
    static PublicIP& getInstance(){
        static PublicIP pi;
        return pi;
    }
protected:
    explicit PublicIP(QObject *parent = nullptr);
public:
    ~PublicIP();
    bool init();
    bool requestPubliceIP();
protected:
    bool ParseInfo(IpAddressItem &info,const QByteArray &data);
signals:
    void queryFinish(IpAddressItem &data,bool issuccess);
private:
    QNetworkAccessManager *manager;
    QNetworkRequest *request;
    const char *m_url = "https://openapi.lddgo.net/base/gtool/api/v1/GetIp";
};

#endif // PUBLICIP_H
