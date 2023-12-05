cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)

include_directories(${RAYFLEX_EXTERNAL_INCLUDES} ${CMAKE_SOURCE_DIR}/include)
link_libraries(rayflex ${RAYFLEX_EXTERNAL_LINKS})

add_executable(phys3d_basic basic.cpp)
target_compile_definitions(phys3d_basic PRIVATE SUPPORT_PHYS_3D=1)

add_executable(phys3d_world world.cpp)
target_compile_definitions(phys3d_world PRIVATE SUPPORT_PHYS_3D=1 SUPPORT_GFX_3D=1)
