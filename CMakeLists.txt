cmake_minimum_required(VERSION 3.15)
project(NESEmulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES
	src/*.cpp 
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_precompile_headers(${PROJECT_NAME}
	PRIVATE
		src/pch.hpp
)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)

target_include_directories(${PROJECT_NAME} 
	PRIVATE 
		sfml-system 
		sfml-network 
		sfml-graphics 
		sfml-window
)

target_link_libraries(${PROJECT_NAME} 
	PRIVATE 
		sfml-system 
		sfml-network 
		sfml-graphics 
		sfml-window
)