#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "udpslave.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void recvUdpData(QByteArray &data,const QHostAddress &addr,const quint16 &port);
    void on_pushButton_clicked();

    void reflushIPList();
    void on_pushButtonsend_clicked();

private:
    Ui::Widget *ui;
    UdpSlave *udpSlave;
    QList<ServerIPInfo> m_serverIPList;
};
#endif // WIDGET_H
