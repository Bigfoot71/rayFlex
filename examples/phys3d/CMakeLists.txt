cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)

include_directories(${RAYFLEX_EXTERNAL_INCLUDES} ${CMAKE_SOURCE_DIR}/include)
link_libraries(rayflex ${RAYFLEX_EXTERNAL_LINKS})

add_executable(phys3d_basic basic.cpp)
add_executable(phys3d_world world.cpp)
