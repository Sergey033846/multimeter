cmake_minimum_required(VERSION 3.12)

project(Multimeter)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Выбираем флаги для отладки и релиза в зависимости от типа сборки
set(CMAKE_BUILD_TYPE "Default")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in Debug mode")    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DDEBUG")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Building in Release mode")    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
else()
    message(STATUS "Building in default mode")
endif()

# Указываем путь к папке _common
set(COMMON_PATH "./../_common")

# Исходные файлы
set(SRC_FILES
    main.cpp       
    ranges.cpp
    channel.cpp
    analog_input.cpp    
    channel_factory.h
    channel_controller.cpp
    commands.h
    command_factory.h
    task_pool.cpp
    multimeter.cpp    
    ${COMMON_PATH}/config.h
    ${COMMON_PATH}/logger.cpp
    ${COMMON_PATH}/my_tools.cpp
)

# Создаем исполняемый файл проекта
add_executable(multimeter ${SRC_FILES})

# Добавляем путь к папке _common в список путей поиска заголовков
target_include_directories(multimeter PUBLIC ${COMMON_PATH})
