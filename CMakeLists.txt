cmake_minimum_required(VERSION 3.20)
project(OpenGLTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

add_executable(from-pixels-to-blocks
    src/main.cpp
)

target_link_libraries(from-pixels-to-blocks
    PRIVATE
    glfw
    glad::glad
)
