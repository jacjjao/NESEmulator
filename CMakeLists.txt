cmake_minimum_required(VERSION 3.15)
project(NESEmulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES
    src/*.hpp
	src/*.cpp 
)

add_executable(${PROJECT_NAME} ${SOURCES})

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