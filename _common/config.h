#pragma once

#include <cstddef>

namespace MyConfig {

/**
 * @brief Структура, содержащая значения по умолчанию для конфигурации.
 * 
 * Структура позволяет централизованно хранить константные параметры по умолчанию,
 * такие как частота опроса, количество каналов и другие.
 */
struct DefaultConfig {
    // Частота опроса (таймаут в мс)
    static constexpr int polling_frequency = 100;

    // Количество каналов 
    static constexpr int num_channels = 2;

    // Диапазон измерений
    static constexpr int range = 0;    
};

} 
