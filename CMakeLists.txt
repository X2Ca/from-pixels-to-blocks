cmake_minimum_required(VERSION 3.20)
project(OpenGLTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(OpenCV REQUIRED)


add_executable(from-pixels-to-blocks
    src/main.cpp
    src/Function.cpp
    src/ImageProcessing.cpp
)

target_link_libraries(from-pixels-to-blocks
    PRIVATE
    glfw
    glad::glad
    ${OpenCV_LIBS}
)
file(GLOB SHADERS "shaders/*.glsl")

target_include_directories(from-pixels-to-blocks PRIVATE include)

foreach(shader ${SHADERS})
    configure_file(${shader} ${CMAKE_BINARY_DIR}/shaders COPYONLY)
endforeach()