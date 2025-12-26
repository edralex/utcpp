# Подключение doctest
include(FetchContent)
FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.12
)
FetchContent_MakeAvailable(doctest)

# Включаем родительскую директорию
include_directories(../)

# Автоматически собираем все cpp файлы с тестами (исключая main)
file(GLOB tests "*.cpp")
list(REMOVE_ITEM tests "${CMAKE_CURRENT_SOURCE_DIR}/doctest_main.cpp")

# Создаем один исполняемый файл со всеми тестами
add_executable(all_doctest_tests doctest_main.cpp ${tests})
target_link_libraries(all_doctest_tests simpleson doctest::doctest)

if(MSVC)
    target_compile_definitions(all_doctest_tests PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

add_test(NAME doctest_all COMMAND all_doctest_tests)
