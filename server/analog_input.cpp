#include "analog_input.h"
#include "ranges.h"
#include "my_tools.h"
#include "config.h"

#include <stdexcept>
#include <thread>
#include <chrono>

/**
 * @brief Конструктор класса AnalogInput.
 * 
 * Инициализирует канал с именем, диапазоном, частотой, флагом работы и значением измерения.
 * Канал по умолчанию находится в состоянии ожидания.
 * 
 * @param name Имя канала.
 */
AnalogInput::AnalogInput(const std::string& name)
    : Channel(name), range(MyConfig::DefaultConfig::range), frequency(MyConfig::DefaultConfig::polling_frequency), 
        running(false), measuring_value(0.0f) {
    state = ChannelStateManager::ChannelState::Idle;
}

/**
 * @brief Деструктор класса AnalogInput.
 * 
 * Останавливает канал и завершает его работу.
 */
AnalogInput::~AnalogInput() {
    stop();
}

/**
 * @brief Устанавливает новый диапазон для канала.
 * 
 * Устанавливает диапазон измерений. Если диапазон выходит за пределы допустимых значений,
 * выбрасывается исключение.
 * 
 * @param new_range Новый диапазон канала.
 * @throws std::out_of_range Если диапазон некорректен.
 */
void AnalogInput::set_range(int new_range) { 
    std::lock_guard<std::mutex> lock(mtx);   
    if (new_range < 0 || new_range >= RangeManager::size()) {
        throw std::out_of_range("Invalid range value");
    }    
    range = new_range;
}

/**
 * @brief Возвращает текущий диапазон канала.
 * 
 * @return Текущий диапазон канала.
 */
int AnalogInput::get_range() const {
    std::lock_guard<std::mutex> lock(mtx);
    return range;
}

/**
 * @brief Устанавливает частоту измерений канала.
 * 
 * Устанавливает частоту измерений в миллисекундах. Если частота некорректна,
 * выбрасывается исключение.
 * 
 * @param freq Частота измерений.
 * @throws std::invalid_argument Если частота некорректна.
 */
void AnalogInput::set_frequency(int freq) {
    std::lock_guard<std::mutex> lock(mtx);
    if (freq <= 0) {
        throw std::invalid_argument("Frequency must be positive");
    }
    frequency = freq;
}

/**
 * @brief Возвращает текущую частоту измерений.
 * 
 * @return Частота измерений.
 */
int AnalogInput::get_frequency() const {
    std::lock_guard<std::mutex> lock(mtx);
    return frequency;
}

/**
 * @brief Запускает процесс измерений.
 * 
 * Запускает новый поток для измерений. Канал начинает работать и изменяет свое состояние.
 */
void AnalogInput::start() {
    std::lock_guard<std::mutex> lock(mtx);
    if (running.load()) return;        
    running.store(true);
    channel_thread = std::thread(&AnalogInput::channel_loop, this);    
    set_state(ChannelStateManager::ChannelState::Measure);
}

/**
 * @brief Останавливает процесс измерений.
 * 
 * Останавливает работу канала и завершает поток измерений.
 */
void AnalogInput::stop() {        
    std::lock_guard<std::mutex> lock(mtx);
    
    if (!running.load()) return;

    running.store(false);
    if (channel_thread.joinable()) {
        channel_thread.join();
    }
    set_state(ChannelStateManager::ChannelState::Idle);
}

/**
 * @brief Возвращает текущее измеряемое значение.
 * 
 * Возвращает последнее измеренное значение.
 * 
 * @return Последнее измеренное значение.
 */
float AnalogInput::get_measuring_value() const {
    return measuring_value.load();
}

/**
 * @brief Внутренний метод для работы канала.
 * 
 * Циклически генерирует случайные значения в пределах текущего диапазона и обновляет
 * измеряемое значение с заданной частотой.
 */
void AnalogInput::channel_loop() {    
    while (running.load()) {        
        const auto& current_range = RangeManager::get_range(range);
        
        // Генерируем случайное значение в пределах диапазона
        float value = MyTools::generate_random_value(current_range.min_value, current_range.max_value);

        measuring_value.store(value);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
    }
}
