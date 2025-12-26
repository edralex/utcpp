cmake_minimum_required(VERSION 3.8)
project(cista LANGUAGES C CXX VERSION 0.7)

# Компиляторные флаги
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  set(cista-compile-flags
          -Wno-unknown-warning-option
          -Wno-global-constructors
          -Wno-exit-time-destructors
          -fno-strict-aliasing
          -Weverything
          -Wno-c++98-compat
          -Wno-c++98-compat-pedantic
          -Werror
  )
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(cista-compile-flags -Wall -Wextra)
endif()

# Опции
option(CISTA_ZERO_OUT "zero out fresh memory for valgrind" OFF)

# Генерация single header
add_subdirectory(tools/uniter EXCLUDE_FROM_ALL)
file(GLOB_RECURSE cista-include-files include/*.h*)
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cista.h
        COMMAND uniter
        ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cista/serialization.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cista/reflection/comparable.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cista/reflection/printable.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cista/reflection/member_index.h
        > ${CMAKE_CURRENT_BINARY_DIR}/cista.h
        DEPENDS ${cista-include-files}
)
add_custom_target(generate-cista-single-header ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cista.h
)
enable_testing()
#add_subdirectory(test)
add_subdirectory(googletest)
#add_subdirectory(catch2test)