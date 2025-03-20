#include "weatherwidget.h"
#include "ui_weatherwidget.h"
#include <QCompleter>
#include <QFile>
#include <QDebug>
#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__

WeatherWidget::WeatherWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WeatherWidget)
{
    ui->setupUi(this);

    // QFile file(":/qss/QComboBox.qss");
    // if(file.open(QIODevice::ReadOnly)){
    //     ui->comboBox->setStyleSheet(file.readAll());
    //     file.close();
    // }

    QStringList allCity = Weather::getInstance().getAllCity();
    ui->comboBox->addItems(allCity);
    ui->comboBox->setCurrentText("成都");

    // 设置可编辑
    ui->comboBox->setEditable(true);
    // 自动补全实例的构建
    QCompleter *comp = new QCompleter(allCity,ui->comboBox);
    // 设置匹配模式，只要包含就显示
    comp->setFilterMode(Qt::MatchContains);
    // 设置大小写区分，不区大小写
    comp->setCaseSensitivity(Qt::CaseInsensitive);
    // 设置向用户提供补全的方式
    comp->setCompletionMode(QCompleter::PopupCompletion);
    // 装载补全实例
    ui->comboBox->setCompleter(comp);

    connect(&Weather::getInstance(),&Weather::queryFinish,this,[=](WeatherInfo &winfo,QString &errormessage){
        qdebug << errormessage;
        ui->labelinfo->setText(errormessage);
        if(errormessage == "查询完成"){
            isok = true;
            m_winfo = winfo;
            on_pushButtonhome_clicked();
        }
    });
}

WeatherWidget::~WeatherWidget()
{
    delete ui;
}

void WeatherWidget::on_pushButton_clicked()
{
    QString city = ui->comboBox->currentText();
    bool info = Weather::getInstance().queryWeather(city);
    if(!info){
        ui->labelinfo->setText(QString("未发现城市：%1").arg(city));
    }
}

void WeatherWidget::insertText(QString text, QPlainTextEdit* plainTextEdit,int fontSize, QColor fontColor, QColor backColor)
{
    QTextCharFormat fmt;
    //字体色
    fmt.setForeground(QBrush(fontColor));
    //fmt.setUnderlineColor("red");

    //背景色
    fmt.setBackground(QBrush(backColor));
    //字体大小
    fmt.setFontPointSize(fontSize);
    //文本框使用以上设定
    plainTextEdit->mergeCurrentCharFormat(fmt);
    //文本框添加文本
    plainTextEdit->appendPlainText(text);
}


void WeatherWidget::on_pushButtonhome_clicked()
{
    if(false == isok)return;
    currentIndex = -1;
    ui->plainTextEdit->clear();
    QString str;
    str = QString("信息:%1").arg(m_winfo.message);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("时间:%1").arg(m_winfo.time);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("城市:%1 %2").arg(m_winfo.parentCity).arg(m_winfo.city);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("城市Key:%1").arg(m_winfo.cityKey);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("更新时间:%1\n").arg(m_winfo.updateTime);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));

    str = QString("湿度:%1").arg(m_winfo.shidu);
    insertText(str,ui->plainTextEdit,16, QColor("#1f80a1"),QColor("#a0a0a0"));
    str = QString("PM2.5:%1").arg(m_winfo.pm25);
    insertText(str,ui->plainTextEdit,16, QColor("#1f80a1"),QColor("#a0a0a0"));
    str = QString("PM1.0:%1").arg(m_winfo.pm10);
    insertText(str,ui->plainTextEdit,16, QColor("#1f80a1"),QColor("#a0a0a0"));
    str = QString("空气质量:%1").arg(m_winfo.quality);
    insertText(str,ui->plainTextEdit,16, QColor("#1f80a1"),QColor("#a0a0a0"));
    str = QString("温度:%1").arg(m_winfo.wendu);
    insertText(str,ui->plainTextEdit,16, QColor("#1f80a1"),QColor("#a0a0a0"));
    str = QString("感冒提示:%1").arg(m_winfo.ganmao);
    insertText(str,ui->plainTextEdit,16, QColor("#1f80a1"),QColor("#a0a0a0"));
}


void WeatherWidget::on_pushButtonp_clicked()
{
    if(false == isok)return;
    currentIndex--;
    if(currentIndex < 0){
        currentIndex = 0;
    }
    if(currentIndex >= m_winfo.data.size()){
        currentIndex = m_winfo.data.size() - 1;
    }
    ui->plainTextEdit->clear();
    QString str;
    str = QString("日期:%1 %2").arg(m_winfo.data[currentIndex].date).arg(m_winfo.data[currentIndex].week);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("高温:%1").arg(m_winfo.data[currentIndex].high);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("低温:%1").arg(m_winfo.data[currentIndex].low);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("时间:%1").arg(m_winfo.data[currentIndex].ymd);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("日出:%1").arg(m_winfo.data[currentIndex].sunrise);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("日落:%1").arg(m_winfo.data[currentIndex].sunset);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("空气质量:%1").arg(m_winfo.data[currentIndex].aqi);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("风向:%1").arg(m_winfo.data[currentIndex].fx);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("风级:%1").arg(m_winfo.data[currentIndex].fl);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("天气:%1").arg(m_winfo.data[currentIndex].type);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("通知:%1").arg(m_winfo.data[currentIndex].notice);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
}


void WeatherWidget::on_pushButtonn_clicked()
{
    if(false == isok)return;
    currentIndex++;
    if(currentIndex < 0){
        currentIndex = 0;
    }

    if(currentIndex >= m_winfo.data.size()){
        currentIndex = m_winfo.data.size() - 1;
    }
    ui->plainTextEdit->clear();
    QString str;
    str = QString("日期:%1 %2").arg(m_winfo.data[currentIndex].date).arg(m_winfo.data[currentIndex].week);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("高温:%1").arg(m_winfo.data[currentIndex].high);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("低温:%1").arg(m_winfo.data[currentIndex].low);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("时间:%1").arg(m_winfo.data[currentIndex].ymd);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("日出:%1").arg(m_winfo.data[currentIndex].sunrise);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("日落:%1").arg(m_winfo.data[currentIndex].sunset);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("空气质量:%1").arg(m_winfo.data[currentIndex].aqi);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("风向:%1").arg(m_winfo.data[currentIndex].fx);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("风级:%1").arg(m_winfo.data[currentIndex].fl);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("天气:%1").arg(m_winfo.data[currentIndex].type);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("通知:%1").arg(m_winfo.data[currentIndex].notice);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
}


void WeatherWidget::on_pushButtony_clicked()
{
    if(false == isok)return;
    currentIndex = -2;
    ui->plainTextEdit->clear();
    QString str;
    str = QString("日期:%1 %2").arg(m_winfo.yesterdaydata.date).arg(m_winfo.yesterdaydata.week);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("高温:%1").arg(m_winfo.yesterdaydata.high);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("低温:%1").arg(m_winfo.yesterdaydata.low);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("时间:%1").arg(m_winfo.yesterdaydata.ymd);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("日出:%1").arg(m_winfo.yesterdaydata.sunrise);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("日落:%1").arg(m_winfo.yesterdaydata.sunset);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("空气质量:%1").arg(m_winfo.yesterdaydata.aqi);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("风向:%1").arg(m_winfo.yesterdaydata.fx);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("风级:%1").arg(m_winfo.yesterdaydata.fl);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("天气:%1").arg(m_winfo.yesterdaydata.type);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
    str = QString("通知:%1").arg(m_winfo.yesterdaydata.notice);
    insertText(str,ui->plainTextEdit,16, QColor("#1a1b1c"),QColor("#a0a0a0"));
}
