#include "listw.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QBoxLayout>
#include <QDebug>
#include <QTimer>
#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__

ListW::ListW(QWidget *parent)
    : QWidget{parent}
{
    listmodel = new ListModel(this);
    QQuickWidget *qw = new QQuickWidget(this);
    qw->setResizeMode(QQuickWidget::SizeRootObjectToView);
    //qw->engine()->addImportPath("qrc:/");
    qw->rootContext()->setContextProperty("ListW",this);
    qw->rootContext()->setContextProperty("listmodel",listmodel);
    qw->setSource(QUrl("qrc:/listw/list.qml"));
    qw->showFullScreen();
    connect(qw,&QQuickWidget::sceneGraphError,this,[=](QQuickWindow::SceneGraphError error, const QString &message){
        qdebug<<error<<message;
    });
    connect(qw,&QQuickWidget::statusChanged,this,[=](QQuickWidget::Status status){
        qdebug<<status;
    });

    QHBoxLayout *hboxlayout = new QHBoxLayout(this);
    hboxlayout->addWidget(qw);

    QTimer *timer = new QTimer(this);
    timer->start(1000);
    static int i=0;
    static bool ib = false;
    static QString s="0";
    connect(timer,&QTimer::timeout,this,[=](){
        s = QString("str:%1").arg(i);
        ListDataInfo data;
        data.set_t1(i);
        data.set_t2(ib);
        data.set_t3(s);
        listmodel->addData(data);

        if(ib){
            listmodel->deleteData(0);
            ListDataInfo data;
            listmodel->getData(0,data);
            data.set_t3(QString("xxxxxxxxxxx %1").arg(data.t1()));
            listmodel->updateData(0,data);
            //listmodel->clearData();
        }

        if(ib)ib=false;else ib=true;
        i++;
        //qdebug << listmodel->currentIndext();
    });
}
