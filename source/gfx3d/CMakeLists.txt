if(SUPPORT_GFX_3D)
    set(RAYFLEX_SOURCE_GFX_3D
        source/gfx3d/rfCamera.cpp
        source/gfx3d/rfLights.cpp
        source/gfx3d/rfSprite.cpp
    )
    if(NOT SUPPORT_GFX_2D)
        list(APPEND RAYFLEX_SOURCE_GFX_3D
            source/gfx2d/rfSprite.cpp
        )
    endif()
endif()