cmake_minimum_required(VERSION 3.10)
project(ShprotaProject CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")

# 1. Настройка путей к вашему Boost
set(BOOST_ROOT "/home/kotus/boost_1_80_0")
set(Boost_NO_SYSTEM_PATHS ON) # Игнорировать системный Boost (/usr/include)

# Поиск необходимых модулей Boost
find_package(Boost REQUIRED COMPONENTS system filesystem thread)

# Добавляем пути к заголовкам для всего проекта
include_directories(${BOOST_ROOT})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}) # Чтобы видел config.h и т.д.

# 2. Создаем библиотеки из ваших файлов

# Библиотека LOG
add_library(spf_log STATIC log/log.cpp)

# Библиотека SQL
add_library(sq_class STATIC sql/SqClass.cpp)

# Библиотеки READ/WRITE (shprota)
add_library(spf_reader STATIC shprota/ReadFromBuff.cpp)
add_library(spf_writer STATIC shprota/WriteToBuff.cpp)

# Библиотека CONFIG + Реализация Boost.JSON
# Мы объединяем их, как вы делали в 'ar rcs libconfig.a libconfig.o boost_json_impl.o'
add_library(spf_config STATIC config.cpp boost_json_impl.cpp)

# Библиотека LAYERS
add_library(layers_helper STATIC layers/layersHelper.cpp)

# 3. Создаем исполняемый файл
add_executable(result main.cpp)

# 4. Линковка (CMake сам определит правильный порядок)
target_link_libraries(result
    PRIVATE
    layers_helper
    spf_config
    spf_reader
    spf_writer
    spf_log
    sq_class
    Boost::system
    Boost::filesystem
    Boost::thread
    sqlite3
    pthread
)

# 5. Настройка вывода (как ваш cp ./obj/result ./result)
set_target_properties(result PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")