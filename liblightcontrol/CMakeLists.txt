cmake_minimum_required (VERSION 3.1.0)

project (lightcontrol)

file(GLOB SourceFiles
    "*.h"
    "*.cpp"
)

file(GLOB NodeSourceFiles
    "Nodes/*.h"
    "Nodes/*.cpp"
)

add_library(lightcontrol ${SourceFiles} ${NodeSourceFiles})
target_link_libraries(lightcontrol Qt5::Core)
target_link_libraries(lightcontrol Qt5::Network)
target_link_libraries(lightcontrol Qt5::WebSockets)