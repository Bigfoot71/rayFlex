cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)

include_directories(${RAYFLEX_EXTERNAL_INCLUDES} ${CMAKE_SOURCE_DIR}/include)
link_libraries(rayflex ${RAYFLEX_EXTERNAL_LINKS})

add_executable(gfx3d_lights lights.cpp)
target_compile_definitions(gfx3d_lights PRIVATE SUPPORT_GFX_3D=1)
