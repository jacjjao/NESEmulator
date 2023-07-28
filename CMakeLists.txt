cmake_minimum_required(VERSION 3.15)
project(NESEmulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest CONFIG REQUIRED)
if (WIN32)
	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
enable_testing()

file(GLOB_RECURSE SOURCES
    src/*.hpp
	src/*.cpp 
)

file(GLOB TEST_SOURCES
	tests/*.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
add_executable(the_tests ${TEST_SOURCES})

if(MSVC)
	target_compile_options(${PROJECT_NAME} 
		PRIVATE 
			"/analyze"
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
	elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
		target_compile_options(${PROJECT_NAME} 
			PRIVATE 
				"/MD"
				"/O2"
		)
	else() 
		message("Yet to configure build type")
	endif()
else()
	message("Gotta update the CMakeList.txt to support this compiler")
endif()

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

target_link_libraries(the_tests 
	PRIVATE 
		GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(the_tests)