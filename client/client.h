#pragma once

#include <string>

/**
 * @class Client
 * @brief Класс клиента для работы с сервером через UNIX-сокет.
 * 
 * Этот класс позволяет подключаться к серверу, отправлять команды и получать ответы.
 */
class Client {
public:
    /**
     * @brief Конструктор класса Client.
     * 
     * Инициализирует клиента с указанным путем к сокету.
     * @param socket_path Путь к сокету для подключения.
     */
    Client(const std::string& socket_path);

    /**
     * @brief Деструктор класса Client.
     * 
     * Закрывает сокет, если он был открыт.
     */
    ~Client();

    /**
     * @brief Подключение к серверу.
     * 
     * Создает сокет и пытается подключиться к серверу.
     * @return true, если подключение успешно, иначе false.
     */
    bool connect_to_server();

    /**
     * @brief Отправка команды на сервер.
     * 
     * Отправляет строку команды на сервер.
     * @param command Команда для отправки.
     * @return true, если команда отправлена успешно, иначе false.
     */
    bool send_command(const std::string& command);

    /**
     * @brief Получение ответа от сервера.
     * 
     * Ожидает получения ответа от сервера и возвращает его в виде строки.
     * @return Ответ от сервера в виде строки или пустая строка в случае ошибки.
     */
    std::string receive_response();

private:
    /**
     * @brief Логгирование сообщений клиента.
     * 
     * Записывает сообщение в стандартный вывод с префиксом [Client Log].
     * @param message Сообщение для вывода.
     */
    void client_log(const std::string& message);

private:
    std::string socket_path; ///< Путь к сокету.
    int client_socket; ///< Дескриптор сокета клиента.
};
