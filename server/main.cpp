#include "multimeter.h"

#include <iostream>
#include <iomanip>

// Путь к сокету для подключения
#define SOCKET_PATH "/tmp/multimeter_socket"

int main() {        
    // Создание объекта Multimeter с заданным путем к сокету и количеством каналов
    Multimeter multimeter(SOCKET_PATH, 3, 4);

    // Запуск работы мультиметра
    multimeter.run();
      
    return 0;
}