cmake_minimum_required (VERSION 3.1.0)

project (LightControlGui)

file(GLOB SourceFiles
    "*.h"
    "*.cpp"
    "*.ui"
)

file(GLOB WidgetSourceFiles
    "Widgets/*.h"
    "Widgets/*.cpp"
    "Widgets/*.ui"
)

add_executable(LightControlGui ${SourceFiles} ${WidgetSourceFiles})
target_link_libraries(LightControlGui Qt5::Core)
target_link_libraries(LightControlGui Qt5::Network)
target_link_libraries(LightControlGui Qt5::WebSockets)
target_link_libraries(LightControlGui Qt5::Widgets)
target_link_libraries(LightControlGui lightcontrol)
target_include_directories(LightControlGui PUBLIC ${CMAKE_SOURCE_DIR}/liblightcontrol)