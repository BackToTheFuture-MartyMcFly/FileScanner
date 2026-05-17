cmake_minimum_required(VERSION 3.25)

project(FileScanner)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE_FILES FileScanner.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})