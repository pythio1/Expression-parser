cmake_minimum_required(VERSION 3.14)
project(expr_parser CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

add_library(expr_core
    src/lexer.cpp
    src/parser.cpp
    src/interpreter.cpp
)
target_include_directories(expr_core PUBLIC include)

add_executable(expr_cli src/main.cpp)
target_link_libraries(expr_cli PRIVATE expr_core)

enable_testing()
add_executable(expr_tests tests/test_parser.cpp)
target_link_libraries(expr_tests PRIVATE expr_core)
add_test(NAME expr_tests COMMAND expr_tests)
