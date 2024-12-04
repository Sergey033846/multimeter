cmake_minimum_required(VERSION 3.12)

project(Client)

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
    client.cpp
)

# Создаем исполняемый файл проекта
add_executable(client ${SRC_FILES})

# Добавляем путь к папке _common в список путей поиска заголовков
target_include_directories(client PUBLIC ${COMMON_PATH})
