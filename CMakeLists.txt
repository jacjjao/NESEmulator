cmake_minimum_required(VERSION 3.15)
project(NESEmulator)

add_executable(${PROJECT_NAME} src/main.cpp "src/CPU6502.cpp" )

if(MSVC)
	target_compile_options(${PROJECT_NAME} 
		PRIVATE 
			"/analyze"
			"/std:c++20"
			"/W4"	
			"/MP"
	)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_compile_options(${PROJECT_NAME} 
			PRIVATE 
				"/fsanitize=address"
				"/MDd"
				"/Od"
		)
		message("Debug build")
	elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
		target_compile_options(${PROJECT_NAME} 
			PRIVATE 
				"/MD"
				"/O2"
		)
		message("Release build")
	else() 
		message("Yet to configure build type")
	endif()
else()
	message("Gotta update the CMakeList.txt to support this compiler")
endif()

find_package(SDL2 REQUIRED)

target_compile_definitions(${PROJECT_NAME} 
	PRIVATE 
		SDL_MAIN_HANDLED
)

target_include_directories(${PROJECT_NAME}
	PRIVATE 
		SDL2::SDL2
)

target_link_libraries(${PROJECT_NAME} 
	PRIVATE 
		SDL2::SDL2
)