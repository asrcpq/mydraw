cmake_minimum_required(VERSION 3.1.0)

project(mydraw LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(mydraw
	src/main.cpp
	src/mainapp.cpp
	src/undoque.cpp
	src/colorsel.cpp
	src/resource_manager.cpp
)

target_link_libraries(mydraw Qt5::Widgets)

install(TARGETS mydraw DESTINATION bin)
