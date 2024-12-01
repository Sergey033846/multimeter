#include "multimeter.h"

#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sstream>
#include <signal.h>
#include <errno.h>

/**
 * @brief Конструктор класса Multimeter.
 * 
 * Инициализирует сервер с указанным количеством потоков и каналов.
 * 
 * @param socket_path Путь к Unix-сокету для соединений.
 * @param thread_count Количество потоков в пуле.
 * @param channel_count Количество каналов.
 */
Multimeter::Multimeter(const std::string& socket_path, size_t thread_count, size_t channel_count)
    : pool(thread_count), channel_controller(channel_count), socket_path(socket_path) {
    Log::log("Multimeter is ready to work");
}

/**
 * @brief Деструктор класса Multimeter.
 * 
 * Останавливает сервер и закрывает сокет.
 */
Multimeter::~Multimeter() {
    channel_controller.stop();
    stop();
    Log::log("Multimeter is turned off");
}

/**
 * @brief Добавляет канал в систему.
 * 
 * @param channel Канал, который будет добавлен.
 */
void Multimeter::add_channel(std::shared_ptr<IChannel>&& channel) {
    channel_controller.add_channel(std::move(channel));
}

/**
 * @brief Запускает сервер и начинает прослушивание запросов от клиентов.
 * 
 * Эта функция блокирует выполнение и обрабатывает клиентские соединения.
 */
void Multimeter::run() {
    setup_signal_handler();
    setup_socket();

    Log::log("Multimeter is running...");

    fd_set readfds;
    struct timeval timeout;

    while (server_running) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int select_result = select(server_socket + 1, &readfds, nullptr, nullptr, &timeout);

        if (select_result == -1) {
            if (errno == EINTR) {
                continue; // Если select был прерван сигналом, продолжаем выполнение
            }
            perror("select");
            break;
        } else if (select_result == 0) {
            continue; // Тайм-аут, продолжаем
        } else {
            if (FD_ISSET(server_socket, &readfds)) {
                int client_socket = accept(server_socket, nullptr, nullptr);
                if (client_socket == -1) {
                    perror("accept");
                    continue;
                }

                pool.enqueue([this, client_socket] {
                    handle_client(client_socket);
                });
            }
        }
    }
}

/**
 * @brief Останавливает сервер и освобождает ресурсы.
 * 
 * Закрывает сокет и очищает все ресурсы.
 */
void Multimeter::stop() {
    server_running = false;
    if (server_socket != -1) {
        close(server_socket);
        unlink(socket_path.c_str());
    }
    Log::log("Multimeter is stopped");
}

/**
 * @brief Настроить обработчик сигнала для безопасного завершения работы сервера.
 * 
 * Устанавливает обработчик для сигнала SIGINT, чтобы корректно завершить работу.
 */
void Multimeter::setup_signal_handler() {
    struct sigaction sa{};
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, nullptr) == -1) {
        perror("sigaction");
        exit(1);
    }
}

/**
 * @brief Настроить сокет сервера.
 * 
 * Создает сокет, привязывает его к пути и начинает прослушивание.
 */
void Multimeter::setup_socket() {
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path.c_str());

    unlink(socket_path.c_str());

    if (bind(server_socket, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(server_socket, 5) == -1) {
        perror("listen");
        exit(1);
    }

    set_socket_nonblocking(server_socket);
}

/**
 * @brief Устанавливает сокет в неблокирующий режим.
 * 
 * @param socket Дескриптор сокета.
 */
void Multimeter::set_socket_nonblocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        exit(1);
    }
    flags |= O_NONBLOCK;
    if (fcntl(socket, F_SETFL, flags) == -1) {
        perror("fcntl F_SETFL");
        exit(1);
    }
}

/**
 * @brief Обработчик сигнала для завершения работы сервера.
 * 
 * @param signum Номер сигнала.
 */
void Multimeter::signal_handler(int signum) {
    Log::log("Received signal " + std::to_string(signum) + ", stopping the server...");
    server_running = false;
}

/**
 * @brief Обрабатывает запросы от клиента.
 * 
 * Читает команду от клиента и обрабатывает её.
 * 
 * @param client_socket Дескриптор сокета клиента.
 */
void Multimeter::handle_client(int client_socket) {
    char buffer[256];
    while (server_running) {
        ssize_t bytes_received = read(client_socket, buffer, sizeof(buffer) - 1);
        if (bytes_received <= 0) break;
        buffer[bytes_received] = '\0';

        std::string command(buffer);
        Log::log("--> [ Client " + std::to_string(client_socket) + " ] send command [" + command + "]");

        std::string response = process_command(command);

        write(client_socket, response.c_str(), response.size());
    }
    close(client_socket);
}

/**
 * @brief Обрабатывает команду от клиента.
 * 
 * Парсит команду и выполняет её, возвращая результат.
 * 
 * @param command_string Строка команды.
 * @return Ответ на команду.
 */
std::string Multimeter::process_command(const std::string& command_string) const {
    std::string response = "unknown command or parameters";

    // парсим строку команды
    std::string command_name;
    std::vector<std::string> parameters;
    parse_command_string(command_string, command_name, parameters);

    if (parameters.size()) {
        std::shared_ptr<IChannel> channel = channel_controller.find_channel(parameters[0]);

        if (channel) {
            std::shared_ptr<ICommand> command = CommandFactory::create_command(command_name, channel, parameters);
            if (command) {
                response = command->execute();
            }
        }
        else {
            response = "There is no such channel [" + parameters[0] + "]!";
        }
    }

    return response;
}

/**
 * @brief Разбирает строку команды.
 * 
 * Извлекает имя команды и параметры из строки.
 * 
 * @param input Строка с командой от клиента.
 * @param command_name Имя команды.
 * @param parameters Список параметров.
 */
void Multimeter::parse_command_string(const std::string& input, std::string& command_name, std::vector<std::string>& parameters) const {
    std::istringstream stream(input);

    std::getline(stream, command_name, ' ');

    std::string params;
    std::getline(stream, params);

    std::stringstream params_stream(params);
    std::string param;
    while (std::getline(params_stream, param, ',')) {
        // Убираем возможные пробелы перед параметрами
        param.erase(param.begin(), std::find_if(param.begin(), param.end(), [](unsigned char ch) { return !std::isspace(ch); }));
        // Убираем возможные пробелы после параметров
        param.erase(std::find_if(param.rbegin(), param.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), param.end());
        parameters.push_back(param);
    }
}
