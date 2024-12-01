#pragma once

#include "channel.h"
#include "ranges.h"
#include "my_tools.h"

#include <string>
#include <memory>
#include <vector>

using TypeCmdParams = const std::vector<std::string>&;

/**
 * @class ICommand
 * @brief Абстрактный базовый класс для всех команд.
 *
 * Этот класс описывает общие методы для выполнения команд, которые могут взаимодействовать с каналом.
 */
class ICommand {
public:
    /**
     * @brief Конструктор.
     * @param channel Канал, с которым будет работать команда.
     */
    explicit ICommand(std::shared_ptr<IChannel> channel) : channel(channel) {}

    /// Деструктор
    virtual ~ICommand() = default;

    /**
     * @brief Выполняет команду.
     * 
     * Абстрактный метод, который должен быть реализован в подклассах для выполнения конкретной команды.
     * @return Строка с результатом выполнения команды.
     */
    virtual std::string execute() = 0;

    /**
     * @brief Получает ответ на выполнение команды.
     * 
     * Абстрактный метод, который должен быть реализован в подклассах для получения ответа.
     * @return Строка с результатом выполнения команды.
     */
    virtual std::string get_response() = 0;

protected:
    /// Канал, с которым работает команда
    std::shared_ptr<IChannel> channel;
};

/**
 * @class StartMeasureCommand
 * @brief Команда для начала измерений.
 *
 * Этот класс реализует команду, которая инициирует начало измерений на канале.
 */
class StartMeasureCommand : public ICommand {
public:
    /**
     * @brief Конструктор.
     * @param channel Канал, с которым будет работать команда.
     * @param params Параметры команды (не используются в данной команде).
     */
    StartMeasureCommand(std::shared_ptr<IChannel> channel, TypeCmdParams params)
        : ICommand(channel) {}

    /**
     * @brief Выполняет команду начала измерений.
     * 
     * Метод проверяет состояние канала, и если канал в состоянии Idle, начинает измерения.
     * @return Строка с результатом выполнения команды ("ok" или "fail").
     */
    std::string execute() override {   
        ChannelStateManager::ChannelState state = channel->get_state();
        if (state == ChannelStateManager::ChannelState::Idle) {
            channel->start();
            return get_response();
        }
        return "fail, " + ChannelStateManager::to_string(state);
    }

    /**
     * @brief Получает ответ на выполнение команды.
     * @return Строка с результатом выполнения команды ("ok" или "fail").
     */
    std::string get_response() override {        
        return channel->get_state() == ChannelStateManager::ChannelState::Measure ? "ok" : "fail";
    }
};

/**
 * @class SetRangeCommand
 * @brief Команда для установки диапазона.
 *
 * Этот класс реализует команду для установки диапазона на канале.
 */
class SetRangeCommand : public ICommand {
private:
    int new_range; ///< Новый диапазон

public:
    /**
     * @brief Конструктор.
     * @param channel Канал, с которым будет работать команда.
     * @param params Параметры команды, где первый элемент - новый диапазон.
     */
    SetRangeCommand(std::shared_ptr<IChannel> channel, TypeCmdParams params)
        : ICommand(channel), new_range(std::stoi(params[1])) {}

    /**
     * @brief Выполняет команду установки диапазона.
     * 
     * Метод проверяет состояние канала и устанавливает новый диапазон, если канал в состоянии Idle или Measure.
     * @return Строка с результатом выполнения команды ("ok" или "fail").
     */
    std::string execute() override {     
        ChannelStateManager::ChannelState state = channel->get_state();
        if (state == ChannelStateManager::ChannelState::Idle || state == ChannelStateManager::ChannelState::Measure) {  
            channel->set_range(new_range);
            return get_response();
        }        
        return "fail, " + ChannelStateManager::to_string(state);
    }

    /**
     * @brief Получает ответ на выполнение команды.
     * 
     * Проверяет, совпадает ли установленный диапазон с заданным.
     * @return Строка с результатом выполнения команды ("ok" или "fail").
     */
    std::string get_response() override {        
        auto current_range = channel->get_range();
        std::string ok_fail = (new_range == current_range) ? "ok" : "fail";
        return ok_fail + ", " + std::to_string(new_range);
    }
};

/**
 * @class StopMeasureCommand
 * @brief Команда для остановки измерений.
 *
 * Этот класс реализует команду, которая останавливает измерения на канале.
 */
class StopMeasureCommand : public ICommand {
public:
    /**
     * @brief Конструктор.
     * @param channel Канал, с которым будет работать команда.
     * @param params Параметры команды (не используются в данной команде).
     */
    StopMeasureCommand(std::shared_ptr<IChannel> channel, TypeCmdParams params)
        : ICommand(channel) {}

    /**
     * @brief Выполняет команду остановки измерений.
     * 
     * Метод проверяет состояние канала и останавливает измерения, если канал в состоянии Measure.
     * @return Строка с результатом выполнения команды ("ok" или "fail").
     */
    std::string execute() override {  
        ChannelStateManager::ChannelState state = channel->get_state();  
        if (state == ChannelStateManager::ChannelState::Measure) {              
            channel->stop();          
            return get_response();
        }        
        return "fail, " + ChannelStateManager::to_string(state);
    }

