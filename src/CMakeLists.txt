cmake_minimum_required(VERSION 3.27)
set(PROJECT_NAME Messager)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
        server/server.cpp)

set(HEADERS
        server/server.h)

add_executable(${PROJECT_NAME} main.cpp ${HEADERS} ${SOURCES})

include_directories (headers/usr/include С:/SFML/include)
set(CMAKE_PREFIX_PATH C:/SFML)
set(SFML_STATIC_LIBRARIES TRUE)

# include_directories("{path_to_PostgreSQL}/include")

find_package(SFML 2.6.1 COMPONENTS network audio graphics window system)

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio sfml-network sfml-window sfml-system)

# target_link_libraries(${PROJECT_NAME} "{path_to_postgreSQL}/lib/libpq.lib")
