cmake_minimum_required (VERSION 3.1.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Widgets Network WebSockets REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(liblightcontrol)
add_subdirectory(light_control)
add_subdirectory(LightControlGui)
