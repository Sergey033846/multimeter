#include "qt_client.h"
#include <QDebug>
#include <QLocalSocket>
#include <QTextStream>

// Константа для пути к сокету
const QString Client::SOCKET_PATH = "/tmp/multimeter_socket";  // Путь к сокету

Client::Client(QObject *parent)
    : QObject(parent), socket(new QLocalSocket(this)), isConnected(false) {

    connect(socket, &QLocalSocket::connected, this, &Client::onConnected); // Слот для подключения
    connect(socket, &QLocalSocket::disconnected, this, &Client::onDisconnected); // Слот для отключения
    connect(socket, &QLocalSocket::errorOccurred, this, &Client::onErrorOccurred); // Обработка ошибки
}

Client::~Client() {
    if (socket->isOpen()) {
        qDebug() << "Destructor!";
        socket->close();
    }
}

void Client::connectToServer() {
    if (socket->state() == QLocalSocket::ConnectedState) {
        qDebug() << "Already connected!";
        return;
    }

    qDebug() << "Attempting to connect to server at:" << SOCKET_PATH;
    socket->connectToServer(SOCKET_PATH);

    if (socket->waitForConnected(3000)) {
        qDebug() << "Successfully connected to server!";       
        emit statusChanged("Connected"); 
        isConnected = true;  // Устанавливаем isConnected в true
    } else {
        qDebug() << "Failed to connect! Error:" << socket->errorString();
        emit statusChanged("Disconnected");        
    }

    qDebug() << "Socket state connectToServer: " << socket->state();    
}

void Client::sendCommand(const QString &command) {    
    qDebug() << "Socket state sendCommand: " << socket->state();
    
    if (!isConnected || !socket->isValid()) {  
        // Если сокета нет или он не подключён, пытаемся установить соединение
        qDebug() << "Attempting to reconnect...";
        socket->connectToServer(SOCKET_PATH);
        
        if (socket->waitForConnected(3000)) {
            qDebug() << "Reconnected successfully.";
            emit statusChanged("Connected"); 
            isConnected = true;
        } else {
            qDebug() << "Failed to reconnect! Error:" << socket->errorString();            
            emit statusChanged("Disconnected"); 
            return;
        }
    }

    qDebug() << "Sending command:" << command;
    socket->write(command.toUtf8());
    socket->flush();

    if (socket->waitForReadyRead(3000)) {
        QString response = QString::fromUtf8(socket->readAll());
        qDebug() << "Received response:" << response;
        
        handleResponse(response);

        emit updateChannelText(0, response);
    } else {
        qDebug() << "Failed to receive response!";        
    }
}

void Client::handleResponse(const QString &response) {
    qDebug() << "Handling response:" << response;
    
    QStringList parts = response.split(',');

    if (parts.first() == "ok") {
        if (parts.size() > 1) {
            int channelIndex = parts[1].remove("channel").toInt();
            QString value = parts.size() > 2 ? parts[2] : "";            
            qDebug() << "Channel " << channelIndex << " is Running with value: " << value;
        }
    } else {
        if (parts.size() > 1) {
            int channelIndex = parts[1].remove("channel").toInt();            
            qDebug() << "Channel " << channelIndex << " failed.";
        }
    }
}

void Client::onErrorOccurred(QLocalSocket::LocalSocketError error) {  // Используем правильный тип
    qWarning() << "Socket error:" << error;    
    qDebug() << "Error details:" << socket->errorString();
}

void Client::onConnected() {
    qDebug() << "Socket connected!";
    isConnected = true;  // Отмечаем, что подключение установлено    
}

void Client::onDisconnected() {
    qDebug() << "!!!Socket disconnected!";
    isConnected = false;  // Отмечаем, что подключение разорвано
    
    // Попытка переподключения через некоторое время
    //QTimer::singleShot(3000, this, &Client::connectToServer);  // Попробовать подключиться через 3 секунды
}
