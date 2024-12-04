cmake_minimum_required(VERSION 3.12)

# Определяем общие параметры для всего проекта
project(MultimeterProject)

# Устанавливаем стандарт C++ для всего проекта
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Указываем путь к папке общих файлов
set(COMMON_PATH "${CMAKE_SOURCE_DIR}/_common")

# Добавляем путь к папке _common в список путей поиска заголовков для всего проекта
include_directories(${COMMON_PATH})

# Добавляем подпроект client
add_subdirectory(client)

# Добавляем подпроект QtClient
add_subdirectory(QtClient)

# Добавляем подпроект multimeter
add_subdirectory(server)