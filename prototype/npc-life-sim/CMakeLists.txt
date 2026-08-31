cmake_minimum_required(VERSION 3.16)
project(npc_life_sim CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB SIM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(npc_life_sim ${SIM_SOURCES})
target_include_directories(npc_life_sim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(npc_life_sim PRIVATE -Wall -Wextra)
