#ifndef QT_CLIENT_H
#define QT_CLIENT_H

#include <QObject>
#include <QLocalSocket>
#include <QString>

class Client : public QObject {
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    ~Client();

    Q_INVOKABLE void connectToServer();
    Q_INVOKABLE QString sendCommand(const QString &command);

private:
    QLocalSocket *socket;
    bool isConnected;    

    static const QString SOCKET_PATH;
};

#endif // QT_CLIENT_H
