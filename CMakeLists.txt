cmake_minimum_required(VERSION 3.16)

project(algochecker VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(algochecker
    src/main.cpp
    src/static_analyzer/file_loader.cpp
    src/static_analyzer/lexer.cpp
    src/static_analyzer/parser.cpp
    src/static_analyzer/complexity_evaluator.cpp
    src/dynamic_analyzer/data_generator.cpp
    src/dynamic_analyzer/benchmarker.cpp
    src/dynamic_analyzer/exporter.cpp
    src/dynamic_analyzer/visualizer.cpp
    src/web_server/server.cpp
)

target_include_directories(algochecker PRIVATE include)
