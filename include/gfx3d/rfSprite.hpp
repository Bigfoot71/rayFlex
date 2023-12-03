#ifndef RAYFLEX_GFX_3D_SPRITE_HPP
#define RAYFLEX_GFX_3D_SPRITE_HPP

#include "../gfx2d/rfSprite.hpp"

namespace rf { namespace gfx3d {

    /**
     * @brief Derived class for rendering animated 3D sprites on billboards.
     */
    class Sprite : gfx2d::Sprite
    {
      private:
        /**
         * @brief Draw the billboard with advanced options.
         * @param camera Camera3D object.
         * @param source Source rectangle for the sprite.
         * @param position Position of the billboard.
         * @param up Up vector of the billboard.
         * @param size Size of the billboard.
         * @param origin Origin vector.
         * @param rotation Rotation angle of the billboard.
         * @param tint Tint color of the billboard.
         */
        void DrawBillboardSuperPro(
            const Camera3D& camera,
            Rectangle source,
            const raylib::Vector3& position,
            const raylib::Vector3& up,
            const raylib::Vector2& size,
            const raylib::Vector2& origin,
            float rotation,
            Color tint) const;

      public:
        /**
         * @brief Draw a billboard in 3D space.
         * @param camera Camera3D object.
         * @param pos Position of the billboard.
         * @param color Tint color of the billboard.
         * @param instance Sprite instance for animation.
         */
        void DrawBillboard(Camera3D& camera, Vector3& pos, Color color, const Instance * const instance) const;

        /**
         * @brief Draw a billboard in 3D space with specified size.
         * @param camera Camera3D object.
         * @param pos Position of the billboard.
         * @param size Size of the billboard.
         * @param color Tint color of the billboard.
         * @param instance Sprite instance for animation.
         */
        void DrawBillboard(Camera3D& camera, Vector3& pos, Vector2 size, Color color, const Instance * const instance) const;

        /**
         * @brief Draw a billboard in 3D space with default color.
         * @param camera Camera3D object.
         * @param pos Position of the billboard.
         * @param color Tint color of the billboard.
         * @param keyInstance Key of the sprite instance for animation.
         */
        void DrawBillboard(Camera3D& camera, Vector3& pos, Color color = WHITE, const std::string& keyInstance = "main") const;

        /**
         * @brief Draw a billboard in 3D space with specified size and default color.
         * @param camera Camera3D object.
         * @param pos Position of the billboard.
         * @param size Size of the billboard.
         * @param color Tint color of the billboard.
         * @param keyInstance Key of the sprite instance for animation.
         */
        void DrawBillboard(Camera3D& camera, Vector3& pos, Vector2 size, Color color = WHITE, const std::string& keyInstance = "main") const;
    };

}}

#endif //RAYFLEX_GFX_3D_SPRITE_HPP
