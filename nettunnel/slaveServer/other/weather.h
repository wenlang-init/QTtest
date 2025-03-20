#ifndef WEATHER_H
#define WEATHER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

struct WeatherData
{
    QString date; // 日期
    QString high; // 高温
    QString low; // 低温
    QString ymd; // 时间
    QString week; // 星期
    QString sunrise; // 日出
    QString sunset; // 日落
    QString aqi; // 空气质量
    QString fx; // 风向
    QString fl; // 风级
    QString type; // 天气
    QString notice; // 通知
    WeatherData() {}
};

struct WeatherInfo
{
    QString message; // 信息
    QString time; // 当前时间

    QString city; // 城市
    QString parentCity; // 父城市
    QString cityKey; // 城市code
    QString updateTime; // 更新时间

    QList<WeatherData> data; // 预报数据
    WeatherData yesterdaydata; // 昨日数据

    QString shidu; // 湿度
    QString pm25; // pm2.5
    QString pm10; // pm1.0
    QString quality; // 空气质量
    QString wendu; // 温度
    QString ganmao; // 建议

    WeatherInfo() {}
};

class Weather : public QObject
{
    Q_OBJECT
public:
    static Weather &getInstance(){
        static Weather weather;
        return weather;
    }
    ~Weather();
    bool queryWeather(const QString &city);
    bool queryLocation();
    QStringList getAllCity();
protected:
    explicit Weather(QObject *parent = nullptr);
    void initNetwork();
    void queryInfoFromNetwork(const QString &url);
    bool ParseWeatherInfo(WeatherInfo &winfo,QString &errormessage,const QByteArray &data);
signals:
    void queryFinish(WeatherInfo &winfo,QString &errormessage);
private:
    QNetworkAccessManager *manager;
    QNetworkRequest *request;
    const char *m_weatherurl = "http://t.weather.itboy.net/api/weather/city/";
    const char *m_cityurl = "http://pv.sohu.com/cityjson?ie=utf-8/";
    //const char *m_cityurl = "https://api.map.baidu.com/location/ip?ak=您的AK&ip=您的IP&coor=bd09ll";
};

#endif // WEATHER_H
