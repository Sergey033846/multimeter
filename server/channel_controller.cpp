#include "channel_controller.h"
#include "logger.h" 

/**
 * @brief Конструктор ChannelController.
 * 
 * Инициализирует контроллер с указанным количеством каналов и запускает поток,
 * генерирующий случайные состояния для каналов.
 * 
 * @param channel_count Количество каналов, которые будут добавлены в контроллер.
 */
ChannelController::ChannelController(size_t channel_count) : stop_state_gen(false) {
    for (size_t i = 0; i != channel_count; ++i) {
        add_channel(ChannelFactory::create_analog_input_channel("channel" + std::to_string(i)));
    }

    // Запуск потока генерации состояний
    thread_state_gen = std::thread(&ChannelController::state_generator, this);
}

/**
 * @brief Деструктор ChannelController.
 * 
 * Останавливает поток генерации состояний и завершает работу с каналами.
 */
ChannelController::~ChannelController() {
    stop_state_gen = true;
    if (thread_state_gen.joinable()) {
        thread_state_gen.join();
    }
}

/**
 * @brief Добавляет новый канал в контроллер.
 * 
 * Эта функция добавляет указанный канал в контроллер для дальнейшей работы.
 * 
 * @param channel Канал, который нужно добавить.
 */
void ChannelController::add_channel(std::shared_ptr<IChannel>&& channel) {
    std::string channel_name = channel->get_name();
    {            
        std::unique_lock lock(map_mutex);
        channels[channel_name] = std::move(channel);   
    }
    Log::log("ChannelController Channel " + channel_name + " added");     
}

/**
 * @brief Останавливает все каналы в контроллере.
 * 
 * Эта функция останавливает все каналы, управляемые контроллером.
 */
void ChannelController::stop() {
    std::unique_lock lock(map_mutex);
    for (const auto& channel : channels) {
        channel.second->stop();
    }
}

/**
 * @brief Ищет канал по имени.
 * 
 * Эта функция ищет канал в контроллере по его имени и возвращает его,
 * если он найден.
 * 
 * @param channel_name Имя канала.
 * @return Указатель на канал или nullptr, если канал не найден.
 */
std::shared_ptr<IChannel> ChannelController::find_channel(const std::string& channel_name) const {
    std::shared_lock lock(map_mutex);
    auto it = channels.find(channel_name);        
    if (it != channels.end()) {
        return it->second;  
    }

    return nullptr;
}

/**
 * @brief Функция для генерации случайных состояний для каналов.
 * 
 * Этот поток генерирует случайные состояния для всех каналов в контроллере
 * и обновляет их состояния с определенной периодичностью.
 */
void ChannelController::state_generator() {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> state_dist(0, ChannelStateManager::size() - 1);

    while (!stop_state_gen) {
        {
            std::unique_lock lock(map_mutex);
            for (auto& [name, channel] : channels) {
                ChannelStateManager::ChannelState random_state = static_cast<ChannelStateManager::ChannelState>(state_dist(rng));
                channel->set_state(random_state);
                Log::log("Channel [" + name + "] state updated to " + ChannelStateManager::to_string(random_state));
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(10)); // Пауза в 10 секунд между обновлениями
    }
}
