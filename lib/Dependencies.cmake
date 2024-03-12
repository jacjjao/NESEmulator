include(FetchContent)

FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1
)

FetchContent_MakeAvailable(sfml)

set(BUILD_SHARED_LIBS ON)

set(SFML_BUILD_EXAMPLES OFF)
set(SFML_BUILD_DOC ON)

set(SFML_BUILD_NETWORK OFF)

set(SFML_BUILD_SYSTEM ON)
set(SFML_BUILD_AUDIO ON)
set(SFML_BUILD_GRAPHICS ON)
set(SFML_BUILD_WINDOW ON)

FetchContent_GetProperties(sfml)
if(NOT sfml_POPULATED)
    FetchContent_Populate(sfml)
    add_subdirectory(${sfml_SOURCE_DIR} ${sfml_BINARY_DIR})
endif()