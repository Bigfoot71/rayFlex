cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)

include_directories(${RAYFLEX_EXTERNAL_INCLUDES} ${CMAKE_SOURCE_DIR}/include)
link_libraries(rayflex ${RAYFLEX_EXTERNAL_LINKS})

add_executable(client client.cpp)
target_compile_definitions(client PRIVATE SUPPORT_NET=1)

add_executable(server server.cpp)
target_compile_definitions(server PRIVATE SUPPORT_NET=1)
