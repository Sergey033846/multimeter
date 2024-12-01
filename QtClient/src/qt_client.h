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
    Q_INVOKABLE void sendCommand(const QString &command);

signals:
    void statusChanged(const QString &status);
    void commandResponse(const QString &response);    
    void updateChannelText(int channelIndex, const QString &status);


private:
    void handleResponse(const QString &response);
    void onErrorOccurred(QLocalSocket::LocalSocketError error);  // Используем правильный тип
    void onConnected();   // Новый слот для подключения
    void onDisconnected(); // Добавляем слот для обработки отключения

    QLocalSocket *socket;
    bool isConnected;     // Флаг для проверки подключения

    static const QString SOCKET_PATH;
};

#endif // QT_CLIENT_H
