#include "publicip.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__

PublicIP::PublicIP(QObject *parent)
    : QObject{parent}
{
    init();
}

PublicIP::~PublicIP()
{
    delete manager;
    delete request;
}

bool PublicIP::init()
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished,this, [=](QNetworkReply *reply){
        qdebug << reply->size() << reply->url();
        QByteArray data = reply->readAll();
        //qdebug << data;
        IpAddressItem info;
        if(true == ParseInfo(info,data)){
            emit queryFinish(info,true);
        } else {
            emit queryFinish(info,false);
        }
        //qdebug<<info.ipv4<<info.ipv6;
        reply->deleteLater();
    });
    request = new QNetworkRequest();
    //request->setUrl(QUrl("http://qt-project.org"));
    return true;
}

bool PublicIP::requestPubliceIP()
{
    request->setUrl(QUrl(m_url));
    QNetworkReply *reply = manager->get(*request);
    //if(reply->isFinished());
    return true;
    connect(reply, &QIODevice::readyRead, this, [=](){
        qdebug << "readyRead";
    });
    connect(reply, &QNetworkReply::errorOccurred,this, [=](QNetworkReply::NetworkError err){
        qdebug << err << qt_error_string(err);
    });
    connect(reply, &QNetworkReply::sslErrors,
            this, [=](const QList<QSslError> &errors){
                for(int i=0;i<errors.size();i++){
                    qdebug<<errors[i].errorString();
                }
                // foreach (auto error, errors) {
                //     qdebug<<error.errorString();
                // }
                // //QList<QSslError>::iterator itstl;
                // QList<QSslError>::const_iterator itstlconst;
                // for(itstlconst = errors.begin();itstlconst != errors.end();itstlconst++){
                //     qdebug<<itstlconst->errorString();
                // }
                // // 读写
                // //QMutableListIterator<QSslError> itjava(errors);
                // QListIterator<QSslError> itjavaconst(errors);
                // while (itjavaconst.hasNext()) {
                //     qdebug << itjavaconst.next().errorString();
                // }
            });
    return true;
}

bool PublicIP::ParseInfo(IpAddressItem &info, const QByteArray &data)
{
    QJsonParseError jsonError;
    QJsonDocument document;

    document = QJsonDocument::fromJson(data,&jsonError);

    if(document.isNull() || jsonError.error != QJsonParseError::NoError){
        qdebug << jsonError.errorString();
        return false;
    }

    if(!document.isObject()){
        qdebug << "解析错误";
        return false;
    }
    QJsonObject object = document.object();
    if(object.contains("data")){
        QJsonValue value = object.value("data");
        if(!value.isObject()){
            qdebug << "解析错误";
            return false;
        }
        QJsonObject obj = value.toObject();
        if(!obj.contains("ip") || !obj.contains("ipv6")){
            qdebug << "解析错误";
            return false;
        }
        value = obj.value("ip");
        info.ipv4 = value.toString();
        value = obj.value("ipv6");
        info.ipv6 = value.toString();

        return true;
    }
    qdebug << "解析错误";
    return false;
}
