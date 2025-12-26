include(FetchContent)

# Загружаем googletest
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2  # или release-1.15.2, используйте актуальную версию
)

# Для Windows: предотвращаем переопределение флагов компилятора родительского проекта
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Создаем тестовый executable
file(GLOB_RECURSE cista-gtest-files *.cc)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/../cista.h PROPERTIES GENERATED TRUE)


add_executable(cista-gtest-single-header ${cista-gtest-files} ${CMAKE_BINARY_DIR}/cista.h)
add_dependencies(cista-gtest-single-header generate-cista-single-header)

target_link_libraries(cista-gtest-single-header gtest_main)
target_include_directories(cista-gtest-single-header PRIVATE ${CMAKE_BINARY_DIR})
target_compile_options(cista-gtest-single-header PRIVATE ${cista-compile-flags})
target_compile_definitions(cista-gtest-single-header PRIVATE SINGLE_HEADER)
if (CISTA_ZERO_OUT)
    target_compile_definitions(cista-gtest-single-header PRIVATE CISTA_ZERO_OUT=1)
endif()

# Интеграция с CTest

include(GoogleTest)
gtest_discover_tests(cista-gtest-single-header)
#add_test(NAME cista-tests COMMAND cista-test-single-header)