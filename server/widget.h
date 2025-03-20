
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "tcpserver.h"
#include "udpobj.h"
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QAudioSource>
#include <QIODevice>
#include <QAudioSink>
#include <QMediaDevices>
#include <QVideoWidget>
#include "network.h"
#include "propertyHelper.h"
#include "testwk.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE
#include <QDebug>
class AudioIODevice : public QIODevice
{
    Q_OBJECT
public:
    explicit AudioIODevice(QObject *parent = nullptr): QIODevice(parent){};
    qint64 addData(QByteArray &data) {
        if(m_data.size() > 100000000){
            m_data.clear();
        }
        //qDebug()<<"aaaaaaaaaa "<<data.size();
        m_data.append(data);
        return data.size();
    }
protected:
    qint64 readData(char *data, qint64 maxSize) override {
        Q_UNUSED(data);
        Q_UNUSED(maxSize);
        qint64 size = maxSize;
        if(size > m_data.size()){
            size = m_data.size();
        }
        memcpy(data,m_data.data(),size);
        m_data.remove(0,size);
        qDebug()<<"sss "<<size;
        return size;
    }
    qint64 writeData(const char *data, qint64 maxSize) override {
        QByteArray m_data;
        if(maxSize >0){
            m_data.append(data,maxSize);
            emit sigrxaudiodata(m_data);
        }
        return maxSize;
    }
    qint64 bytesAvailable() const override {
        return m_data.size() + QIODevice::bytesAvailable();
    }
    qint64 size() const override { return m_data.size(); }
signals:
    void sigrxaudiodata(QByteArray &data);
private:
    QByteArray m_data;
};

class Widget : public QWidget

{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Q_PROPERTY(QString qmlQString READ qmlQString WRITE set_qmlQString NOTIFY qmlQStringChanged)
public:
    const QString& qmlQString() const
    {
        return m_qmlQString;
    }
    void set_qmlQString(const QString& value)
    {
        if(m_qmlQString == value)
            return;
        m_qmlQString = value;
        emit qmlQStringChanged(value);
    }
    Q_SIGNAL void qmlQStringChanged(const QString& value);
private:
    QString m_qmlQString = "";

private slots:
    void on_textEdit_tcp_textChanged();

    void on_textEdit_udp_textChanged();

    void on_pushButton_tcp_clicked();

    void on_pushButton_udp_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_comboBox1_currentTextChanged(const QString &arg1);

    void on_comboBoxo1_currentTextChanged(const QString &arg1);

    void on_checkBox_stateChanged(int arg1);

    void on_tabWidget_tabBarDoubleClicked(int index);

    void on_pushButtonmw_clicked();
signals:
    void isrotate90(bool value);
private:
    Ui::Widget *ui;
    TcpServer *tcpserver;
    Udpobj *udp;

    QMediaPlayer *mplayer;
    QAudioOutput *audioOutput;
    quint64 position;
    double volume = 1;

    QAudioSource *audiosource = nullptr;
    AudioIODevice *iodevice = nullptr;

    QAudioSink *audiosink = nullptr;
    AudioIODevice *iodeviceo = nullptr;
    QMediaDevices *mediadevice = nullptr;
    QIODevice *ioout = nullptr;

    QVideoWidget *videowidget;

    TestWk *testwk = nullptr;
};

#endif // WIDGET_H
