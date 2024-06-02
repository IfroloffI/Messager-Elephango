cmake_minimum_required(VERSION 3.27)
set(PROJECT_NAME Messager)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

set(SFML_DIR "C:\\SFML/lib/cmake/SFML")

find_package(SFML 2.5.1 COMPONENTS network graphics window system REQUIRED)

set(SOURCES
        server/server.cpp)

set(HEADERS
        server/server.h)

add_executable(${PROJECT_NAME} main.cpp ${HEADERS} ${SOURCES})

include_directories("C:\\SFML/include")
target_link_libraries(${PROJECT_NAME} sfml-network sfml-graphics sfml-window sfml-system)
