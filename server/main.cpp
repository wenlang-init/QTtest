
#include "widget.h"

#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QFile>
#include "nettunnel/nettunnel.h"
#include "nettunnel/udpslave.h"
int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
#if QT_VERSION == QT_VERSION_CHECK(6,5,0)
    qputenv("QT_MEDIA_BACKEND", "windows");
#endif
#endif
    QApplication a(argc, argv);
    Widget w;
    w.show();

    QFile file(":/qss/MacOS.qss");
    if(file.open(QIODevice::ReadOnly)){
        w.setStyleSheet(file.readAll());
        file.close();
    }
    // NetTunnel::getInstance().open(12345);
    // QHostAddress saddr("192.168.31.210");
    // UdpSlave us;
    // us.open(saddr,12345,12346);
    // us.setIsServer(true,0x12345678);
    //us.setIsClient(true);

//#ifndef ANDROID //__aarch64__
//    w.show();
//#else
//    QGraphicsScene *scene = new QGraphicsScene;
//    QGraphicsProxyWidget *sw = scene->addWidget(&w);
//    sw->setRotation(90);
//    QGraphicsView *view = new QGraphicsView(scene);
//    //view->resize(w.size());
//    view->show();
//    QObject::connect(&w,&Widget::isrotate90,nullptr,[=](bool value){
//        if(value){
//            sw->setRotation(90);
//        } else {
//            sw->setRotation(0);
//        }
//    });
//#endif
    return a.exec();
}
