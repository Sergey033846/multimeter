#include "client.h"
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>

/**
 * @brief Конструктор класса Client.
 * 
 * Инициализирует клиента с указанным путем к сокету.
 * @param socket_path Путь к сокету для подключения.
 */
Client::Client(const std::string& socket_path)
    : socket_path(socket_path), client_socket(-1) {}

/**
 * @brief Деструктор класса Client.
 * 
 * Закрывает сокет, если он был открыт.
 */
Client::~Client() {
    if (client_socket != -1) {
        close(client_socket);
        client_log("Client socket closed.");
    }
}

/**
 * @brief Подключение к серверу.
 * 
 * Создает сокет и пытается подключиться к серверу.
 * @return true, если подключение успешно, иначе false.
 */
bool Client::connect_to_server() {
    client_log("Attempting to connect to server...");

    // Создание сокета
    client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("socket");
        client_log("Failed to create socket.");
        return false;
    }

    // Настройка адреса
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path.c_str());

    // Подключение к серверу
    if (connect(client_socket, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect");
        client_log("Failed to connect to server.");
        close(client_socket);
        client_socket = -1;
        return false;
    }

    client_log("Successfully connected to server.");
    return true;
}

/**
 * @brief Отправка команды на сервер.
 * 
 * Отправляет строку команды на сервер.
 * @param command Команда для отправки.
 * @return true, если команда отправлена успешно, иначе false.
 */
bool Client::send_command(const std::string& command) {
    client_log("Sending command: " + command);

    // Отправка команды на сервер
    ssize_t bytes_sent = send(client_socket, command.c_str(), command.size(), 0);
    if (bytes_sent == -1) {
        perror("send");
        client_log("Failed to send command: " + command);
        return false;
    }

    client_log("Command sent successfully: " + command);
    return true;
}

/**
 * @brief Получение ответа от сервера.
 * 
 * Ожидает получения ответа от сервера и возвращает его в виде строки.
 * @return Ответ от сервера в виде строки или пустая строка в случае ошибки.
 */
std::string Client::receive_response() {
    // Получение ответа от сервера
    char buffer[256];
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received == -1) {
        perror("recv");
        client_log("Failed to receive response.");
        return "";
    }

    if (bytes_received == 0) {
        client_log("Server closed the connection.");
        return ""; // Сервер закрыл соединение
    }

    buffer[bytes_received] = '\0';
    client_log("Received response: " + std::string(buffer));
    return std::string(buffer);
}

/**
 * @brief Логгирование сообщений клиента.
 * 
 * Записывает сообщение в стандартный вывод с префиксом [Client Log].
 * @param message Сообщение для вывода.
 */
void Client::client_log(const std::string& message) {
    std::cout << "[Client Log] " << message << std::endl;
}
