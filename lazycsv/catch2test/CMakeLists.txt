include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.0
)
FetchContent_MakeAvailable(Catch2)

# Включаем родительскую директорию
include_directories(../)

# Автоматически собираем все cpp файлы
file(GLOB tests "*.cpp")

foreach(test ${tests})
    string(REGEX REPLACE ".*/" "" test_name "${test}")
    string(REGEX REPLACE ".cpp$" "" test_name "${test_name}")

    add_executable("${test_name}_test" ${test})
    target_link_libraries("${test_name}_test" lazycsv Catch2::Catch2WithMain)

    if(MSVC)
        target_compile_definitions("${test_name}_test" PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()

    # Копируем директорию inputs для каждого теста
    add_custom_command(
            TARGET "${test_name}_test"
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/inputs
            ${CMAKE_CURRENT_BINARY_DIR}/inputs
    )

    # Используем catch_discover_tests для автообнаружения тестов
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    include(CTest)
    include(Catch)
    catch_discover_tests("${test_name}_test")
endforeach()
