#include "wlayout.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QBoxLayout>
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QPermissions>

wLayout::wLayout(QWidget *parent)
    : QWidget{parent}
{
    // QQmlApplicationEngine *qqmla = new QQmlApplicationEngine();
    // qqmla->loadFromModule("QmlTest", "Main");

    qw = new QQuickWidget(this);
    qw->setResizeMode(QQuickWidget::SizeRootObjectToView);

    // // qw->engine()->addImportPath("qrc:/");
    qw->rootContext()->setContextProperty("wLayout", this);


    // QCameraPermission cameraPermission;
    // qApp->requestPermission(cameraPermission, [=](const QPermission
    // &permission) {
    //     if (permission.status() == Qt::PermissionStatus::Denied)
    //         qDebug() << "Camera permission denied";
    //     else
    //         qw->loadFromModule("QmlTest", "WLayout");
    // });

    // qw->setSource(QUrl("qrc:/src/layout/WLayout.qml"));
    qw->loadFromModule("QmlTest", "WLayout");

    // qw->loadFromModule("QmlTest", "Main");
    qw->showFullScreen();
    connect(qw, &QQuickWidget::sceneGraphError, this,
            [ = ](QQuickWindow::SceneGraphError error, const QString& message) {
        qDebug() << error << message;
    });
    connect(qw, &QQuickWidget::statusChanged, this,
            [ = ](QQuickWidget::Status status) {
        qDebug() << status;
    });

    QHBoxLayout *hboxlayout = new QHBoxLayout(this);
    hboxlayout->setContentsMargins(0, 0, 0, 0);
    hboxlayout->addWidget(qw);
}

bool wLayout::changeFullScreen()
{
    isFullScreen() ? showNormal() : showFullScreen();
    return isFullScreen();
}
