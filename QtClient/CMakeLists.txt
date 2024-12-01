cmake_minimum_required(VERSION 3.12)

project(QtClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Quick Qml)

# Исходные файлы
set(SOURCES
    src/main.cpp
    src/qt_client.cpp
)

# Файлы заголовков
set(HEADERS
    src/qt_client.h
)

# Файл ресурсов
set(RESOURCES
    resources.qrc
)

# Добавление ресурсов в проект
qt5_add_resources(QT_RESOURCES ${RESOURCES})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${QT_RESOURCES})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Quick Qt5::Qml)
