#pragma once

#include <vector>
#include <string>
#include <shared_mutex>

/**
 * @class RangeManager
 * @brief Класс для управления диапазонами.
 *
 * Этот класс управляет набором диапазонов, обеспечивая добавление, получение, преобразование в строку и доступ к диапазонам.
 * Также реализует синхронизацию доступа с помощью мьютекса.
 */
class RangeManager {
public:
    /**
     * @struct RangeConfig
     * @brief Структура для представления конфигурации диапазона.
     *
     * Структура содержит минимальное значение, максимальное значение и точность для диапазона.
     */
    struct RangeConfig {
        float min_value; ///< Минимальное значение диапазона
        float max_value; ///< Максимальное значение диапазона
        int precision; ///< Точность отображения значения в диапазоне
    };

    using RangeID = size_t; ///< Тип идентификатора диапазона

    /**
     * @brief Получение строкового представления диапазона по его ID.
     * @param id Идентификатор диапазона.
     * @return Строковое представление диапазона.
     * @throws std::out_of_range Если ID не существует.
     */
    static std::string to_string(RangeID id);

    /**
     * @brief Добавление нового диапазона (глобального или пользовательского).
     * @param range Конфигурация диапазона для добавления.
     */
    static void add_range(const RangeConfig& range);

    /**
     * @brief Получение общего количества диапазонов.
     * @return Общее количество диапазонов.
     */
    static size_t size();

    /**
     * @brief Доступ к диапазону по ID.
     * @param id Идентификатор диапазона.
     * @return Конфигурация диапазона.
     * @throws std::out_of_range Если ID не существует.
     */
    static const RangeConfig& get_range(RangeID id);

private:
    /**
     * @brief Доступ к контейнеру с диапазонами (лениво инициализируется).
     * 
     * Этот метод инициализирует контейнер диапазонов только при первом доступе.
     * @return Ссылка на вектор диапазонов.
     */
    static std::vector<RangeConfig>& ranges();

    /**
     * @brief Преобразование RangeConfig в строку.
     * @param range Конфигурация диапазона.
     * @return Строковое представление диапазона.
     */
    static std::string range_to_str(const RangeConfig& range);

    /**
     * @brief Инициализация глобальных диапазонов.
     * 
     * Метод создает и возвращает список стандартных диапазонов.
     * @return Вектор стандартных диапазонов.
     */
    static std::vector<RangeConfig> initialize_ranges();

    /// Мьютекс для синхронизации доступа к диапазонам
    inline static std::shared_mutex mtx;
};
