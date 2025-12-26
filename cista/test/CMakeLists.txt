add_subdirectory(../tools/doctest ${CMAKE_CURRENT_BINARY_DIR}/doctest)

file(GLOB_RECURSE cista-test-files *.cc)

add_executable(cista-test-single-header ${cista-test-files} ${CMAKE_CURRENT_BINARY_DIR}/cista.h)
add_dependencies(cista-test-single-header generate-cista-single-header)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/cista.h PROPERTIES GENERATED TRUE)
target_link_libraries(cista-test-single-header cista-doctest)
target_include_directories(cista-test-single-header PRIVATE ${CMAKE_BINARY_DIR})
target_compile_options(cista-test-single-header PRIVATE ${cista-compile-flags})
target_compile_definitions(cista-test-single-header PRIVATE SINGLE_HEADER)
if (CISTA_ZERO_OUT)
    target_compile_definitions(cista-test-single-header PRIVATE CISTA_ZERO_OUT=1)
endif()
add_test(NAME cista-tests COMMAND cista-test-single-header)

