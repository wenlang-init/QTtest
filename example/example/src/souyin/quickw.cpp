#include "quickw.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QHBoxLayout>
#include <QDebug>
#include <QMetaObject>
#include <QQuickItem>
#include <QVariant>
#include <QQuickStyle>
#define qdebug qDebug().noquote()

quickW::quickW(QWidget *parent)
    : QWidget{parent}
{
    QQuickStyle::setStyle("Basic");

    quickw = new QQuickWidget(this);

    quickw->setResizeMode(QQuickWidget::SizeRootObjectToView);

    // quickw->engine()->addImportPath("qrc:/qml/");");
    quickw->rootContext()->setContextProperty("quickw", this);
    quickw->setSource(QUrl("qrc:/src/souyin/edittxt.qml"));
    quickw->setClearColor(QColor(Qt::transparent));
    quickw->showFullScreen();
    connect(quickw, &QQuickWidget::sceneGraphError, this,
            [ = ](QQuickWindow::SceneGraphError error, const QString& message) {
        qdebug << error << message;
    });
    connect(quickw, &QQuickWidget::statusChanged, this,
            [ = ](QQuickWidget::Status status) {
        qdebug << status;
    });

    QHBoxLayout *hboxlayout = new QHBoxLayout(this);
    hboxlayout->addWidget(quickw);

    // QVariant retVal;
    // QMetaObject::invokeMethod(quickw->rootObject(), /* Qml实例 */
    //                           "execute",         /* 函数名字 */
    //                           Qt::DirectConnection, /* 连接方式 */
    //                           Q_RETURN_ARG(QVariant, retVal), /* 标记返回值 */
    //                           Q_ARG(QVariant, "Hello"), /* 输入参数1 */
    //                           Q_ARG(QVariant, "world"));/* 输入参数2 */
}

void quickW::setText(const QString& text)
{
    QMetaObject::invokeMethod(quickw->rootObject(),
                              "setText",
                              Q_ARG(QVariant, text));
}

QString quickW::getText()
{
    QVariant retVal;

    QMetaObject::invokeMethod(quickw->rootObject(),
                              "getText",
                              Q_RETURN_ARG(QVariant, retVal));

    return retVal.toString();
}
