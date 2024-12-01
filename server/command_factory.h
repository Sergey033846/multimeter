#pragma once

#include "channel.h"
#include "commands.h"

#include <memory>
#include <functional>
#include <shared_mutex>

/**
 * @class CommandFactory
 * @brief Фабрика команд для создания объектов команд.
 * 
 * Этот класс предоставляет статический метод для создания команд на основе
 * имени команды, канала и параметров. Он инкапсулирует логику создания команд и
 * управления их маппингом.
 */
class CommandFactory {
public:
    /// Тип канала, который используется в командах
    using TypeChannel = std::shared_ptr<IChannel>;

    /// Тип параметров, передаваемых в команды
    using TypeParams = const std::vector<std::string>&;

    /// Тип команды (функция, которая создает команду на основе канала и параметров)
    using TypeCommand = std::function<std::shared_ptr<ICommand>(TypeChannel, TypeParams)>;

    /**
     * @brief Создает команду на основе имени команды и переданных параметров.
     * 
     * Этот метод ищет команду по ее имени в маппинге команд и создает объект соответствующей команды.
     * Если команда с таким именем не найдена, возвращается `nullptr`.
     * 
     * @param command_name Имя команды для создания.
     * @param channel Канал, на котором будет выполняться команда.
     * @param params Параметры, передаваемые в команду.
     * @return Умный указатель на созданную команду или `nullptr`, если команда не найдена.
     */
    static std::shared_ptr<ICommand> create_command(
        const std::string& command_name, TypeChannel channel, TypeParams params) { 
        std::shared_lock lock(get_mutex()); // Разделяемая блокировка для чтения  
        const auto& command_map = get_command_map();
        auto it = command_map.find(command_name);
        if (it != command_map.end()) {
            return it->second(channel, params); 
        }
        return nullptr; // Команда не найдена
    }

private:
    /**
     * @brief Возвращает ссылку на карту команд.
     * 
     * Эта функция инициализирует и возвращает карту команд, которая хранит
     * ассоциативные пары (имя команды -> функция для создания команды).
     * Карта команд инициализируется один раз при первом вызове этой функции.
     * 
     * @return Ссылка на карту команд.
     */
    static const std::unordered_map<std::string, TypeCommand>& get_command_map() {
        // Статическая инициализация карты команд (выполняется один раз)
        static std::unordered_map<std::string, TypeCommand> command_map = {
            {"get_status", [](TypeChannel channel, TypeParams params) {
                return std::make_shared<GetStatusCommand>(channel, params);
            }},
            {"start_measure", [](TypeChannel channel, TypeParams params) {
                return std::make_shared<StartMeasureCommand>(channel, params);
            }},
            {"stop_measure", [](TypeChannel channel, TypeParams params) {
                return std::make_shared<StopMeasureCommand>(channel, params);
            }},
            {"get_result", [](TypeChannel channel, TypeParams params) {
                return std::make_shared<GetResultCommand>(channel, params);
            }},
            {"set_range", [](TypeChannel channel, TypeParams params) {
                return std::make_shared<SetRangeCommand>(channel, params);
            }},
            {"set_frequency", [](TypeChannel channel, TypeParams params) {
                return std::make_shared<SetFrequencyCommand>(channel, params);
            }}
        };

        return command_map;
    }

    /**
     * @brief Возвращает ссылку на мьютекс.
     * 
     * Этот метод возвращает ссылку на мьютекс, который используется для
     * синхронизации доступа к карте команд. Мьютекс нужен для безопасного
     * многопоточного доступа к статической карте команд.
     * 
     * @return Ссылка на мьютекс.
     */
    static std::shared_mutex& get_mutex() {
        static std::shared_mutex mutex;
        return mutex;
    }
};
