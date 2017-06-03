project(server)

cmake_minimum_required(VERSION 2.8)

file(GLOB_RECURSE CPP_FILES source/*.cpp)
file(GLOB_RECURSE H_FILES include/*.h)

include_directories(include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -g -ggdb -Werror")

add_executable(${PROJECT_NAME}_run ${CPP_FILES} ${H_FILES})

