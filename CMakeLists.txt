cmake_minimum_required (VERSION 3.14)
project(simpleson VERSION 1.1.0)
add_library(${PROJECT_NAME}
    json.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC .)
if(MSVC)
    # ignore warnings about scanf
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/W4 /WX)
elseif(BORLAND)
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()

enable_testing()
#add_subdirectory (test)
#add_subdirectory (googletest)
#add_subdirectory (catch2test)
add_subdirectory (doctest)

