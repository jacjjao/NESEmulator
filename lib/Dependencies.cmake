include(FetchContent)

FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1
)

FetchContent_MakeAvailable(sfml)

set(BUILD_SHARED_LIBS TRUE)

set(SFML_BUILD_EXAMPLES FALSE)
set(SFML_BUILD_DOC TRUE)

set(SFML_BUILD_NETWORK FALSE)

set(SFML_BUILD_SYSTEM TRUE)
set(SFML_BUILD_AUDIO TRUE)
set(SFML_BUILD_GRAPHICS TRUE)
set(SFML_BUILD_WINDOW TRUE)

FetchContent_GetProperties(sfml)
if(NOT sfml_POPULATED)
    FetchContent_Populate(sfml)
    add_subdirectory(${sfml_SOURCE_DIR} ${sfml_BINARY_DIR})
endif()