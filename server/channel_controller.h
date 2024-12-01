#pragma once

#include "channel.h"
#include "channel_factory.h"
#include <stdexcept>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <memory>
#include <thread>
#include <atomic>
#include <random>
#include <chrono>

/**
 * @class ChannelController
 * @brief Контроллер управления каналами.
 * 
 * Этот класс управляет коллекцией каналов, предоставляет методы для добавления,
 * поиска и остановки каналов. Также генерирует случайные состояния для каждого канала
 * в отдельном потоке.
 */
class ChannelController {
public:
    /**
     * @brief Конструктор ChannelController.
     * 
     * Инициализирует контроллер с указанным количеством каналов и запускает поток,
     * генерирующий случайные состояния для каналов.
     * 
     * @param channel_count Количество каналов, которые будут добавлены в контроллер.
     */
    ChannelController(size_t channel_count);

    /**
     * @brief Деструктор ChannelController.
     * 
     * Останавливает поток генерации состояний и завершает работу с каналами.
     */
    ~ChannelController();

    /**
     * @brief Добавляет новый канал в контроллер.
     * 
     * Эта функция добавляет указанный канал в контроллер для дальнейшей работы.
     * 
     * @param channel Канал, который нужно добавить.
     */
    void add_channel(std::shared_ptr<IChannel>&& channel);

    /**
     * @brief Останавливает все каналы в контроллере.
     * 
     * Эта функция останавливает все каналы, управляемые контроллером.
     */
    void stop();

    /**
     * @brief Ищет канал по имени.
     * 
     * Эта функция ищет канал в контроллере по его имени и возвращает его,
     * если он найден.
     * 
     * @param channel_name Имя канала.
     * @return Указатель на канал или nullptr, если канал не найден.
     */
    std::shared_ptr<IChannel> find_channel(const std::string& channel_name) const;

private:
    /**
     * @brief Функция для генерации случайных состояний для каналов.
     * 
     * Этот поток генерирует случайные состояния для всех каналов в контроллере
     * и обновляет их состояния с определенной периодичностью.
     */
    void state_generator();

    // Маппинг имени канала на его указатель
    std::unordered_map<std::string, std::shared_ptr<IChannel>> channels;

    // Мьютекс для потокобезопасного доступа к каналам
    mutable std::shared_mutex map_mutex;

    // Поток генерации состояний
    std::thread thread_state_gen;

    // Флаг для остановки генерации состояний
    std::atomic<bool> stop_state_gen;
};
