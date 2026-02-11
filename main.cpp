#include <QGuiApplication> // Fixed: Removed the extra "::"
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCoreApplication> // Explicitly include this for exit()
#include <QQuickStyle>
#include "boschserver.h"

int main(int argc, char *argv[])
{
    QQuickStyle::setStyle("Basic");

    QGuiApplication app(argc, argv);

    // 1. Start the Server
    BoschServer server;
    server.startServer(8090);

    QQmlApplicationEngine engine;

    // 2. Inject C++ into QML
    engine.rootContext()->setContextProperty("boschServer", &server);

    // 3. Load Main.qml
    // Fixed: Using standard QStringLiteral instead of _qs to avoid namespace issues
    const QUrl url(QStringLiteral("qrc:/BoschSimulator/Main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
