#include "aimessage.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#define qdebug qDebug().noquote() //<<__FILE__<<__LINE__<<__FUNCTION__

AiMessage::AiMessage(QObject *parent)
    : QObject{parent}
{
    initNetwork();
}

AiMessage::~AiMessage()
{
    delete manager;
    delete request;
}

void AiMessage::sendData(bool isR1,QString toke,QString value)
{
    qdebug<<toke<<value;
    queryInfoFromNetwork(isR1,value,toke);
    //emit recvMessage(value);
    //QString info("stop");
    //emit stopRecv(info);
}

void AiMessage::queryBalance(QString toke)
{
    qdebug<<toke;
    queryBalanceFromNetwork(toke);
}

void AiMessage::initNetwork()
{
    manager = new QNetworkAccessManager(this);
    // connect(manager, &QNetworkAccessManager::finished,this, [=](QNetworkReply *reply){
    //     qdebug << reply->size() << reply->url();
    //     // QString data = reply->readAll();
    //     // qdebug<<data;
    //     reply->deleteLater();
    //     emit stopRecv();
    // });
    request = new QNetworkRequest();
    //request->setUrl(QUrl("http://qt-project.org"));
}

void AiMessage::queryInfoFromNetwork(bool isR1,QString &text,QString token)
{
    request->setUrl(QUrl("https://api.deepseek.com/chat/completions"));
    request->setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request->setRawHeader("Accept","application/json");
    request->setRawHeader("Authorization",QString("Bearer %1").arg(token).toUtf8());

    QJsonArray message;
    QJsonObject sysmesage;
    sysmesage.insert("role","system");
    sysmesage["content"] = "You are a helpful assistant";
    QJsonObject usermesage;
    usermesage.insert("role","user");
    usermesage["content"] = text;
    message.append(sysmesage);
    message.append(usermesage);

    QJsonObject reqbody;
    reqbody["messages"] = message;
    if(isR1){
        reqbody["model"] = "deepseek-reasoner";
    } else {
        reqbody["model"] = "deepseek-chat";
    }
    reqbody["max_tokens"] = 2048;
    reqbody["stream"] = true;
    reqbody["temperature"] = 1; // 代码生成/数学解题0.0;数据抽取/分析1.0;通用对话	1.3;翻译	1.3;创意类写作/诗歌创作	1.5

    QNetworkReply *reply = manager->post(*request,QJsonDocument(reqbody).toJson());
    connect(reply, &QIODevice::readyRead, this, [=](){
        QByteArray data = reply->readAll();
        QString sdata = data;
        //qdebug<<sdata;
        QStringList strlist = sdata.split("\n\n",Qt::SkipEmptyParts);
        for(int i=0;i<strlist.size();i++){
            if(strlist[i].startsWith("data: ")){
                // 数据
                QString strdata = strlist[i].remove(0,6);
                if(strdata == "[DONE]"){
                    //emit recvMessage(strdata);
                } else {
                    QJsonParseError err;
                    QJsonDocument doc = QJsonDocument::fromJson(strdata.toUtf8(),&err);
                    if(err.error == QJsonParseError::NoError){
                        QString model = doc.object()["model"].toString();
                        if(model == "deepseek-reasoner"){
                            // R1
                            QString reasoning_content = doc.object()["choices"].toArray().first()
                                               .toObject()["delta"].toObject()["reasoning_content"].toString();;
                            if(!reasoning_content.isEmpty()){
                                emit recvMessageRole(reasoning_content);
                            }
                        }
                        QString content = doc.object()["choices"].toArray().first()
                                              .toObject()["delta"].toObject()["content"].toString();
                        if(!content.isEmpty()){
                            emit recvMessage(content);
                        }
                    } else {
                        //qDebug()<<strdata;
                        qdebug<<err.errorString();
                    }
                }
            } else {
                // 错误信息
                QJsonParseError err;
                QJsonDocument doc = QJsonDocument::fromJson(strlist[i].toUtf8(),&err);
                if(err.error == QJsonParseError::NoError){
                    QJsonObject obj = doc.object()["error"].toObject();
                    QString message = obj["message"].toString();
                    QString type = obj["type"].toString();
                    QString param = obj["param"].toString();
                    QString code = obj["code"].toString();
                    QString error = message+";"+ type+";"+ param+";"+ code;
                    //emit stopRecv(error);
                    emit recvMessage(error);
                } else {
                    //qDebug()<<strlist[i];
                    qdebug<<err.errorString();
                }
            }
        }
    });
    connect(reply,&QNetworkReply::finished,this,[=](){
        //qdebug;
        reply->deleteLater();
        emit stopRecv();
    });
    connect(reply, &QNetworkReply::errorOccurred,this, [=](QNetworkReply::NetworkError err){
        qdebug << err << qt_error_string(err);
        reply->deleteLater();
        emit stopRecv();
    });
    connect(reply, &QNetworkReply::sslErrors,
            this, [=](const QList<QSslError> &errors){
                for(int i=0;i<errors.size();i++){
                    qdebug<<errors[i].errorString();
                }
                reply->deleteLater();
                emit stopRecv();
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
}

void AiMessage::queryBalanceFromNetwork(QString token)
{
    request->setUrl(QUrl("https://api.deepseek.com/user/balance"));
    //request->setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request->setRawHeader("Accept","application/json");
    request->setRawHeader("Authorization",QString("Bearer %1").arg(token).toUtf8());

    //QNetworkReply *reply = manager->post(*request,"\0");
    QNetworkReply *reply = manager->get(*request);
    connect(reply, &QIODevice::readyRead, this, [=](){
        QByteArray data = reply->readAll();
        QString sdata = data;
        //qdebug<<data<<sdata;

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(sdata.toUtf8(),&err);
        if(err.error == QJsonParseError::NoError){
            if(doc.object().contains("error")){
                QJsonObject obj = doc.object()["error"].toObject();
                QString message = obj["message"].toString();
                QString type = obj["type"].toString();
                QString param = obj["param"].toString();
                QString code = obj["code"].toString();
                QString error = message+";"+ type+";"+ param+";"+ code;
                //emit stopRecv(error);
                emit recvMessage(error);
            } else if(doc.object().contains("is_available")){
                QString message;
                bool is_available = doc.object()["is_available"].toBool(); // 当前账户是否有余额可供 API 调用
                //message += QString("is_available:%1\n").arg(is_available);
                message += QString("is_available:") + (is_available ? "true":"false") + "\n";
                if(doc.object().contains("balance_infos")){
                    QJsonArray arry = doc.object()["balance_infos"].toArray();
                    for(int i=0;i<arry.size();i++){
                        QString currency = arry[i].toObject()["currency"].toString(); // 货币，人民币或美元
                        QString total_balance = arry[i].toObject()["total_balance"].toString(); // 总的可用余额，包括赠金和充值余额
                        QString granted_balance = arry[i].toObject()["granted_balance"].toString(); // 未过期的赠金余额
                        QString topped_up_balance = arry[i].toObject()["topped_up_balance"].toString(); // 充值余额

                        message += "{\n";
                        message += QString("\tcurrency:%1\n").arg(currency);
                        message += QString("\ttotal_balance:%1\n").arg(total_balance);
                        message += QString("\tgranted_balance:%1\n").arg(granted_balance);
                        message += QString("\ttopped_up_balance:%1\n").arg(topped_up_balance);
                        message += "}\n";
                    }
                }
                emit recvMessage(message);
            }

        } else {
            qDebug()<<sdata;
            qdebug<<err.errorString();
        }
    });
    connect(reply,&QNetworkReply::finished,this,[=](){
        //qdebug;
        reply->deleteLater();
        emit stopRecv();
    });
    connect(reply, &QNetworkReply::errorOccurred,this, [=](QNetworkReply::NetworkError err){
        qdebug << err << qt_error_string(err);
        reply->deleteLater();
        emit stopRecv();
    });
    connect(reply, &QNetworkReply::sslErrors,
            this, [=](const QList<QSslError> &errors){
                for(int i=0;i<errors.size();i++){
                    qdebug<<errors[i].errorString();
                }
                reply->deleteLater();
                emit stopRecv();
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
}