    /**
     * @brief Получает ответ на выполнение команды.
     * 
     * Проверяет, находится ли канал в состоянии Idle после остановки измерений.
     * @return Строка с результатом выполнения команды ("ok" или "fail").
     */
    std::string get_response() override {        
        return channel->get_state() == ChannelStateManager::ChannelState::Idle ? "ok" : "fail";
    }
};

/**
 * @class GetStatusCommand
 * @brief Команда для получения статуса канала.
 *
 * Этот класс реализует команду, которая запрашивает текущий статус канала.
 */
class GetStatusCommand : public ICommand {
private:
    ChannelStateManager::ChannelState state; ///< Текущее состояние канала

public:
    /**
     * @brief Конструктор.
     * @param channel Канал, с которым будет работать команда.
     * @param params Параметры команды (не используются в данной команде).
     */
    GetStatusCommand(std::shared_ptr<IChannel> channel, TypeCmdParams params)
        : ICommand(channel) {}

    /**
     * @brief Выполняет команду получения статуса канала.
     * @return Строка с результатом выполнения команды.
     */
    std::string execute() override {                
        state = channel->get_state();
        return get_response();      
    }

    /**
     * @brief Получает ответ на выполнение команды.
     * 
     * Возвращает статус канала ("ok" или "fail"), а также текущее состояние.
     * @return Строка с результатом выполнения команды.
     */
    std::string get_response() override {        
        std::string ok_fail = "fail";
        if (state == ChannelStateManager::ChannelState::Idle || state == ChannelStateManager::ChannelState::Measure) {
            ok_fail = "ok";
        }
        return ok_fail + ", " + ChannelStateManager::to_string(state);
    }
};

/**
 * @class GetResultCommand
 * @brief Команда для получения результата измерений.
 *
 * Этот класс реализует команду, которая запрашивает результат измерений с канала.
 */
class GetResultCommand : public ICommand {
private:
    float value = 0.0f; ///< Значение измерения
    int range; ///< Диапазон
    ChannelStateManager::ChannelState state; ///< Текущее состояние канала

public:
    /**
     * @brief Конструктор.
     * @param channel Канал, с которым будет работать команда.
     * @param params Параметры команды (не используются в данной команде).
     */
    GetResultCommand(std::shared_ptr<IChannel> channel, TypeCmdParams params)
        : ICommand(channel) {
        range = channel->get_range();
    }

    /**
     * @brief Выполняет команду получения результата измерений.
     * 
     * Метод проверяет состояние канала и, если он находится в состоянии Measure, возвращает значение измерений.
     * @return Строка с результатом выполнения команды.
     */
    std::string execute() override {           
        state = channel->get_state();     
        if (state == ChannelStateManager::ChannelState::Measure) {
            value = channel->get_measuring_value();      
            return get_response();
        }              
        return "fail, " + ChannelStateManager::to_string(state);
    }

    /**
     * @brief Получает ответ на выполнение команды.
     * 
     * Возвращает результат измерений с точностью, соответствующей диапазону.
     * @return Строка с результатом выполнения команды.
     */
    std::string get_response() override {                
        std::string ok_fail = "fail, ";
        if (state == ChannelStateManager::ChannelState::Measure) {
            ok_fail = "ok, " + MyTools::float_to_string(value, RangeManager::get_range(range).precision);
        }
        else {
            ok_fail += ChannelStateManager::to_string(state);
        }
        return ok_fail;
    }
};

/**
 * @class SetFrequencyCommand
 * @brief Команда для установки частоты.
 *
 * Этот класс реализует команду для установки частоты на канале.
 */
class SetFrequencyCommand : public ICommand {
private:
    int new_frequency; ///< Новая частота

public:
    /**
     * @brief Конструктор.
     * @param channel Канал, с которым будет работать команда.
     * @param params Параметры команды, где первый элемент - новая частота.
     */
    SetFrequencyCommand(std::shared_ptr<IChannel> channel, TypeCmdParams params)
        : ICommand(channel), new_frequency(std::stoi(params[1])) {}

    /**
     * @brief Выполняет команду установки частоты.
     * 
     * Метод проверяет состояние канала и устанавливает новую частоту, если канал в состоянии Idle или Measure.
     * @return Строка с результатом выполнения команды ("ok" или "fail").
     */
    std::string execute() override {      
        ChannelStateManager::ChannelState state = channel->get_state();
        if (state == ChannelStateManager::ChannelState::Idle || state == ChannelStateManager::ChannelState::Measure) {   
            channel->set_frequency(new_frequency);
            return get_response();
        }        
        return "fail, " + ChannelStateManager::to_string(state);
    }

    /**
     * @brief Получает ответ на выполнение команды.
     * 
     * Проверяет, совпадает ли установленная частота с заданной.
     * @return Строка с результатом выполнения команды ("ok" или "fail").
     */
    std::string get_response() override {        
        auto current_frequency = channel->get_frequency();
        std::string ok_fail = (new_frequency == current_frequency) ? "ok" : "fail";
        return ok_fail + ", " + std::to_string(new_frequency);
    }
};
