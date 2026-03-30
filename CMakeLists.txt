cmake_minimum_required(VERSION 3.16)

project(algochecker VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(algochecker
    src/algochecker/analyzer.cpp
    src/static_analyzer/file_loader.cpp
    src/static_analyzer/lexer.cpp
    src/static_analyzer/parser.cpp
    src/static_analyzer/complexity_evaluator.cpp
)

target_include_directories(algochecker PUBLIC include)

add_library(algochecker_web
    src/web_server/server.cpp
)

target_include_directories(algochecker_web PUBLIC include)
target_link_libraries(algochecker_web PUBLIC algochecker)

enable_testing()

add_executable(algochecker_tests
    tests/analyzer_api_test.cpp
)

target_include_directories(algochecker_tests PRIVATE include)
target_link_libraries(algochecker_tests PRIVATE algochecker)

add_test(NAME algochecker_tests COMMAND algochecker_tests)
