cmake_minimum_required (VERSION 3.1.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Widgets Network WebSockets REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(liblightcontrol)
add_subdirectory(light_control)
add_subdirectory(LightControlGui)

if(WIN32)
	# get reference to deployqt
	get_target_property(uic_location Qt5::uic IMPORTED_LOCATION)
	get_filename_component( _dir ${uic_location} DIRECTORY)
	set(deployqt "${_dir}/windeployqt.exe")
	if(NOT EXISTS ${deployqt})
	  message(FATAL_ERROR "Failed to locate deployqt executable: [${deployqt}]")
	endif()

	install(CODE "set(deployqt \"${deployqt}\")")
	install(CODE [===[
	execute_process(COMMAND "${deployqt}" "${CMAKE_INSTALL_PREFIX}/bin")
	]===])
endif(WIN32)
