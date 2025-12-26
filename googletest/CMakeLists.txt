include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

# Включаем родительскую директорию
include_directories(../)

# Автоматически собираем все cpp файлы
file(GLOB tests "*.cpp")

foreach(test ${tests})
    string(REGEX REPLACE ".*/" "" test_name "${test}")
    string(REGEX REPLACE ".cpp$" "" test_name "${test_name}")

    add_executable("${test_name}_test" ${test})
    target_link_libraries("${test_name}_test" lazycsv GTest::gtest_main)

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

    # Используем gtest_discover_tests вместо add_test для автообнаружения тестов
    gtest_discover_tests("${test_name}_test")
endforeach()
