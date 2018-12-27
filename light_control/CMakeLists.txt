cmake_minimum_required (VERSION 3.1.0)

project (light_control)

file(GLOB SourceFiles
    "*.h"
    "*.cpp"
)

add_executable(light_control ${SourceFiles})
target_link_libraries(light_control Qt5::Core)
target_link_libraries(light_control Qt5::Network)
target_link_libraries(light_control Qt5::WebSockets)
target_link_libraries(light_control lightcontrol)
target_include_directories(light_control PUBLIC ${CMAKE_SOURCE_DIR}/liblightcontrol)

install(TARGETS light_control DESTINATION bin)
