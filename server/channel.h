#pragma once

#include <string>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <atomic>
#include <memory>

/**
 * @class ChannelStateManager
 * @brief Класс для управления состояниями каналов.
 * 
 * Этот класс позволяет управлять состояниями каналов, конвертировать их в строковое представление,
 * а также добавлять собственные состояния.
 */
class ChannelStateManager {
public:
    /**
     * @enum ChannelState
     * @brief Перечисление возможных состояний канала.
     */
    enum class ChannelState : int { 
        Idle,    ///< Канал в режиме ожидания (не активен).
        Measure, ///< Канал измеряет значение.
        Busy,    ///< Канал занят (например, выполняет другую операцию).
        Error    ///< Канал в состоянии ошибки.
    };

    /**
     * @brief Возвращает строковое представление состояния канала.
     * 
     * Эта функция возвращает строку, представляющую состояние канала.
     * 
     * @param state Состояние канала.
     * @return Строковое представление состояния канала.
     */
    static const std::string& to_string(ChannelState state);

    /**
     * @brief Добавляет пользовательское состояние канала.
     * 
     * Эта функция позволяет добавлять собственные состояния канала в систему.
     * 
     * @param state Новое состояние канала.
     * @param state_name Название состояния.
     */
    static void add_custom_state(ChannelState state, const std::string& state_name);

    /**
     * @brief Возвращает количество состояний канала.
     * 
     * Эта функция возвращает количество зарегистрированных состояний.
     * 
     * @return Количество состояний.
     */
    static size_t size();

private:
    // Маппинг состояний канала на их строковые представления
    inline static std::unordered_map<ChannelState, std::string> state_to_string = {
        {ChannelState::Idle, "idle_state"},
        {ChannelState::Measure, "measure_state"},
        {ChannelState::Busy, "busy_state"},
        {ChannelState::Error, "error_state"}
    };
};

/**
 * @interface IChannel
 * @brief Интерфейс канала.
 * 
 * Этот интерфейс определяет базовые функции для работы с каналом,
 * такие как управление состоянием канала, его настройка и выполнение измерений.
 */
class IChannel {
public:
    virtual ~IChannel() = default;

    /**
     * @brief Получает имя канала.
     * 
     * Эта функция возвращает имя канала.
     * 
     * @return Имя канала.
     */
    virtual const std::string& get_name() const = 0;

    /**
     * @brief Запускает канал.
     * 
     * Эта функция активирует канал для выполнения измерений.
     */
    virtual void start() = 0;

    /**
     * @brief Останавливает канал.
     * 
     * Эта функция останавливает канал и завершает его работу.
     */
    virtual void stop() = 0;

    /**
     * @brief Устанавливает диапазон канала.
     * 
     * Эта функция устанавливает диапазон для измерений канала.
     * 
     * @param range Диапазон.
     */
    virtual void set_range(int range) = 0;

    /**
     * @brief Получает диапазон канала.
     * 
     * Эта функция возвращает текущий диапазон канала.
     * 
     * @return Диапазон канала.
     */
    virtual int get_range() const = 0;

    /**
     * @brief Устанавливает частоту канала.
     * 
     * Эта функция устанавливает частоту измерений канала.
     * 
     * @param frequency Частота.
     */
    virtual void set_frequency(int frequency) = 0;

    /**
     * @brief Получает частоту канала.
     * 
     * Эта функция возвращает текущую частоту канала.
     * 
     * @return Частота канала.
     */
    virtual int get_frequency() const = 0;

    /**
     * @brief Получает значение измерения канала.
     * 
     * Эта функция возвращает значение, полученное канала в процессе измерений.
     * 
     * @return Значение измерения.
     */
    virtual float get_measuring_value() const = 0;

    /**
     * @brief Получает текущее состояние канала.
     * 
     * Эта функция возвращает состояние канала.
     * 
     * @return Состояние канала.
     */
    virtual ChannelStateManager::ChannelState get_state() const = 0;

    /**
     * @brief Устанавливает новое состояние канала.
     * 
     * Эта функция устанавливает новое состояние канала.
     * 
     * @param new_state Новое состояние канала.
     */
    virtual void set_state(ChannelStateManager::ChannelState new_state) = 0;
};

/**
 * @class Channel
 * @brief Реализация канала, который использует интерфейс IChannel.
 * 
 * Этот класс представляет собой конкретную реализацию канала, который может иметь различные состояния,
 * частоты и диапазоны. Он также поддерживает потокобезопасный доступ к своим данным.
 */
class Channel : public IChannel {
public:
    /**
     * @brief Конструктор канала.
     * 
     * Инициализирует канал с заданным именем.
     * 
     * @param channel_name Имя канала.
     */
    explicit Channel(const std::string& channel_name);

    /**
     * @brief Деструктор канала.
     * 
     * Останавливает канал перед уничтожением.
     */
    virtual ~Channel();

    /**
     * @brief Получает имя канала.
     * 
     * Эта функция возвращает имя канала.
     * 
     * @return Имя канала.
     */
    const std::string& get_name() const override;

    /**
     * @brief Получает текущее состояние канала.
     * 
     * Эта функция возвращает текущее состояние канала.
     * 
     * @return Состояние канала.
     */
    ChannelStateManager::ChannelState get_state() const override;

    /**
     * @brief Устанавливает новое состояние канала.
     * 
     * Эта функция устанавливает новое состояние канала.
     * 
     * @param new_state Новое состояние канала.
     */
    void set_state(ChannelStateManager::ChannelState new_state) override;

    /**
     * @brief Запускает канал.
     * 
     * Эта функция активирует канал для выполнения измерений. Реализация зависит от типа канала.
     */
    virtual void start() override = 0;

    /**
     * @brief Останавливает канал.
     * 
     * Эта функция останавливает канал и завершает его работу.
     */
    virtual void stop() override = 0;

protected:
    mutable std::shared_mutex mtx; ///< Мьютекс для потокобезопасного доступа к данным канала.
    std::string name;               ///< Имя канала.
    std::atomic<ChannelStateManager::ChannelState> state; ///< Состояние канала.
};
