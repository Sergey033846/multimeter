#include "channel.h"

/**
 * @brief Возвращает строковое представление состояния канала.
 * 
 * Эта функция возвращает строку, соответствующую состоянию канала, или "unknown" для неизвестных состояний.
 * 
 * @param state Состояние канала.
 * @return Строка, представляющая состояние канала.
 */
const std::string& ChannelStateManager::to_string(ChannelState state) {
    static const std::string unknown = "unknown";
    auto it = state_to_string.find(state);
    return (it != state_to_string.end()) ? it->second : unknown;
}

/**
 * @brief Добавляет пользовательское состояние канала.
 * 
 * Эта функция позволяет добавлять собственные состояния канала в систему.
 * 
 * @param state Новое состояние канала.
 * @param state_name Название состояния.
 */
void ChannelStateManager::add_custom_state(ChannelState state, const std::string& state_name) {
    state_to_string[state] = state_name;
}

/**
 * @brief Возвращает количество состояний канала.
 * 
 * Эта функция возвращает количество зарегистрированных состояний.
 * 
 * @return Количество состояний.
 */
size_t ChannelStateManager::size() {
    return state_to_string.size();
}

/**
 * @brief Конструктор канала.
 * 
 * Инициализирует канал с заданным именем и состоянием "Idle".
 * 
 * @param channel_name Имя канала.
 */
Channel::Channel(const std::string& channel_name)
    : name(channel_name), state(ChannelStateManager::ChannelState::Idle) {}

/**
 * @brief Деструктор канала.
 * 
 * Останавливает канал перед уничтожением.
 */
Channel::~Channel() = default;

/**
 * @brief Получает имя канала.
 * 
 * Эта функция возвращает имя канала.
 * 
 * @return Имя канала.
 */
const std::string& Channel::get_name() const {
    return name;
}

/**
 * @brief Получает текущее состояние канала.
 * 
 * Эта функция возвращает текущее состояние канала.
 * 
 * @return Состояние канала.
 */
ChannelStateManager::ChannelState Channel::get_state() const {
    std::shared_lock lock(mtx);    
    return state.load();
}

/**
 * @brief Устанавливает новое состояние канала.
 * 
 * Эта функция устанавливает новое состояние канала.
 * 
 * @param new_state Новое состояние канала.
 */
void Channel::set_state(ChannelStateManager::ChannelState new_state) {
    std::unique_lock lock(mtx); 
    state.store(new_state);
}
