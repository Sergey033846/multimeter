#pragma once

#include "channel.h"
#include "analog_input.h"

#include <memory>

/**
 * @class ChannelFactory
 * @brief Фабричный класс для создания каналов.
 * 
 * Этот класс предоставляет статический метод для создания объектов каналов. В частности,
 * реализует метод для создания аналоговых входных каналов.
 */
class ChannelFactory {
public:
    /**
     * @brief Создает объект аналогового входного канала.
     * 
     * Этот метод создает объект канала типа `AnalogInput` с указанным именем.
     * Канал создается с использованием умного указателя `std::shared_ptr`, что
     * позволяет безопасно управлять его временем жизни.
     * 
     * @param name Имя канала.
     * @return Умный указатель на созданный объект канала.
     */
    static std::shared_ptr<IChannel> create_analog_input_channel(const std::string& name) {
        return std::make_shared<AnalogInput>(name);
    }
};
