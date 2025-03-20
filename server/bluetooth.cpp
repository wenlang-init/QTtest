
#include "bluetooth.h"
#include <QTimer>

#include <QDebug>
#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__

Bluetooth::Bluetooth(QObject *parent)
    : QObject{parent}
{
    //创建设备发现对象
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    discoveryAgent->setLowEnergyDiscoveryTimeout(10000);

    localDevice = new QBluetoothLocalDevice(this);
    if(localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff){
        // 打开蓝牙
        localDevice->powerOn();
    } else {
        // 关闭蓝牙
        //localDevice->setHostMode(QBluetoothLocalDevice::HostPoweredOff);
    }
    qdebug<<localDevice->name()<<localDevice->address();
    localDevice->deleteLater();

    //搜索到设备后进入
    connect(discoveryAgent,&QBluetoothDeviceDiscoveryAgent::deviceDiscovered,this,[=](QBluetoothDeviceInfo devInfo){
        QStringList info = getinfo(devInfo);
        emit searchdeviceinfo(info,devInfo);
        qdebug<<devInfo.serviceUuids();
        //if(devInfo.address().isNull()){devInfo.deviceUuid()};//mac / ios
        //qdebug << "搜索到设备" << devInfo.address() <<devInfo.name() << devInfo.rssi() << devInfo.deviceUuid() << devInfo.coreConfigurations();
    });

    // 设备信息更新
    connect(discoveryAgent,&QBluetoothDeviceDiscoveryAgent::deviceUpdated,this,[=](const QBluetoothDeviceInfo &devInfo, QBluetoothDeviceInfo::Fields updatedFields){
//        if(updatedFields & QBluetoothDeviceInfo::Field::None){
//        } else if(updatedFields & QBluetoothDeviceInfo::Field::RSSI){
//        } else if(updatedFields & QBluetoothDeviceInfo::Field::ManufacturerData){
//        } else if(updatedFields & QBluetoothDeviceInfo::Field::ServiceData){
//        } else {
//        }
        //qdebug<<updatedFields<<devInfo.manufacturerData()<<devInfo.serviceData();
        Q_UNUSED(updatedFields);
        QStringList info = getinfo(devInfo);
        emit searchdeviceinfo(info,devInfo);
        //qdebug << "更新设备" << devInfo.address() << devInfo.name() << devInfo.rssi() << devInfo.deviceUuid() << devInfo.coreConfigurations()<<updatedFields;
    });

    //搜索设备发生错误
    connect(discoveryAgent,&QBluetoothDeviceDiscoveryAgent::errorOccurred,[=](QBluetoothDeviceDiscoveryAgent::Error error){
        qdebug << error << qt_error_string(error);
    });

    //搜索设备完成
    connect(discoveryAgent,&QBluetoothDeviceDiscoveryAgent::finished,[=](){
        qdebug << "搜索完成";
    });

    //搜索设备 stop
    connect(discoveryAgent,&QBluetoothDeviceDiscoveryAgent::canceled,[=](){
        qdebug << "停止搜索";
    });

    //QLowEnergyController;
}

Bluetooth::~Bluetooth()
{
    discoveryAgent->deleteLater();
}

void Bluetooth::startsearch()
{
    //开始搜索设备
    discoveryAgent->start();
}

void Bluetooth::stopsearch()
{
    discoveryAgent->stop();
}

