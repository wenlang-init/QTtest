
#ifndef BLUETOOTH_H
#define BLUETOOTH_H


#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceInfo>
#include <QBluetoothAddress>
// 连接到运行蓝牙服务器的设备
#include <QBluetoothSocket>
// 连接到运行蓝牙服务器的设备，低功耗蓝牙访问
#include <QLowEnergyController>

class Bluetooth : public QObject
{
    Q_OBJECT
public:
    explicit Bluetooth(QObject *parent = nullptr);
    ~Bluetooth();

public slots:
    void startsearch();
    void stopsearch();
    bool getservice(QBluetoothDeviceInfo &devInfo);
    void createservice(QBluetoothUuid &serviceUuid);
private:
    QStringList getinfo(const QBluetoothDeviceInfo &devInfo);
signals:
    void searchdeviceinfo(QStringList &info,const QBluetoothDeviceInfo &devInfo);
    void serviceupdate(QBluetoothUuid &serviceUuid);
private:
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothLocalDevice *localDevice;
};

#endif // BLUETOOTH_H
