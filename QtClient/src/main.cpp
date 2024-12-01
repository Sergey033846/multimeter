#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "qt_client.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterType<Client>("com.example.client", 1, 0, "Client");

    Client client;

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
