#ifndef RAYFLEX_CORE_RENDERER_HPP
#define RAYFLEX_CORE_RENDERER_HPP

#include <RenderTexture.hpp>

namespace rf { namespace core {

    /**
     * @brief The Renderer class facilitates the use of RenderTextures for final screen rendering.
     * It provides functionality to manage scaling, aspect ratio, and internal rendering offset.
     * This class is primarily used internally but can also be optionally used by the framework's users.
     */
    class Renderer {

      private:
        raylib::RenderTexture target;   ///< The target RenderTexture for rendering.
        raylib::Vector2 offset;         ///< The rendering offset.
        raylib::Vector2 scale;          ///< The rendering scale.
        float aspectRatio;              ///< The aspect ratio of the rendering.
        bool keepRatio;                 ///< Flag indicating whether to maintain aspect ratio during rendering.

      private:
        /**
         * @brief Gets the destination width for rendering.
         * @return The destination width.
         */
        float GetDestWidth() const
        {
            return static_cast<float>(GetRenderWidth());
        }

        /**
         * @brief Gets the destination height for rendering.
         * @return The destination height.
         */
        float GetDestHeight() const
        {
            return static_cast<float>(GetRenderHeight());
        }

        /**
         * @brief Gets the destination dimensions for rendering.
         * @return The destination dimensions as a Vector2.
         */
        raylib::Vector2 GetDestDimensions() const
        {
            return {
                static_cast<float>(GetRenderWidth()),
                static_cast<float>(GetRenderHeight())
            };
        }

        /**
         * @brief Gets the source rectangle for rendering.
         * @return The source rectangle.
         */
        Rectangle GetRecSrc() const
        {
            return { 0.0f, 0.0f, GetWidth(), -GetHeight() };
        }

        /**
         * @brief Gets the destination rectangle for rendering.
         * If aspect ratio is maintained, the rectangle is scaled to fit while preserving the aspect ratio.
         * @return The destination rectangle.
         */
        Rectangle GetRecDst() const
        {
            if (keepRatio)
            {
                const float minScale = GetMinScale();
                return { offset.x, offset.y, GetWidth() * minScale, GetHeight() * minScale };
            }
            return { 0.0f, 0.0f, GetDestWidth(), GetDestHeight() };
        }

        /**
         * @brief Updates the rendering scale based on the destination dimensions and the RenderTexture size.
         */
        void UpdateScale()
        {
            scale = GetDestDimensions() / GetSize();
        }

        /**
         * @brief Updates the rendering offset based on the minimum scale and the RenderTexture size.
         */
        void UpdateOffset()
        {
            const float minScale = GetMinScale();
            offset.x = (GetDestWidth() - target.texture.width * minScale) * 0.5f;
            offset.y = (GetDestHeight() - target.texture.height * minScale) * 0.5f;
        }

        /**
         * @brief Updates the aspect ratio based on the RenderTexture dimensions.
         */
        void UpdateAspectRatio()
        {
            aspectRatio = (target.texture.width > target.texture.height)
                ? static_cast<float>(target.texture.width) / static_cast<float>(target.texture.height)
                : static_cast<float>(target.texture.height) / static_cast<float>(target.texture.width);
        }

      public:
        /**
         * @brief Default constructor for the Renderer class.
         * Initializes the target RenderTexture, offset, scale, aspect ratio, and keepRatio flag.
         */
        Renderer() : target(), offset(), scale(), aspectRatio(0), keepRatio(0) { }

        /**
         * @brief Parameterized constructor for the Renderer class.
         * @param size The initial size of the RenderTexture.
         * @param keepRatio Flag indicating whether to maintain aspect ratio during rendering.
         */
        Renderer(const Vector2& size, bool keepRatio)
        {
            Load(static_cast<int>(size.x), static_cast<int>(size.y), keepRatio);
        }

        /**
         * @brief Parameterized constructor for the Renderer class.
         * @param width The initial width of the RenderTexture.
         * @param height The initial height of the RenderTexture.
         * @param keepRatio Flag indicating whether to maintain aspect ratio during rendering.
         */
        Renderer(int width, int height, bool keepRatio)
        {
            Load(width, height, keepRatio);
        }

        /**
         * @brief Loads the Renderer with the specified width, height, and aspect ratio preservation flag.
         * @param size The initial size of the RenderTexture.
         * @param keepRatio Flag indicating whether to maintain aspect ratio during rendering.
         */
        void Load(const Vector2& size, bool keepRatio)
        {
            Load(static_cast<int>(size.x), static_cast<int>(size.y), keepRatio);
        }

        /**
         * @brief Loads the Renderer with the specified width, height, and aspect ratio preservation flag.
         * @param width The initial width of the RenderTexture.
         * @param height The initial height of the RenderTexture.
         * @param keepRatio Flag indicating whether to maintain aspect ratio during rendering.
         */
        void Load(int width, int height, bool keepRatio)
        {
            this->target = raylib::RenderTexture(
                width == 0 ? GetDestWidth() : width,
                height == 0 ? GetDestHeight() : height);

            this->keepRatio = keepRatio;

            UpdateScale();
            UpdateOffset();
            UpdateAspectRatio();
        }

        /**
         * @brief Unloads the target RenderTexture.
         */
        void Unload()
        {
            target.Unload();
        }

        /**
         * @brief Checks if the target RenderTexture is ready for rendering.
         * @return True if the target RenderTexture is ready; otherwise, false.
         */
        bool IsReady() const
        {
            return target.IsReady();
        }

        /**
         * @brief Checks if the aspect ratio is being maintained during rendering.
         * @return True if aspect ratio is maintained; otherwise, false.
         */
        bool IsRatioKept() const
        {
            return keepRatio;
        }

