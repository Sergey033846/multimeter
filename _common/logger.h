#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <memory>

/**
 * @class ThreadSafeLogger
 * @brief Класс для потокобезопасного логгирования сообщений в отдельном потоке.
 *
 * Этот класс позволяет записывать сообщения в лог из разных потоков, обрабатывая их в одном рабочем потоке.
 * Он использует мьютекс и условную переменную для синхронизации доступа к очереди сообщений.
 */
class ThreadSafeLogger {
public:
    /**
     * @brief Получить единственный экземпляр логгера.
     * 
     * Используется паттерн одиночка (Singleton) для создания и доступа к логгеру.
     * @return Ссылку на единственный экземпляр класса ThreadSafeLogger.
     */
    static ThreadSafeLogger& get_instance() {
        static ThreadSafeLogger instance;
        return instance;
    }

    /**
     * @brief Конструктор класса ThreadSafeLogger.
     * 
     * Запускает рабочий поток для логгирования сообщений.
     */
    ThreadSafeLogger();

    /**
     * @brief Деструктор класса ThreadSafeLogger.
     * 
     * Останавливает логгирование и завершает рабочий поток.
     */
    ~ThreadSafeLogger();

    /**
     * @brief Останавливает процесс логгирования и завершает рабочий поток.
     * 
     * Устанавливает флаг `loggingActive` в false и уведомляет рабочий поток.
     */
    void stop_logging();

    /**
     * @brief Добавляет сообщение в очередь логгирования.
     * 
     * Это асинхронно добавляет сообщение в очередь, и рабочий поток будет его обрабатывать.
     * @param message Сообщение, которое необходимо залоггировать.
     */
    void log_msg(const std::string& message);

private:
    /**
     * @brief Рабочий поток, который обрабатывает сообщения из очереди и выводит их в консоль.
     *
     * Этот поток ожидает появления новых сообщений в очереди и выводит их в стандартный поток вывода.
     * После завершения работы он завершает свою работу.
     */
    void log_worker();    

    std::mutex log_mutex; ///< Мьютекс для синхронизации доступа к очереди сообщений
    std::condition_variable log_cond_var; ///< Условная переменная для уведомления потока о новых сообщениях
    std::queue<std::string> log_queue; ///< Очередь сообщений для логгирования
    std::atomic<bool> logging_active; ///< Флаг, показывающий активность логгирования
    std::thread log_thread; ///< Поток, обрабатывающий логгирование
};

/**
 * @namespace Log
 * @brief Пространство имен для логгирования.
 *
 * Удобный интерфейс для вызова логгирования через статический метод.
 */
namespace Log {
    /**
     * @brief Логирует сообщение, переданное в качестве аргумента.
     * 
     * Использует экземпляр логгера для добавления сообщения в очередь.
     * @param message Сообщение, которое будет залоггировано.
     */
    inline void log(const std::string& message) {
        ThreadSafeLogger::get_instance().log_msg(message);
    }
}
