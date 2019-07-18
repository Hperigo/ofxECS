# Notice name prefix of this variable, set by CMake according
# to value given with "project()" in the root CMakeLists.txt.

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set( ECS_LIBRARY_FILES "${CMAKE_CURRENT_LIST_DIR}/Entity.cpp"
                        "${CMAKE_CURRENT_LIST_DIR}/System.cpp"
                        "${CMAKE_CURRENT_LIST_DIR}/Manager.cpp"
                        )

include_directories(${EcsLibrary_SOURCE_DIR})
add_library(ecs ${ECS_LIBRARY_FILES})