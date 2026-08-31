cmake_minimum_required(VERSION 3.16)
project(npc_life_sim CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_library(sim_core STATIC
    src/GameTime.cpp
    src/Needs.cpp
    src/Npc.cpp
    src/Personality.cpp
    src/Relationship.cpp
    src/Schedule.cpp
    src/Simulation.cpp
    src/Types.cpp
    src/World.cpp
)
target_include_directories(sim_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(sim_core PRIVATE -Wall -Wextra)

add_executable(npc_life_sim src/main.cpp)
target_link_libraries(npc_life_sim PRIVATE sim_core)
target_compile_options(npc_life_sim PRIVATE -Wall -Wextra)

add_executable(npc_life_sim_interactive src/interactive_main.cpp)
target_link_libraries(npc_life_sim_interactive PRIVATE sim_core)
target_compile_options(npc_life_sim_interactive PRIVATE -Wall -Wextra)
