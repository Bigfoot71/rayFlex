include(source/core/CMakeLists.txt)
include(source/gfx2d/CMakeLists.txt)
include(source/gfx3d/CMakeLists.txt)
include(source/phys2d/CMakeLists.txt)
include(source/phys3d/CMakeLists.txt)
include(source/net/CMakeLists.txt)

set(RAYFLEX_SOURCE
    ${RAYFLEX_SOURCE_CORE}
    ${RAYFLEX_SOURCE_GFX_2D}
    ${RAYFLEX_SOURCE_GFX_3D}
    ${RAYFLEX_SOURCE_PHYS_2D}
    ${RAYFLEX_SOURCE_PHYS_3D}
    ${RAYFLEX_SOURCE_NET}
)