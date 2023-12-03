# Definition of external inclusions required for 'rayflex'
set(RAYFLEX_EXTERNAL_INCLUDES
    ${RAYFLEX_ROOT_PATH}/external/raylib-cpp/include
    ${RAYFLEX_ROOT_PATH}/external/raylib/src
)

# Definition of external libraries required for 'rayflex'
set(RAYFLEX_EXTERNAL_LINKS "")

# Configuration of raylib
add_subdirectory(${RAYFLEX_ROOT_PATH}/external/raylib)
list(APPEND RAYFLEX_SOURCE $<TARGET_OBJECTS:raylib>)
if (PLATFORM_CPP MATCHES PLATFORM_DESKTOP)
    list(APPEND RAYFLEX_SOURCE $<TARGET_OBJECTS:glfw>)
endif()

# Configuration of external libraries required for 2D physics support
if(SUPPORT_PHYS_2D)
    set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "")
    set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "")
    set(BOX2D_BUILD_DOCS OFF CACHE BOOL "")
    add_subdirectory(${RAYFLEX_ROOT_PATH}/external/box2d)
    list(APPEND RAYFLEX_EXTERNAL_LINKS box2d)
    list(APPEND RAYFLEX_EXTERNAL_INCLUDES
        ${RAYFLEX_ROOT_PATH}/external/box2d/include
    )
endif()

# Configuration of external libraries required for 3D physics support
if(SUPPORT_PHYS_3D)
    set(USE_GRAPHICAL_BENCHMARK OFF CACHE BOOL "")
    set(ENABLE_VHACD OFF CACHE BOOL "")
    set(BUILD_CPU_DEMOS OFF CACHE BOOL "")
    set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "")
    set(BUILD_EXTRAS OFF CACHE BOOL "")
    set(INSTALL_LIBS OFF CACHE BOOL "")
    set(BUILD_UNIT_TESTS OFF CACHE BOOL "")
    set(INSTALL_CMAKE_FILES OFF CACHE BOOL "")
    add_subdirectory(${RAYFLEX_ROOT_PATH}/external/bullet3)
    list(APPEND RAYFLEX_EXTERNAL_LINKS
        BulletDynamics
        BulletCollision
        LinearMath
    )
    list(APPEND RAYFLEX_EXTERNAL_INCLUDES
        ${RAYFLEX_ROOT_PATH}/external/bullet3/src
    )
endif()

# Configuration of external libraries required for network support
if(SUPPORT_NET)
    add_subdirectory(external/asio)
    set(SODIUM_DISABLE_TESTS ON CACHE BOOL "")
    add_subdirectory(external/libsodium)
    list(APPEND RAYFLEX_EXTERNAL_LINKS asio sodium)
    list(APPEND RAYFLEX_EXTERNAL_INCLUDES
        ${RAYFLEX_ROOT_PATH}/external/asio/asio/include
        ${RAYFLEX_ROOT_PATH}/external/libsodium/src/libsodium/include
    )
endif()
