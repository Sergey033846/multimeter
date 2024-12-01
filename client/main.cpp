#include "client.h"
#include <iostream>

#define SOCKET_PATH "/tmp/multimeter_socket"

/**
 * @brief Основная функция клиента.
 * 
 * Эта функция инициализирует клиент, подключается к серверу и взаимодействует с ним,
 * отправляя команды и получая ответы.
 * 
 * @return Код завершения программы (0 - успех, 1 - ошибка).
 */
int main() {
    Client client(SOCKET_PATH);

    std::cout << "Starting client...\n";

    if (!client.connectToServer()) {
        std::cerr << "Failed to connect to server\n";
        return 1;
    }

    std::cout << "Connected to server\n";

    // Взаимодействие с сервером
    std::string command;
    while (true) {
        std::cout << "Enter command (or 'exit' to quit): ";
        std::getline(std::cin, command);

        if (command == "exit") {
            break;
        }

        if (command.empty()) {
            std::cerr << "Command cannot be empty. Please try again." << std::endl;
            continue;
        }

        if (command.size() >= 256) {
            std::cerr << "Command too long, please use a shorter command." << std::endl;
            continue;
        }

        if (!client.sendCommand(command)) {
            std::cerr << "Failed to send command" << std::endl;
            continue;
        }

        std::string response = client.receiveResponse();
        if (response.empty()) {
            std::cerr << "Failed to receive response or connection closed by server" << std::endl;
            break;
        } else {
            std::cout << "Server response: " << response << std::endl;
        }
    }

    std::cout << "Client exiting..." << std::endl;

    return 0;
}
