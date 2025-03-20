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
    // curl http://ipinfo.io
    // curl http://ifconfig.me // 171.95.96.46
    // curl http://cip.cc
    /* IP      : 171.95.96.46
    地址    : 中国  四川  广安
    运营商  : 电信
    数据二  : 中国四川广安 | 电信
    数据三  : 中国四川省广安市 | 电信
    URL     : http://www.cip.cc/171.95.96.46
    */
    const char *m_url = "https://openapi.lddgo.net/base/gtool/api/v1/GetIp";
    // {"code":0,"msg":null,"data":{"ip":"171.95.96.46","ipv6":"0000:0000:0000:0000:0000:0000:AB5F:602E"}}
};

#endif // PUBLICIP_H
