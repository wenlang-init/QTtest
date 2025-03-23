#ifndef AIMESSAGE_H
#define AIMESSAGE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class AiMessage : public QObject
{
    Q_OBJECT
public:
    explicit AiMessage(QObject *parent = nullptr);
    ~AiMessage();
    Q_INVOKABLE void sendData(bool isR1,QString toke,QString value);
    Q_INVOKABLE void queryBalance(QString toke);
signals:
    void recvMessage(QString message);
    void recvMessageRole(QString message);
    void stopRecv();
protected:
    void initNetwork();
    void queryInfoFromNetwork(bool isR1,QString &text,QString tokrn);
    void queryBalanceFromNetwork(QString tokrn); // 查询余额
    //bool parseInfo();
private:
    QNetworkAccessManager *manager;
    QNetworkRequest *request;
};

#endif // AIMESSAGE_H
