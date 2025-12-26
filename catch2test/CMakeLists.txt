# Подключение Catch2
include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.0
)
FetchContent_MakeAvailable(Catch2)

# Добавляем модули Catch2 в путь для catch_discover_tests
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

# Включаем родительскую директорию
include_directories(../)

# Автоматически собираем все cpp файлы
file(GLOB tests "*.cpp")

# Автоматически находим все cpp файлы
file(GLOB tests "*.cpp")

add_executable(catch2tests ${tests})

target_link_libraries(catch2tests simpleson Catch2::Catch2WithMain)

if(MSVC)
    target_compile_definitions(catch2tests PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()


catch_discover_tests(catch2tests)

