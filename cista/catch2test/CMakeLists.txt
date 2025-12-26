# Загружаем Catch2 через FetchContent
include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.0
)
FetchContent_MakeAvailable(Catch2)

# Ищем все тестовые файлы с расширением .cc (или .cpp, если используете его)
file(GLOB_RECURSE cista_catch2_test_sources *.cc)

# Создаём исполняемый файл для тестов
add_executable(cista-test-catch2 ${cista_catch2_test_sources})

# Добавляем зависимость от генерации single-header файла
add_dependencies(cista-test-catch2 generate-cista-single-header)

# Говорим CMake, что cista.h — сгенерированный файл
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/../cista.h PROPERTIES GENERATED TRUE)

# Подключаем сгенерированный single-header файл как include
target_include_directories(cista-test-catch2 PRIVATE ${CMAKE_BINARY_DIR})

# Подключаем Catch2
target_link_libraries(cista-test-catch2 PRIVATE Catch2::Catch2WithMain)

# Устанавливаем флаги компиляции
if(DEFINED cista-compile-flags)
    target_compile_options(cista-test-catch2 PRIVATE ${cista-compile-flags})
endif()

# Передаём опции препроцессора, если есть
if(CISTA_ZERO_OUT)
    target_compile_definitions(cista-test-catch2 PRIVATE CISTA_ZERO_OUT=1)
endif()

# Определяем макрос для single-header режима
target_compile_definitions(cista-test-catch2 PRIVATE SINGLE_HEADER)

add_test(NAME cista-tests COMMAND cista-test-catch2 )