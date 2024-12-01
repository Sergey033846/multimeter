#include "logger.h"
#include <iostream>

/**
 * @brief Конструктор класса ThreadSafeLogger.
 *
 * Инициализирует логгер и запускает рабочий поток для обработки сообщений.
 */
ThreadSafeLogger::ThreadSafeLogger() : logging_active(true) {
    // Запуск потока для обработки логгирования
    log_thread = std::thread(&ThreadSafeLogger::log_worker, this);
}

/**
 * @brief Деструктор класса ThreadSafeLogger.
 *
 * Останавливает процесс логгирования и завершает рабочий поток.
 */
ThreadSafeLogger::~ThreadSafeLogger() {
    stop_logging();  // Останавливаем процесс логгирования
    if (log_thread.joinable()) {
        log_thread.join();  // Ожидаем завершения рабочего потока
    }
}

/**
 * @brief Останавливает процесс логгирования и завершает рабочий поток.
 * 
 * Устанавливает флаг `loggingActive` в false и уведомляет рабочий поток о завершении работы.
 */
void ThreadSafeLogger::stop_logging() {    
    std::unique_lock<std::mutex> lock(log_mutex);
    logging_active = false;  // Устанавливаем флаг остановки
    log_cond_var.notify_one();  // Уведомляем рабочий поток
}

/**
 * @brief Добавляет сообщение в очередь для логгирования.
 * 
 * Это асинхронно добавляет сообщение в очередь. Рабочий поток будет извлекать и выводить его.
 * @param message Сообщение, которое необходимо залоггировать.
 */
void ThreadSafeLogger::log_msg(const std::string& message) {                       
    std::unique_lock<std::mutex> lock(log_mutex);  // Захватываем мьютекс для потока безопасности
    log_queue.push(message);  // Добавляем сообщение в очередь
    log_cond_var.notify_one();  // Уведомляем рабочий поток, что есть новое сообщение
}

/**
 * @brief Рабочий поток, который обрабатывает сообщения из очереди и выводит их в консоль.
 * 
 * Рабочий поток ожидает появления сообщений в очереди и выводит их в стандартный вывод.
 * Если флаг `loggingActive` становится ложным и очередь пуста, поток завершает свою работу.
 */
void ThreadSafeLogger::log_worker() {    
    while (true) {        
        std::string message;
        {
            std::unique_lock<std::mutex> lock(log_mutex);  // Захватываем мьютекс для безопасности доступа
            log_cond_var.wait(lock, [this] { return !logging_active || !log_queue.empty(); });  // Ожидаем новые сообщения

            if (!logging_active && log_queue.empty()) {
                break;  // Завершаем работу, если логгирование остановлено и очередь пуста
            }

            if (!log_queue.empty()) {
                message = log_queue.front();  // Извлекаем сообщение из очереди
                log_queue.pop();  // Убираем сообщение из очереди
            }
        }

        if (!message.empty()) {
            std::cout << message << std::endl;  // Выводим сообщение в консоль
        }
    }
}

