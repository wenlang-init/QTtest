
#include "qmlwidget.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QBoxLayout>
#include <QDebug>
#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__

qmlWidget::qmlWidget(QWidget *parent)
    : QWidget{parent}
{
    QQuickWidget *qw = new QQuickWidget(this);
    qw->setResizeMode(QQuickWidget::SizeRootObjectToView);
    //qw->engine()->addImportPath("qrc:/");
    qw->rootContext()->setContextProperty("qmlWidget",this);
    qw->setSource(QUrl("qrc:/qml/main.qml"));
    qdebug<<QUrl("qrc:/qml/main.qml").toLocalFile();
    qw->showFullScreen();
    connect(qw,&QQuickWidget::sceneGraphError,this,[=](QQuickWindow::SceneGraphError error, const QString &message){
        qdebug<<error<<message;
    });
    connect(qw,&QQuickWidget::statusChanged,this,[=](QQuickWidget::Status status){
        qdebug<<status;
    });

    QHBoxLayout *hboxlayout = new QHBoxLayout(this);
    hboxlayout->addWidget(qw);
}

void qmlWidget::closeshow()
{
    emit hidewidget();
}

