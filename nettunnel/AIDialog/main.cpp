#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "aimessage.h"
#include "printFunction.h"

int main(int argc, char *argv[])
{
    REDIRECT_QTMESSAGE(nullptr);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    AiMessage aiMessage;
    engine.rootContext()->setContextProperty("aiMessage",&aiMessage);
    //engine.addImportPath("qrc:/");
    engine.load(url);

    return app.exec();
}
