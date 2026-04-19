cmake_minimum_required(VERSION 3.20)
project(renderer C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)

find_package(SDL2 REQUIRED)

add_executable(renderer
    src/main.c
    src/display.c
    src/vector.c
    src/mesh.c
    src/triangle.c
)

target_include_directories(renderer PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(renderer PRIVATE ${SDL2_LIBRARIES})
