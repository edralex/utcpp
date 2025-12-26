include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)


include_directories(../)


file(GLOB tests "*.cpp")

add_executable(googletests ${tests})

target_link_libraries(googletests simpleson GTest::gtest_main)

if(MSVC)
    target_compile_definitions(googletests PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()


gtest_discover_tests(googletests)
