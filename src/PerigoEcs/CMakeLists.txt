cmake_minimum_required(VERSION 3.12)

project(EcsLibrary)

set(EcsLibrary_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ecs")

add_subdirectory(ecs)
add_subdirectory(tests)