        /**
         * @brief Clears the target RenderTexture with the specified color.
         * @param color The color to clear the RenderTexture with.
         */
        void Clear(Color color = BLACK) const
        {
            ClearBackground(color);
        }

        /**
         * @brief Begins rendering to the target RenderTexture.
         * @return A reference to the Renderer instance.
         */
        Renderer& BeginMode()
        {
            target.BeginMode();
            return *this;
        }

        /**
         * @brief Ends rendering to the target RenderTexture.
         * @return A reference to the Renderer instance.
         */
        Renderer& EndMode()
        {
            target.EndMode();
            return *this;
        }

        /**
         * @brief Updates the Renderer's scale, offset, and aspect ratio based on the current state.
         */
        void Update()
        {
            UpdateScale();
            if (keepRatio) UpdateOffset();
            else UpdateAspectRatio();
        }

        /**
         * @brief Draws the target RenderTexture to the screen with optional tinting.
         * @param tint The tint color to apply to the RenderTexture.
         */
        void Draw(Color tint = WHITE) const
        {
            DrawTexturePro(target.texture, GetRecSrc(), GetRecDst(), { 0, 0 }, 0, tint);
        }

        /**
         * @brief Sets whether to maintain aspect ratio during rendering.
         * @param enabled True to maintain aspect ratio; false otherwise.
         */
        void KeepRatio(bool enabled)
        {
            if (keepRatio == enabled) return;
            keepRatio = enabled;

            UpdateScale();
            UpdateOffset();
            UpdateAspectRatio();
        }

        /**
         * @brief Resizes the target RenderTexture to the specified size.
         * @param size The new size of the RenderTexture.
         */
        void Resize(const Vector2& size)
        {
            Resize(static_cast<int>(size.x), static_cast<int>(size.y));
        }

        /**
         * @brief Resizes the target RenderTexture to the specified width and height.
         * @param width The new width of the RenderTexture.
         * @param height The new height of the RenderTexture.
         */
        void Resize(int width, int height)
        {
            target.Unload();
            target = raylib::RenderTexture(width, height);
            UpdateScale(); UpdateOffset(); UpdateAspectRatio();
        }

        /**
         * @brief Gets a reference to the Texture2D of the target RenderTexture.
         * @return A const reference to the Texture2D of the target RenderTexture.
         */
        const Texture2D& GetTexture() const
        {
            return target.texture;
        }

        /**
         * @brief Gets the current rendering offset.
         * @return The current rendering offset as a Vector2.
         */
        raylib::Vector2 GetOffset() const
        {
            return offset;
        }

        /**
         * @brief Gets the current rendering scale.
         * @return The current rendering scale as a Vector2.
         */
        raylib::Vector2 GetScale() const
        {
            return scale;
        }

        /**
         * @brief Gets the current aspect ratio of the target RenderTexture.
         * @return The current aspect ratio as a float.
         */
        float GetAspectRatio() const
        {
            return aspectRatio;
        }

        /**
         * @brief Gets the minimum scale factor between the x and y scales.
         * @return The minimum scale factor.
         */
        float GetMinScale() const
        {
            return std::min(scale.x, scale.y);
        }

        /**
         * @brief Gets the current width of the target RenderTexture.
         * @return The current width as a float.
         */
        float GetWidth() const
        {
            return static_cast<float>(target.texture.width);
        }

        /**
         * @brief Gets the current height of the target RenderTexture.
         * @return The current height as a float.
         */
        float GetHeight() const
        {
            return static_cast<float>(target.texture.height);
        }

        /**
         * @brief Gets the current size of the target RenderTexture.
         * @return The current size as a Vector2.
         */
        raylib::Vector2 GetSize() const
        {
            return raylib::Vector2(GetWidth(), GetHeight());
        }

        /**
         * @brief Converts UV coordinates to pixel coordinates based on the current RenderTexture size.
         * @param uv The UV coordinates to convert.
         * @return The converted pixel coordinates as a Vector2.
         */
        raylib::Vector2 ConvertUV(raylib::Vector2 uv)
        {
            return uv * GetSize();
        }

        /**
         * @brief Converts screen coordinates to renderer coordinates.
         * @param p The screen coordinates to convert.
         * @return The converted renderer coordinates as a Vector2.
         */
        raylib::Vector2 ScreenToRenderer(raylib::Vector2 p) const
        {
            if (!keepRatio) return (p - offset) / scale;
            return (p - offset) * (1.0f / GetMinScale());
        }

        /**
         * @brief Converts renderer coordinates to screen coordinates.
         * @param p The renderer coordinates to convert.
         * @return The converted screen coordinates as a Vector2.
         */
        raylib::Vector2 RendererToScreen(raylib::Vector2 p) const
        {
            if (!keepRatio) return p * scale + offset;
            return p * (1.0f / GetMinScale()) + offset;
        }

        /**
         * @brief Gets a lambda function for converting screen coordinates to renderer coordinates.
         * @return A lambda function taking a Vector2 parameter and returning the converted renderer coordinates.
         */
        auto GetScreenToRendererLambda() const
        {
            return [this](raylib::Vector2 p) { return this->ScreenToRenderer(p); };
        }

        /**
         * @brief Gets a lambda function for converting renderer coordinates to screen coordinates.
         * @return A lambda function taking a Vector2 parameter and returning the converted screen coordinates.
         */
        auto GetRendererToScreenLambda() const
        {
            return [this](raylib::Vector2 p) { return this->RendererToScreen(p); };
        }
    };

}}

#endif //RAYFLEX_CORE_RENDERER_HPP
