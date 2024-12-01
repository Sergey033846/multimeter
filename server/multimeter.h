#pragma once

#include <string>
#include <memory>
#include <vector>
#include <atomic>
#include "task_pool.h"
#include "logger.h"
#include "command_factory.h"
#include "channel_controller.h"
#include "config.h"

/**
 * @class Multimeter
 * @brief Класс сервера мультиметра, который управляет каналами и выполняет команды от клиентов.
 * 
 * Этот класс реализует сервер, который слушает Unix-сокет, обрабатывает команды от клиентов, 
 * а также управляет набором каналов. Сервер использует пул потоков для асинхронной обработки 
 * запросов.
 */
class Multimeter {
public:
    /**
     * @brief Конструктор класса Multimeter.
     * 
     * Инициализирует сервер, создавая пул задач и контроллер каналов.
     * 
     * @param socket_path Путь к Unix-сокету для соединения с клиентами.
     * @param thread_count Количество потоков в пуле задач для обработки запросов.
     * @param channel_count Количество каналов, которые может обслуживать сервер.
     */
    explicit Multimeter(const std::string& socket_path, size_t thread_count, size_t channel_count = MyConfig::DefaultConfig::num_channels);

    /**
     * @brief Деструктор класса Multimeter.
     * 
     * Останавливает сервер и закрывает сокет.
     */
    ~Multimeter();

    /**
     * @brief Добавляет канал в систему.
     * 
     * @param channel Канал, который будет добавлен.
     */
    void add_channel(std::shared_ptr<IChannel>&& channel);

    /**
     * @brief Запускает сервер и начинает прослушивание запросов от клиентов.
     * 
     * Эта функция блокирует выполнение и обрабатывает клиентские соединения,
     * вызывая соответствующие команды в пуле потоков.
     */
    void run();

    /**
     * @brief Останавливает сервер и освобождает ресурсы.
     * 
     * Завершает работу сервера, закрывает сокет и очищает ресурсы.
     */
    void stop();

private:
    /**
     * @brief Обработчик сигнала для корректного завершения работы сервера.
     * 
     * Этот метод вызывается при получении сигнала (например, SIGINT).
     * 
     * @param signum Номер сигнала.
     */
    static void signal_handler(int signum);

    /**
     * @brief Устанавливает сокет в неблокирующий режим.
     * 
     * Этот метод меняет флаги сокета, чтобы он работал в неблокирующем режиме.
     * 
     * @param socket Дескриптор сокета.
     */
    static void set_socket_nonblocking(int socket);

    /**
     * @brief Настроить обработчик сигнала.
     * 
     * Эта функция настраивает обработчик сигнала SIGINT (например, для безопасного завершения работы).
     */
    void setup_signal_handler();

    /**
     * @brief Настроить сокет сервера.
     * 
     * Этот метод создает сокет, привязывает его к указанному пути и начинает прослушивание входящих соединений.
     */
    void setup_socket();

    /**
     * @brief Обрабатывает запросы от клиента.
     * 
     * Читает команду от клиента и обрабатывает её.
     * 
     * @param client_socket Дескриптор сокета клиента.
     */
    void handle_client(int client_socket);

    /**
     * @brief Обрабатывает строку команды и возвращает соответствующий ответ.
     * 
     * Парсит команду, выполняет её через соответствующий канал и возвращает результат.
     * 
     * @param command_string Строка, представляющая команду от клиента.
     * @return Ответ на команду.
     */
    std::string process_command(const std::string& command_string) const;

    /**
     * @brief Разбирает строку команды на имя команды и параметры.
     * 
     * Эта функция разделяет строку команды на её имя и список параметров.
     * 
     * @param input Строка с командой от клиента.
     * @param command_name Имя команды.
     * @param parameters Список параметров для команды.
     */
    void parse_command_string(const std::string& input, std::string& command_name, std::vector<std::string>& parameters) const;

    int server_socket = -1; ///< Дескриптор сокета сервера.
    TaskPool pool; ///< Пул потоков для асинхронной обработки запросов.
    ChannelController channel_controller; ///< Контроллер каналов.
    std::string socket_path; ///< Путь к Unix-сокету.
    static inline std::atomic<bool> server_running = true; ///< Флаг работы сервера.
};
