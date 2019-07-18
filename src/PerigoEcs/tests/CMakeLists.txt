set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${EcsLibrary_SOURCE_DIR}/tests/catch2)

include_directories(${EcsLibrary_SOURCE_DIR})
link_directories(${EcsLibrary_SOURCE_DIR})

add_executable(tests main.cpp manager_tests.cpp)
target_link_libraries(tests ecs)