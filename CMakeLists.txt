cmake_minimum_required(VERSION 3.0)
project(rayflex)

set(CMAKE_CXX_STANDARD 17)

# Retrieving the root directory of 'rayflex'
set(RAYFLEX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# Detects if 'rayflex' is the main project
if("${CMAKE_SOURCE_DIR}" STREQUAL "${RAYFLEX_ROOT_PATH}")
    set(RAYFLEX_IS_MAIN TRUE)
endif()

# Setting the directory for CMake configuration modules
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Also include 'raylib/cmake/LibraryConfigurations' here
# to obtain GRAPHICS and PLATFORM_CPP definitions
include(external/raylib/cmake/LibraryConfigurations.cmake)
if (NOT PLATFORM_CPP)
    set(PLATFORM_CPP "PLATFORM_DESKTOP")
endif()

# Configuration for compiling with emscripten (wasm)
if("${PLATFORM_CPP}" STREQUAL "PLATFORM_WEB")
    include(ConfigEmscripten)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s WASM=1 -s ASYNCIFY=1 -s USE_GLFW=3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1 -s ASYNCIFY=1 -s USE_GLFW=3")
endif()

# Include 'rayflex' configuration options
include(CMakeOptions.txt)

# Include 'rayflex' sources
include(source/CMakeLists.txt)

# Configure external dependencies
include(ConfigExternal)

# Configure the 'rayflex' library
add_library(${PROJECT_NAME} ${RAYFLEX_SOURCE})

# Specify include paths for 'rayflex'
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${RAYFLEX_EXTERNAL_INCLUDES}
)

# Specify libraries to link with 'rayflex'
target_link_libraries(${PROJECT_NAME} PUBLIC
    ${RAYFLEX_EXTERNAL_LINKS}
)

# Definitions of macros for 'rayflex'
target_compile_definitions(${PROJECT_NAME} PUBLIC
    SUPPORT_GFX_2D=$<BOOL:${SUPPORT_GFX_2D}>
    SUPPORT_GFX_3D=$<BOOL:${SUPPORT_GFX_3D}>
    SUPPORT_PHYS_2D=$<BOOL:${SUPPORT_PHYS_2D}>
    SUPPORT_PHYS_3D=$<BOOL:${SUPPORT_PHYS_3D}>
    SUPPORT_NET=$<BOOL:${SUPPORT_NET}>
    "${PLATFORM_CPP}"
    "${GRAPHICS}"
)

# Configure examples
if(RAYFLEX_BUILD_EXAMPLES)
    add_subdirectory(examples/core)
    if(SUPPORT_GFX_2D)
        add_subdirectory(examples/gfx2d)
        if(SUPPORT_PHYS_2D)
            add_subdirectory(examples/phys2d)
        endif()
    endif()
    if(SUPPORT_GFX_3D)
        add_subdirectory(examples/gfx3d)
        if(SUPPORT_PHYS_3D)
            add_subdirectory(examples/phys3d)
        endif()
    endif()
    if(SUPPORT_NET)
        add_subdirectory(examples/net)
    endif()
endif()