bool Bluetooth::getservice(QBluetoothDeviceInfo &devInfo)
{
#if 1
    QBluetoothSocket *btSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol,this);
    connect(btSocket,&QBluetoothSocket::connected,this,[=](){
        QBluetoothSocket *btSocket = qobject_cast<QBluetoothSocket *>(sender());
        qdebug<<"连接成功"<<btSocket->peerAddress()<<btSocket->peerName()<<btSocket->state();
    });
    connect(btSocket,&QBluetoothSocket::disconnected,this,[=](){
        QBluetoothSocket *btSocket = qobject_cast<QBluetoothSocket *>(sender());
        qdebug<<"连接断开"<<btSocket->peerAddress()<<btSocket->peerName()<<btSocket->state();
    });
    connect(btSocket,&QBluetoothSocket::errorOccurred,this,[=](QBluetoothSocket::SocketError error){
        QBluetoothSocket *btSocket = qobject_cast<QBluetoothSocket *>(sender());
        qdebug<<"连接错误"<<btSocket->peerAddress()<<btSocket->peerName()<<btSocket->state()<<error;
    });
    connect(btSocket,&QBluetoothSocket::stateChanged,this,[=](QBluetoothSocket::SocketState state){
        QBluetoothSocket *btSocket = qobject_cast<QBluetoothSocket *>(sender());
        qdebug<<"状态改变"<<btSocket->peerAddress()<<btSocket->peerName()<<btSocket->state()<<state;
    });
    for(int i=0;i<devInfo.serviceUuids().size();i++){
        btSocket->connectToService(devInfo.address(),devInfo.serviceUuids()[i],QIODevice::ReadWrite);
    }
#else
    // 低功耗蓝牙
    if((devInfo.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration))
    {
        //创建蓝牙控制器
        QLowEnergyController *lowBtControl = QLowEnergyController::createCentral(devInfo,this);
        //连接设备
        lowBtControl->connectToDevice();
        //连接成功后进入
        connect(lowBtControl,&QLowEnergyController::connected,this,[=](){
            qdebug << "连接成功";
            QLowEnergyController *lowBtControl = qobject_cast<QLowEnergyController *>(sender());
            //搜索服务
            lowBtControl->discoverServices();

        });
        //连接失败后进入
        connect(lowBtControl,&QLowEnergyController::errorOccurred,this,[=](QLowEnergyController::Error newError){
            qdebug << "连接失败" << qt_error_string(newError);
        });
        //搜索到服务后进入
        connect(lowBtControl,&QLowEnergyController::serviceDiscovered, this,[=](QBluetoothUuid serviceUuid) {
            qdebug<<serviceUuid;
            emit serviceupdate(serviceUuid);
        });
    } else {
        //创建通信的socket 普通蓝牙使用
        QBluetoothSocket *btSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol,this);
        btSocket->connectToService(devInfo.address(),QBluetoothUuid("00001101-0000-1000-8000-00805F9B34FB"),QIODevice::ReadWrite);
    }
#endif
    return true;
}

void Bluetooth::createservice(QBluetoothUuid &serviceUuid)
{
    //创建服务
//    QLowEnergyService *m_service = lowBtControl->createServiceObject(QBluetoothUuid(serviceUuid),this);
//    if(m_service != nullptr){
//        qdebug<<"创建服务成功";
//        connect(m_service,&QLowEnergyService::stateChanged, this, [=]() {

//        });
//    } else {
//        qdebug<<"创建服务失败";
//    }
//    m_service->discoverDetails(); // 启动服务
}

QStringList Bluetooth::getinfo(const QBluetoothDeviceInfo &devInfo)
{
    QStringList info;
    info<<devInfo.name();
    if(devInfo.address().isNull()){
        info<<QString("");
    } else {
        info<<devInfo.address().toString();
    }
    info.append(QString::number(devInfo.rssi()));
    if(devInfo.deviceUuid().isNull()){
        info<<QString("");
    } else {
        info<<devInfo.deviceUuid().toString();
    }
    switch (devInfo.coreConfigurations()) {
    case QBluetoothDeviceInfo::UnknownCoreConfiguration:
        info<<"未知";
        break;
    case QBluetoothDeviceInfo::BaseRateCoreConfiguration:
        info<<"标准";
        break;
    case QBluetoothDeviceInfo::BaseRateAndLowEnergyCoreConfiguration:
        info<<"智能"; // 支持标准和低功耗
        break;
    case QBluetoothDeviceInfo::LowEnergyCoreConfiguration:
        info<<"低功耗";
        break;
    default:
        info<<"未知";
        break;
    }
    return info;
}

