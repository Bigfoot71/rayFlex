#ifndef RAYFLEX_HPP
#define RAYFLEX_HPP

#include <raylib-cpp.hpp>

#include "core/rfApp.hpp"
#include "core/rfRandom.hpp"
#include "core/rfRenderer.hpp"
#include "core/rfSaveManager.hpp"
#include "core/rfAssetManager.hpp"

#ifdef SUPPORT_GFX_2D
#   include "gfx2d/rfParticles.hpp"
#   include "gfx2d/rfSprite.hpp"
#endif

#ifdef SUPPORT_GFX_3D
#   include "gfx3d/rfCamera.hpp"
#   include "gfx3d/rfLights.hpp"
#   include "gfx2d/rfSprite.hpp"    ///< gfx3d::Sprite depends on gfx2d::Sprite
#   include "gfx3d/rfSprite.hpp"
#endif

#ifdef SUPPORT_PHYS_2D
#   include "phys2d/rfPhysics.hpp"
#endif

#ifdef SUPPORT_PHYS_3D
#   include "phys3d/rfModel.hpp"
#   include "phys3d/rfObject.hpp"
#   include "phys3d/rfWorld.hpp"
#endif

#ifdef SUPPORT_NET
#   include "net/rfClientInterface.hpp"
#   include "net/rfConnection.hpp"
#   include "net/rfPacket.hpp"
#   include "net/rfSecurity.hpp"
#   include "net/rfServerInterface.hpp"
#   include "net/rfTSQueue.hpp"
#endif

#endif //RAYFLEX_HPP