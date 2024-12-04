#include "qt_client.h"
#include <QDebug>
#include <QLocalSocket>
#include <QTextStream>

// Константа для пути к сокету
const QString Client::SOCKET_PATH = "/tmp/multimeter_socket";  // Путь к сокету

Client::Client(QObject *parent)
    : QObject(parent), socket(new QLocalSocket(this)), isConnected(false) {
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
        isConnected = true; 
    } else {
        qDebug() << "Failed to connect! Error:" << socket->errorString();        
    }

    qDebug() << "Socket state connectToServer: " << socket->state();    
}

QString Client::sendCommand(const QString &command) {    
    qDebug() << "Socket state sendCommand: " << socket->state();

    QString response = "undefined";

    if (!isConnected || !socket->isValid()) {  
        // Если сокета нет или он не подключён, пытаемся установить соединение
        qDebug() << "Attempting to reconnect...";
        socket->connectToServer(SOCKET_PATH);
        
        if (socket->waitForConnected(1000)) {
            qDebug() << "Reconnected successfully.";            
            isConnected = true;
        } else {
            qDebug() << "Failed to reconnect! Error:" << socket->errorString();                        
            return response;
        }
    }

    qDebug() << "Sending command:" << command;
    socket->write(command.toUtf8());
    socket->flush();

    if (socket->waitForReadyRead(1000)) {
        response = QString::fromUtf8(socket->readAll());
        qDebug() << "Received response:" << response;                       
    } else {
        qDebug() << "Failed to receive response!";        
    }

    return response;
}

