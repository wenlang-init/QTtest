#include "qmlinturn.h"
#include "other/publicip.h"
#include "other/weather.h"
#include <QDebug>
#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__

QmlInTurn::QmlInTurn(QObject *parent)
    : QObject{parent}
{
    udpSlave = new UdpSlave(this);
    connect(udpSlave,SIGNAL(recvUdpData(QByteArray &,const QHostAddress &,const quint16 &)),this,SLOT(recvUdpData(QByteArray &,const QHostAddress &,const quint16 &)));
    //Weather::getInstance().queryWeather("成都");
    connect(&Weather::getInstance(),&Weather::queryFinish,this,[=](WeatherInfo &winfo,QString &errormessage){
        qdebug << errormessage;
        if(errormessage == "查询完成"){
            QStringList listValue;

            QString str;

            str = QString("日期:%1 %2\n").arg(winfo.yesterdaydata.date).arg(winfo.yesterdaydata.week);
            str += QString("高温:%1\n").arg(winfo.yesterdaydata.high);
            str += QString("低温:%1\n").arg(winfo.yesterdaydata.low);
            str += QString("时间:%1\n").arg(winfo.yesterdaydata.ymd);
            str += QString("日出:%1\n").arg(winfo.yesterdaydata.sunrise);
            str += QString("日落:%1\n").arg(winfo.yesterdaydata.sunset);
            str += QString("空气质量:%1\n").arg(winfo.yesterdaydata.aqi);
            str += QString("风向:%1\n").arg(winfo.yesterdaydata.fx);
            str += QString("风级:%1\n").arg(winfo.yesterdaydata.fl);
            str += QString("天气:%1\n").arg(winfo.yesterdaydata.type);
            str += QString("通知:%1\n").arg(winfo.yesterdaydata.notice);
            listValue.append(str);

            str = QString("信息:%1\n").arg(winfo.message);
            str += QString("时间:%1\n").arg(winfo.time);
            str += QString("城市:%1 %2\n").arg(winfo.parentCity).arg(winfo.city);
            str += QString("城市Key:%1\n").arg(winfo.cityKey);
            str += QString("更新时间:%1\n").arg(winfo.updateTime);

            str += QString("湿度:%1\n").arg(winfo.shidu);
            str += QString("PM2.5:%1\n").arg(winfo.pm25);
            str += QString("PM1.0:%1\n").arg(winfo.pm10);
            str += QString("空气质量:%1\n").arg(winfo.quality);
            str += QString("温度:%1\n").arg(winfo.wendu);
            str += QString("感冒提示:%1\n").arg(winfo.ganmao);
            listValue.append(str);

            for(int i=0;i<winfo.data.size();i++){
                str = QString("日期:%1 %2\n").arg(winfo.data[i].date).arg(winfo.data[i].week);
                str += QString("高温:%1\n").arg(winfo.data[i].high);
                str += QString("低温:%1\n").arg(winfo.data[i].low);
                str += QString("时间:%1\n").arg(winfo.data[i].ymd);
                str += QString("日出:%1\n").arg(winfo.data[i].sunrise);
                str += QString("日落:%1\n").arg(winfo.data[i].sunset);
                str += QString("空气质量:%1\n").arg(winfo.data[i].aqi);
                str += QString("风向:%1\n").arg(winfo.data[i].fx);
                str += QString("风级:%1\n").arg(winfo.data[i].fl);
                str += QString("天气:%1\n").arg(winfo.data[i].type);
                str += QString("通知:%1\n").arg(winfo.data[i].notice);
                listValue.append(str);
            }

            //qdebug << listValue;
            //set_weatherValue(listValue);
            emit queryWeatherValue(listValue);
        }
    });
    //PublicIP::getInstance().requestPubliceIP();
    connect(&PublicIP::getInstance(),&PublicIP::queryFinish,this,[=](IpAddressItem &data,bool issuccess){
        qdebug<<data.ipv4<<data.ipv6<<issuccess;
    });
}

void QmlInTurn::sendData(QString value)
{
    if(udpSlave){
        qdebug<<udpSlave->sendData(value.toUtf8(),m_addr,m_port);
    }
}

bool QmlInTurn::connectServer()
{
    udpSlave->setIsServer(true,m_serverFlag);
    if(true == udpSlave->open(m_masterAddr,m_masterPort)){
        return true;
    } else {
        udpSlave->close();
        return false;
    }
}

void QmlInTurn::setConfig(QString addr, QString port, QString flag)
{
    m_masterAddr.setAddress(addr);
    m_masterPort = port.toUShort();
    m_serverFlag = flag.toInt(nullptr,16);
}

QStringList QmlInTurn::getCityList()
{
    return Weather::getInstance().getAllCity();
}

void QmlInTurn::queryWeather(QString value)
{
    Weather::getInstance().queryWeather(value);
}

void QmlInTurn::recvUdpData(QByteArray &data, const QHostAddress &addr, const quint16 &port)
{
    m_addr = addr;
    m_port = port;
    //qdebug<<QString::fromUtf8(data);
    set_recvText(QString::fromUtf8(data));
}
