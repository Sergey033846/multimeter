#include "task_pool.h"
#include "logger.h"

#include <vector>
#include <functional>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>

/**
 * @brief Конструктор класса TaskPool.
 * 
 * Создает пул потоков, каждый из которых будет выполнять метод worker_thread.
 * @param num_threads Количество потоков для создания в пуле.
 */
TaskPool::TaskPool(size_t num_threads) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back([this] { worker_thread(); });
    }
}

/**
 * @brief Деструктор класса TaskPool.
 * 
 * Останавливает пул потоков и ожидает завершения всех потоков.
 */
TaskPool::~TaskPool() {
    stop_pool();  
    for (auto &worker : workers) {
        if (worker.joinable()) {
            worker.join(); ///< Ожидаем завершения каждого потока
        }
    }
}

/**
 * @brief Останавливает пул потоков.
 * 
 * Устанавливает флаг остановки и уведомляет все потоки, чтобы они завершили выполнение.
 */
void TaskPool::stop_pool() {
    std::unique_lock<std::mutex> lock(queue_mutex); 
    stop.store(true);  ///< Устанавливаем флаг остановки
    cond_var.notify_all();  ///< Уведомляем все потоки, чтобы они завершили работу
}

/**
 * @brief Добавляет задачу в очередь для выполнения.
 * 
 * Эта функция добавляет задачу в очередь и уведомляет один из рабочих потоков о том, что задача готова к выполнению.
 * @param task Функция, которая будет выполнена потоком.
 */
void TaskPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (!stop.load()) {
            tasks.push(std::move(task));  ///< Добавляем задачу в очередь
        }
        cond_var.notify_one();  ///< Уведомляем один из потоков, что задача появилась
    }
}

/**
 * @brief Рабочий поток, который выполняет задачи из очереди.
 * 
 * Этот метод работает в бесконечном цикле, извлекая задачи из очереди и выполняя их. 
 * Рабочий поток завершит свою работу, если пул будет остановлен и очередь задач станет пустой.
 */
void TaskPool::worker_thread() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            cond_var.wait(lock, [this] { return stop || !tasks.empty(); });  ///< Ожидаем задачи или сигнала об остановке

            if (stop && tasks.empty()) {
                Log::log("Pool_thread return");  ///< Логируем завершение потока
                return;  ///< Завершаем работу потока
            }

            if (!tasks.empty()) {
                task = std::move(tasks.front());  ///< Извлекаем задачу из очереди
                tasks.pop();  ///< Убираем задачу из очереди
            }
        }
        task();  ///< Выполняем задачу
    }
}
