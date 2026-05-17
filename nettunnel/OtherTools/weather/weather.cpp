#include "weather.h"
#include "citycode.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#define qdebug qDebug().noquote() << __FILE__ << __LINE__ << __FUNCTION__

Weather::Weather(QObject *parent)
    : QObject{parent}
{
    initNetwork();
    initWeatherCityTab();
}

Weather::~Weather()
{
    delete manager;
    delete request;
}

void Weather::initNetwork()
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this,
            [ = ](QNetworkReply *reply) {
        qdebug << reply->size() << reply->url();
        QByteArray data = reply->readAll();

        // qdebug << data;
        WeatherInfo winfo;
        QString errormessage;

        if (true == ParseWeatherInfo(winfo, errormessage, data)) {
            errormessage = "查询完成";
        } else {
            winfo = WeatherInfo();
        }
        emit queryFinish(winfo, errormessage);
        reply->deleteLater();
    });
    request = new QNetworkRequest();

    // request->setUrl(QUrl("http://qt-project.org"));
}

void Weather::queryInfoFromNetwork(const QString& url)
{
    request->setUrl(url);
    QNetworkReply *reply = manager->get(*request);

    // if(reply->isFinished());
    return;

    connect(reply, &QIODevice::readyRead,         this, [ = ]() {
        qdebug << "readyRead";
    });
    connect(reply, &QNetworkReply::errorOccurred, this,
            [ = ](QNetworkReply::NetworkError err) {
        qdebug << err << qt_error_string(err);
    });
    connect(reply, &QNetworkReply::sslErrors,
            this, [ = ](const QList<QSslError>& errors) {
        for (int i = 0; i < errors.size(); i++) {
            qdebug << errors[i].errorString();
        }

        // foreach (auto error, errors) {
        //     qdebug<<error.errorString();
        // }
        // //QList<QSslError>::iterator itstl;
        // QList<QSslError>::const_iterator itstlconst;
        // for(itstlconst = errors.begin();itstlconst !=
        // errors.end();itstlconst++){
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

bool Weather::ParseWeatherInfo(WeatherInfo     & winfo,
                               QString         & errormessage,
                               const QByteArray& data)
{
    QJsonParseError jsonError;
    QJsonDocument   document;

    document = QJsonDocument::fromJson(data, &jsonError);

    if (document.isNull() || (jsonError.error != QJsonParseError::NoError)) {
        errormessage = jsonError.errorString();
        qdebug << errormessage;
        return false;
    }

    if (!document.isObject()) {
        errormessage = "解析错误";
        qdebug << errormessage;
        return false;
    }
    QJsonObject object = document.object();

    if (object.contains("message")) {
        QJsonValue value = object.value("message");
        winfo.message = value.toString();
    }

    if (object.contains("status")) {
        QJsonValue value = object.value("status");
        qdebug << value;

        if (value.toInt() != 200) {
            qdebug << "查询返回错误";
            errormessage = "查询返回错误";
            return false;
        }

        // return false;
    }

    if (object.contains("time")) {
        QJsonValue value = object.value("time");
        winfo.time = value.toString();
    }

    if (object.contains("cityInfo")) {
        QJsonValue value = object.value("cityInfo");

        if (!value.isObject()) {
            errormessage = "解析错误";
            qdebug << errormessage;
            return false;
        }
        QJsonObject obj = value.toObject();

        if (obj.contains("city")) {
            QJsonValue value = obj.value("city");
            winfo.city = value.toString();
        }

        if (obj.contains("parent")) {
            QJsonValue value = obj.value("parent");
            winfo.parentCity = value.toString();
        }

        if (obj.contains("citykey")) {
            QJsonValue value = obj.value("citykey");
            winfo.cityKey = value.toString();
        }

        if (obj.contains("updateTime")) {
            QJsonValue value = obj.value("updateTime");
            winfo.updateTime = value.toString();
        }
    } else {
        errormessage = "查询返回错误";
        qdebug << "查询错误";
        return false;
    }

    if (object.contains("data")) {
        QJsonValue value = object.value("data");

        if (!value.isObject()) {
            errormessage = "解析错误";
            qdebug << errormessage;
            return false;
        }
        QJsonObject obj = value.toObject();

        if (obj.contains("shidu")) {
            QJsonValue value = obj.value("shidu");
            winfo.shidu = value.toString();
        }

        if (obj.contains("pm25")) {
            QJsonValue value = obj.value("pm25");
            winfo.pm25 = QString::number(value.toInt());
        }

        if (obj.contains("pm10")) {
            QJsonValue value = obj.value("pm10");
            winfo.pm10 = QString::number(value.toInt());
        }

        if (obj.contains("quality")) {
            QJsonValue value = obj.value("quality");
            winfo.quality = value.toString();
        }

        if (obj.contains("wendu")) {
            QJsonValue value = obj.value("wendu");
            winfo.wendu = value.toString();
        }

        if (obj.contains("ganmao")) {
            QJsonValue value = obj.value("ganmao");
            winfo.ganmao = value.toString();
        }

        if (obj.contains("forecast")) {
            QJsonValue value = obj.value("forecast");

            if (false == value.isArray()) {
                errormessage = "解析错误";
                qdebug << errormessage;
                return false;
            }

            QJsonArray array = value.toArray();

            for (int i = 0; i < array.size(); i++) {
                QJsonValue value = array.at(i);

                if (false == value.isObject()) {
                    errormessage = "解析错误";
                    qdebug << errormessage;
                    return false;
                }
                QJsonObject obj = value.toObject();
                WeatherData wdata;
                wdata.date = obj.value("date").toString();
                wdata.high = obj.value("high").toString();
                wdata.low = obj.value("low").toString();
                wdata.ymd = obj.value("ymd").toString();
                wdata.week = obj.value("week").toString();
                wdata.sunrise = obj.value("sunrise").toString();
                wdata.sunset = obj.value("sunset").toString();
                wdata.aqi = QString::number(obj.value("aqi").toInt());
                wdata.fx = obj.value("fx").toString();
                wdata.fl = obj.value("fl").toString();
                wdata.type = obj.value("type").toString();
                wdata.notice = obj.value("notice").toString();

                winfo.data.append(wdata);
            }
        }

        if (obj.contains("yesterday")) {
            QJsonValue value = obj.value("yesterday");

            if (false == value.isObject()) {
                errormessage = "解析错误";
                qdebug << errormessage;
                return false;
            }

            QJsonObject obj = value.toObject();

            winfo.yesterdaydata.date = obj.value("date").toString();
            winfo.yesterdaydata.high = obj.value("high").toString();
            winfo.yesterdaydata.low = obj.value("low").toString();
            winfo.yesterdaydata.ymd = obj.value("ymd").toString();
            winfo.yesterdaydata.week = obj.value("week").toString();
            winfo.yesterdaydata.sunrise = obj.value("sunrise").toString();
            winfo.yesterdaydata.sunset = obj.value("sunset").toString();
            winfo.yesterdaydata.aqi = QString::number(obj.value("aqi").toInt());
            winfo.yesterdaydata.fx = obj.value("fx").toString();
            winfo.yesterdaydata.fl = obj.value("fl").toString();
            winfo.yesterdaydata.type = obj.value("type").toString();
            winfo.yesterdaydata.notice = obj.value("notice").toString();
        }
    } else {
        errormessage = "查询返回错误";
        qdebug << "查询错误";
        return false;
    }

    return true;
}

bool Weather::queryWeather(const QString& cityWeatherCode)
{
    QString weatherurl(m_weatherurl);

    weatherurl.append(cityWeatherCode);
    queryInfoFromNetwork(weatherurl);

    return true;
}

bool Weather::queryWeather(const QString& locationName,
                           const QString& subCityName,
                           const QString& capitalCityName)
{
    // getCityCodeFromIndex(0);
    const char *code = getCityCode(locationName.toUtf8().data(),
                                   subCityName.toUtf8().data(),
                                   capitalCityName.toUtf8().data());

    if (code == NULL) {
        qdebug << QString("未发现城市：%1-%2-%3").arg(capitalCityName,
                                                subCityName,
                                                locationName);
        return false;
    }
    QString strcode = QString::fromUtf8(code);

    return queryWeather(strcode);
}

bool Weather::queryLocation()
{
    return false;
}

void Weather::initWeatherCityTab()
{
    for (int i = 0; i < getCityCount(); i++) {
        _allWeatherCityTab *pcapitalCitynode = nullptr;

        for (int j = 0; j < m_AllWeatherCity.size(); j++) {
            if (m_AllWeatherCity[j].capitalCityName ==
                cityWeatherCodeTab[i].capitalCityName) {
                pcapitalCitynode = &m_AllWeatherCity[j];
                break;
            }
        }

        if (!pcapitalCitynode) {
            _allWeatherCityTab node;
            node.capitalCityName = cityWeatherCodeTab[i].capitalCityName;
            m_AllWeatherCity.append(node);
            pcapitalCitynode = &m_AllWeatherCity.last();
        }

        _allWeathersubCityTab *psubCitynode = nullptr;

        for (int j = 0; j < pcapitalCitynode->subCityTab.size(); j++) {
            if (pcapitalCitynode->subCityTab[j].subCityName ==
                cityWeatherCodeTab[i].subCityName) {
                psubCitynode = &pcapitalCitynode->subCityTab[j];
                break;
            }
        }

        if (!psubCitynode) {
            _allWeathersubCityTab node;
            node.subCityName = cityWeatherCodeTab[i].subCityName;
            pcapitalCitynode->subCityTab.append(node);
            psubCitynode = &pcapitalCitynode->subCityTab.last();
        }
        _allWeatherlocationTab locationnode;
        locationnode.cityWeatherCode = cityWeatherCodeTab[i].cityWeatherCode;
        locationnode.locationName = cityWeatherCodeTab[i].locationName;
        psubCitynode->locationTab.append(locationnode);
    }
}

QStringList Weather::getAllCityName()
{
    QStringList strlist;

    for (int i = 0; i < m_AllWeatherCity.size(); i++) {
        strlist.append(m_AllWeatherCity[i].capitalCityName);
    }
    return strlist;
}

QStringList Weather::getAllsubCityName(const QString& capitalCityName)
{
    QStringList strlist;

    for (int i = 0; i < m_AllWeatherCity.size(); i++) {
        if (QString(m_AllWeatherCity[i].capitalCityName) == capitalCityName) {
            for (int j = 0; j < m_AllWeatherCity[i].subCityTab.size(); j++) {
                strlist.append(m_AllWeatherCity[i].subCityTab[j].subCityName);
            }
            break;
        }
    }
    return strlist;
}

QStringList Weather::getAlllocationName(const QString& capitalCityName,
                                        const QString& subCityName)
{
    QStringList strlist;

    for (int i = 0; i < m_AllWeatherCity.size(); i++) {
        if (QString(m_AllWeatherCity[i].capitalCityName) == capitalCityName) {
            for (int j = 0; j < m_AllWeatherCity[i].subCityTab.size(); j++) {
                if (QString(m_AllWeatherCity[i].subCityTab[j].subCityName) ==
                    subCityName) {
                    for (int k = 0;
                         k < m_AllWeatherCity[i].subCityTab[j].locationTab.size();
                         k++) {
                        strlist.append(
                            m_AllWeatherCity[i].subCityTab[j].locationTab[k].locationName);
                    }
                    break;
                }
            }
            break;
        }
    }
    return strlist;
}

QString Weather::getcityWeatherCode(const QString& locationName,
                                    const QString& subCityName,
                                    const QString& capitalCityName)
{
    const char *code = getCityCode(locationName.toUtf8().data(),
                                   subCityName.toUtf8().data(),
                                   capitalCityName.toUtf8().data());

    if (code == NULL) {
        qdebug << QString("未发现城市：%1-%2-%3").arg(capitalCityName,
                                                subCityName,
                                                locationName);
        return QString("");
    }

    return QString::fromUtf8(code);
}
