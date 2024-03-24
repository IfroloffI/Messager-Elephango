cmake_minimum_required(VERSION 3.27)
set(PROJECT_NAME Messager)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)

set(SOURCES main.cpp)
set(HEADERS)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
