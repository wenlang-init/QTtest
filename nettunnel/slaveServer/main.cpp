#include "qmlinturn.h"

// #include <QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "printFunction.h"

int main(int argc, char *argv[])
{
    REDIRECT_QTMESSAGE(nullptr);

    // QApplication a(argc, argv);
    // Widget w;
    // w.show();

    QGuiApplication app(argc, argv);

    // https://www.cnblogs.com/linuxAndMcu/p/11961090.html
    // qmlRegisterType<QmlInTurn>("QmlInTurn",1, 0, "QmlInTurn");
    // import QmlInTurn 1.0

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl& objUrl) {
        if (!obj && (url == objUrl)) QCoreApplication::exit(-1);
    },
        Qt::QueuedConnection);
    QmlInTurn qmlInTurn;
    engine.rootContext()->setContextProperty("qmlInTurn", &qmlInTurn);

    // engine.addImportPath("qrc:/");
    engine.load(url);

    return app.exec();
}
