#include "ranges.h"

#include <sstream>
#include <stdexcept>
#include <mutex>

//std::mutex RangeManager::mtx; // Инициализация мьютекса

/**
 * @brief Преобразование RangeConfig в строку.
 * 
 * Этот метод преобразует объект конфигурации диапазона в строковое представление,
 * включая минимальное и максимальное значения и точность.
 * @param range Конфигурация диапазона.
 * @return Строковое представление диапазона.
 */
std::string RangeManager::range_to_str(const RangeConfig& range) {
    std::ostringstream oss;
    oss.precision(range.precision);
    oss << "[" << range.min_value << ", " << range.max_value << "] (precision: " << range.precision << ")";
    return oss.str();
}

/**
 * @brief Получение контейнера с диапазонами (ленивая инициализация).
 * 
 * Этот метод возвращает ссылку на вектор диапазонов. Вектор инициализируется при первом доступе.
 * @return Ссылка на вектор диапазонов.
 */
std::vector<RangeManager::RangeConfig>& RangeManager::ranges() {
    static std::vector<RangeConfig> ranges = initialize_ranges();
    return ranges;
}

/**
 * @brief Инициализация глобальных диапазонов.
 * 
 * Метод создает набор стандартных диапазонов, которые инициализируются при первом запросе.
 * @return Вектор стандартных диапазонов.
 */
std::vector<RangeManager::RangeConfig> RangeManager::initialize_ranges() {
    return {
        {0.0000001f, 0.001f, 7},
        {0.001f, 1.0f, 3},
        {1.0f, 1000.0f, 1},
        {1000.0f, 1000000.0f, 1}
    };
}

/**
 * @brief Получение строкового представления диапазона по его ID.
 * 
 * Метод получает строковое представление диапазона по его уникальному идентификатору.
 * В случае ошибки выбрасывает исключение `std::out_of_range`, если ID некорректен.
 * @param id Идентификатор диапазона.
 * @return Строковое представление диапазона.
 * @throws std::out_of_range Если ID не существует.
 */
std::string RangeManager::to_string(RangeID id) {
    std::shared_lock lock(mtx); ///< Блокировка для безопасного доступа
    if (id >= ranges().size()) {
        throw std::out_of_range("Invalid RangeID");
    }
    return range_to_str(ranges()[id]);
}

/**
 * @brief Добавление нового диапазона (глобального или пользовательского).
 * 
 * Этот метод добавляет новый диапазон в список доступных диапазонов.
 * @param range Конфигурация нового диапазона.
 */
void RangeManager::add_range(const RangeConfig& range) {
    std::unique_lock lock(mtx); ///< Блокировка для безопасного изменения данных
    ranges().push_back(range);
}

/**
 * @brief Получение общего количества диапазонов.
 * 
 * Метод возвращает количество доступных диапазонов.
 * @return Общее количество диапазонов.
 */
size_t RangeManager::size() {
    std::shared_lock lock(mtx); ///< Блокировка для безопасного доступа
    return ranges().size();
}

/**
 * @brief Доступ к диапазону по ID.
 * 
 * Этот метод возвращает конфигурацию диапазона по его уникальному идентификатору.
 * В случае ошибки выбрасывает исключение `std::out_of_range`, если ID некорректен.
 * @param id Идентификатор диапазона.
 * @return Конфигурация диапазона.
 * @throws std::out_of_range Если ID не существует.
 */
const RangeManager::RangeConfig& RangeManager::get_range(RangeID id) {
    std::shared_lock lock(mtx); ///< Блокировка для безопасного доступа
    if (id >= ranges().size()) {
        throw std::out_of_range("Invalid RangeID");
    }
    return ranges()[id];
}
