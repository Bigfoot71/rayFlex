#ifndef RAYFLEX_CORE_CURSOR_HPP
#define RAYFLEX_CORE_CURSOR_HPP

#include <Texture.hpp>
#include <Rectangle.hpp>

namespace rf { namespace core {

    class Cursor
    {
      private:
        raylib::Texture *texture = nullptr;
        raylib::Rectangle source{};
        raylib::Vector2 origin{};
        float scale = 1.0f;
        Color color{};

      public:
        void Set(raylib::Texture* texture, const raylib::Vector2& origin = {}, float scale = 1.0f, const raylib::Rectangle& source = {}, const Color& color = WHITE)
        {
            this->texture = texture;
            this->origin = origin;
            this->scale = scale;
            this->color = color;

            if (source.width == 0 || source.height == 0)
            {
                this->source.SetPosition(0, 0);
                this->source.SetSize(texture->width, texture->height);
            }
            else
            {
                this->source = source;
            }
        }

        bool IsActive() const
        {
            return texture != nullptr;
        }

        void Draw(const raylib::Vector2& position)
        {
            texture->Draw(source, { position.x, position.y, source.width * scale, source.height * scale }, origin, 0.0f, color);
        }
    };

}}

#endif //RAYFLEX_CORE_CURSOR_HPP
