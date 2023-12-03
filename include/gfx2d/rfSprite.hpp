#ifndef RAYFLEX_GFX_2D_SPRITE_HPP
#define RAYFLEX_GFX_2D_SPRITE_HPP

#include <raylib-cpp.hpp>
#include <unordered_map>
#include <cstdint>
#include <memory>
#include <rlgl.h>

namespace rf { namespace gfx2d {

    /**
     * @brief Class for managing 2D sprite animations.
     */
    class Sprite
    {
      public:
        /**
         * @brief Struct representing an animation within the sprite.
         */
        struct Animation
        {
            float speed;        ///< The animation speed.
            uint16_t count;     ///< The number of frames in the animation.
            uint16_t start;     ///< The starting frame index.
            uint16_t end;       ///< The ending frame index.
            bool loop;          ///< Whether the animation should loop.

            /**
             * @brief Constructor for the Animation struct.
             * @param s The animation speed.
             * @param c The number of frames in the animation.
             * @param st The starting frame index.
             * @param e The ending frame index.
             * @param l Whether the animation should loop.
             */
            Animation(float s, uint16_t c, uint16_t st, uint16_t e, bool l)
                : speed(s), count(c), start(st), end(e), loop(l) { }
        };

        /**
         * @brief Struct representing an instance of a sprite animation.
         */
        struct Instance
        {
            raylib::Rectangle frameRec; ///< The current frame rectangle.
            Animation* animation;       ///< Pointer to the associated animation.
            float animTime;             ///< The current animation time.
            uint16_t currentFrame;      ///< The current frame index.

            /**
             * @brief Constructor for the Instance struct.
             * @param fr The current frame rectangle.
             * @param a Pointer to the associated animation.
             * @param at The current animation time.
             * @param cf The current frame index.
             */
            Instance(const raylib::Rectangle& fr, Animation* a, float at, uint16_t cf)
                : frameRec(fr), animation(a), animTime(at), currentFrame(cf) { }
        };

        using MapAnimations = std::unordered_map<std::string, std::unique_ptr<Animation>>; ///< Type alias for the map of animations.
        using MapInstances = std::unordered_map<std::string, std::unique_ptr<Instance>>;   ///< Type alias for the map of animation instances.

      protected:
        MapAnimations animations;       ///< Map of animation names to Animation objects.
        MapInstances instances;         ///< Map of animation instance names to Instance objects.

        raylib::Texture2D* texture;     ///< Pointer to the texture used by the sprite.
        bool unloadTexture = false;     ///< Flag indicating whether the texture should be unloaded.

        raylib::Vector2 frameSize;      ///< The size of each frame in the sprite sheet.
        raylib::Vector2 frameCenter;    ///< The center of each frame in the sprite sheet.
        raylib::Rectangle texSource;    ///< The source rectangle for the sprite sheet.

        uint8_t cols, rows;             ///< The number of columns and rows in the sprite sheet.
        uint16_t frameNum;              ///< The total number of frames in the sprite sheet.

      public:
        /**
         * @brief Default constructor for the Sprite class.
         */
        Sprite() { }

        /**
         * @brief Constructor for the Sprite class with image path, columns, rows, and animation speed.
         * @param imPath The path to the image file.
         * @param cols The number of columns in the sprite sheet.
         * @param rows The number of rows in the sprite sheet.
         * @param speed The animation speed.
         */
        Sprite(const std::string& imPath, int cols, int rows, float speed = 0.1f)
        {
            Load(imPath, cols, rows, speed);
        }

        /**
         * @brief Constructor for the Sprite class with image path, columns, rows, texture source, and animation speed.
         * @param imPath The path to the image file.
         * @param cols The number of columns in the sprite sheet.
         * @param rows The number of rows in the sprite sheet.
         * @param texSource The source rectangle for the sprite sheet.
         * @param speed The animation speed.
         */
        Sprite(const std::string& imPath, int cols, int rows, const raylib::Rectangle& texSource, float speed = 0.1f)
        {
            Load(imPath, cols, rows, texSource, speed);
        }

        /**
         * @brief Constructor for the Sprite class with texture, columns, rows, texture source, and animation speed.
         * @param texture Pointer to the texture used by the sprite.
         * @param cols The number of columns in the sprite sheet.
         * @param rows The number of rows in the sprite sheet.
         * @param texSource The source rectangle for the sprite sheet.
         * @param speed The animation speed.
         */
        Sprite(raylib::Texture2D* texture, int cols, int rows, const raylib::Rectangle& texSource, float speed = 0.1f)
        {
            Load(texture, cols, rows, texSource, speed);
        }

        /**
         * @brief Deleted copy constructor to disable copying.
         */
        Sprite(const Sprite&) = delete;

        /**
         * @brief Deleted copy assignment operator to disable copying.
         */
        Sprite& operator=(const Sprite&) = delete;

        /**
         * @brief Move constructor for the Sprite class.
         * @param other Another instance of the Sprite class.
         */
        Sprite(Sprite&& other) noexcept
            : animations(std::move(other.animations))
            , instances(std::move(other.instances))
            , texture(std::exchange(other.texture, nullptr))
            , unloadTexture(std::exchange(other.unloadTexture, false))
            , frameSize(other.frameSize)
            , frameCenter(other.frameCenter)
            , texSource(other.texSource)
            , cols(other.cols)
            , rows(other.rows)
            , frameNum(other.frameNum)
        { }

        /**
         * @brief Move assignment operator for the Sprite class.
         * @param other Another instance of the Sprite class.
         * @return Reference to the current instance.
         */
        Sprite& operator=(Sprite&& other) noexcept
        {
            if (this != &other)
            {
                animations = std::move(other.animations);
                instances = std::move(other.instances);
                texture = std::exchange(other.texture, nullptr);
                unloadTexture = std::exchange(other.unloadTexture, false);
                frameSize = other.frameSize;
                frameCenter = other.frameCenter;
                texSource = other.texSource;
                cols = other.cols;
                rows = other.rows;
                frameNum = other.frameNum;
            }
            return *this;
        }

        /**
         * @brief Destructor for the Sprite class.
         */
        ~Sprite()
        {
            if (unloadTexture && texture) delete texture;
        }

        // LOADING METHODS //

        /**
         * @brief Load sprite sheet from file with specified number of columns, rows, and animation speed.
         * @param imPath The path to the image file.
         * @param cols The number of columns in the sprite sheet.
         * @param rows The number of rows in the sprite sheet.
         * @param speed The animation speed.
         */
        void Load(const std::string& imPath, int cols, int rows, float speed = 0.1f);

        /**
         * @brief Load sprite sheet from file with specified number of columns, rows, texture source, and animation speed.
         * @param imPath The path to the image file.
         * @param cols The number of columns in the sprite sheet.
         * @param rows The number of rows in the sprite sheet.
         * @param texSource The source rectangle for the sprite sheet.
         * @param speed The animation speed.
         */
        void Load(const std::string& imPath, int cols, int rows, Rectangle texSource, float speed = 0.1f);

        /**
         * @brief Load sprite sheet from an image with specified number of columns, rows, texture source, and animation speed.
         * @param image The image data for the sprite sheet.
         * @param cols The number of columns in the sprite sheet.
         * @param rows The number of rows in the sprite sheet.
         * @param texSource The source rectangle for the sprite sheet.
         * @param speed The animation speed.
         */
        void Load(Image image, int cols, int rows, Rectangle texSource, float speed = 0.1f);

        /**
         * @brief Load sprite sheet from a texture with specified number of columns, rows, texture source, and animation speed.
         * @param texture Pointer to the texture used by the sprite.
         * @param cols The number of columns in the sprite sheet.
         * @param rows The number of rows in the sprite sheet.
         * @param texSource The source rectangle for the sprite sheet.
         * @param speed The animation speed.
         */
        void Load(raylib::Texture2D* texture, int cols, int rows, const Rectangle& texSource, float speed = 0.1f);

        // ANIMATION MANAGEMENT //

        /**
         * @brief Create a new animation and store it in the animations map.
         * @param keyAnimation The key to identify the animation.
         * @param startFrame The starting frame index of the animation.
         * @param endFrame The ending frame index of the animation.
         * @param speed The animation speed.
         * @param loop Whether the animation should loop.
         * @return A pointer to the created animation.
         */
        Animation* NewAnimation(const std::string& keyAnimation, uint16_t startFrame, uint16_t endFrame, float speed, bool loop);

        /**
         * @brief Set the active animation for a given instance.
         * @param keyAnimation The key identifying the animation.
         * @param keyInstance The key identifying the instance (default is "main").
         */
        void SetAnimation(const std::string& keyAnimation, const std::string& keyInstance = "main");

        /**
         * @brief Get a pointer to the animation with the specified key.
         * @param keyAnimation The key identifying the animation.
         * @return A pointer to the animation.
         */
        Animation* GetAnimation(const std::string& keyAnimation);

        /**
         * @brief Get a const pointer to the animation with the specified key.
         * @param keyAnimation The key identifying the animation.
         * @return A const pointer to the animation.
         */
        const Animation* GetAnimation(const std::string& keyAnimation) const;

        /**
         * @brief Get the source rectangle for a specific frame of an animation.
         * @param frameIndex The index of the frame.
         * @param animation A pointer to the animation.
         * @return The source rectangle for the frame.
         */
        Rectangle GetAnimationFrameRec(uint16_t frameIndex, const Animation * const animation) const;

        /**
         * @brief Get the source rectangle for a specific frame of the active animation.
         * @param frameIndex The index of the frame.
         * @param keyAnimation The key identifying the animation (default is "main").
         * @return The source rectangle for the frame.
         */
        Rectangle GetAnimationFrameRec(uint16_t frameIndex, const std::string& keyAnimation = "main") const;

        /**
         * @brief Set the speed of the active animation.
         * @param time The animation speed.
         * @param keyAnimation The key identifying the animation (default is "main").
         */
        void SetAnimationSpeed(float time, const std::string& keyAnimation = "main");

        /**
         * @brief Set whether the active animation should loop.
         * @param loop Whether the animation should loop.
         * @param keyAnimation The key identifying the animation (default is "main").
         */
        void SetAnimationLoop(bool loop, const std::string& keyAnimation = "main");

        /**
         * @brief Get an iterator pointing to the beginning of the animations map.
         * @return Const iterator pointing to the beginning of the animations map.
         */
        MapAnimations::const_iterator GetBeginAnimations() const;

        /**
         * @brief Get an iterator pointing to the end of the animations map.
         * @return Const iterator pointing to the end of the animations map.
         */
        MapAnimations::const_iterator GetEndAnimations() const;

        // INSTANCE MANAGEMENT //

        /**
         * @brief Create a new instance and store it in the instances map.
         * @param keyInstance The key to identify the instance.
         * @param animation A pointer to the animation to associate with the instance.
         * @return A pointer to the created instance.
         */
        Instance* NewInstance(const std::string& keyInstance, Animation* animation);

        /**
         * @brief Create a new instance and store it in the instances map using the specified animation key.
         * @param keyInstance The key to identify the instance.
         * @param keyAnimation The key identifying the animation to associate with the instance (default is "main").
         * @return A pointer to the created instance.
         */
        Instance* NewInstance(const std::string& keyInstance, const std::string& keyAnimation = "main")
        {
            return NewInstance(keyInstance, GetAnimation(keyAnimation));
        }

        /**
         * @brief Remove an instance from the instances map.
         * @param keyInstance The key identifying the instance to remove.
         */
        void RemoveInstance(const std::string& keyInstance);

        /**
         * @brief Remove all instances from the instances map.
         */
        void ClearInstances();

        /**
         * @brief Get a pointer to the instance with the specified key.
         * @param keyInstance The key identifying the instance.
         * @return A pointer to the instance.
         */
        Instance* GetInstance(const std::string& keyInstance);

        /**
         * @brief Get a const pointer to the instance with the specified key.
         * @param keyInstance The key identifying the instance.
         * @return A const pointer to the instance.
         */
        const Instance* GetInstance(const std::string& keyInstance) const;

        /**
         * @brief Set the current frame of the active animation for a specific instance.
         * @param position The frame index to set.
         * @param keyInstance The key identifying the instance (default is "main").
         */
        void GotoFrame(uint16_t position, const std::string& keyInstance = "main");

        /**
         * @brief Check if the current frame of the active animation for a specific instance is equal to the specified frame index.
         * @param position The frame index to compare.
         * @param keyInstance The key identifying the instance (default is "main").
         * @return True if the current frame is equal to the specified frame index, otherwise false.
         */
        bool IsCurrentFrameEqual(uint16_t position, const std::string& keyInstance = "main") const;

        /**
         * @brief Check if the current frame of the active animation for a specific instance is before the specified frame index.
         * @param position The frame index to compare.
         * @param keyInstance The key identifying the instance (default is "main").
         * @return True if the current frame is before the specified frame index, otherwise false.
         */
        bool IsCurrentFrameBefore(uint16_t position, const std::string& keyInstance = "main") const;

        /**
         * @brief Check if the current frame of the active animation for a specific instance is after the specified frame index.
         * @param position The frame index to compare.
         * @param keyInstance The key identifying the instance (default is "main").
         * @return True if the current frame is after the specified frame index, otherwise false.
         */
        bool IsCurrentFrameAfter(uint16_t position, const std::string& keyInstance = "main") const;

        /**
         * @brief Check if the active animation for a specific instance has finished.
         * @param keyInstance The key identifying the instance (default is "main").
         * @return True if the animation has finished, otherwise false.
         */
        bool IsAnimFinished(const std::string& keyInstance = "main") const;

        /**
         * @brief Get an iterator pointing to the beginning of the instances map.
         * @return Const iterator pointing to the beginning of the instances map.
         */
        MapInstances::const_iterator GetBeginInstances() const;

        /**
         * @brief Get an iterator pointing to the end of the instances map.
         * @return Const iterator pointing to the end of the instances map.
         */
        MapInstances::const_iterator GetEndInstances() const;

        // INSTANCE - UPDATE AND DRAW //

        /**
         * @brief Update the animation state of a specific instance.
         * @param dt The time elapsed since the last frame.
         * @param instance A pointer to the instance to update.
         */
        void Update(float dt, Instance* instance);

        /**
         * @brief Update the animation state of all instances.
         * @param dt The time elapsed since the last frame.
         */
        void UpdateAll(float dt);

        /**
         * @brief Update the animation state of a specific instance using the specified key.
         * @param dt The time elapsed since the last frame.
         * @param keyInstance The key identifying the instance (default is "main").
         */
        void Update(float dt, const std::string& keyInstance = "main")
        {
            Update(dt, GetInstance(keyInstance));
        }

        /**
         * @brief Draw the sprite at a specific position and color for a specific instance.
         * @param pos The position to draw the sprite.
         * @param color The color tint to apply.
         * @param instance A pointer to the instance to draw.
         */
        void Draw(Vector2 pos, Color color, const Instance * const instance) const;

        /**
         * @brief Draw the sprite with additional parameters for scaling, rotation, and UV origin for a specific instance.
         * @param pos The position to draw the sprite.
         * @param scale The scale factor to apply.
         * @param rotation The rotation angle in degrees.
         * @param uvOrigin The UV origin for rotation (default is { 0.5f, 0.5f }).
         * @param color The color tint to apply.
         * @param instance A pointer to the instance to draw.
         */
        void Draw(Vector2 pos, float scale, float rotation, Vector2 uvOrigin, Color color, const Instance * const instance) const;

        /**
         * @brief Draw the sprite with additional parameters for scaling, rotation, and UV origin for a specific instance.
         * @param pos The position to draw the sprite.
         * @param sx The horizontal scale factor to apply.
         * @param sy The vertical scale factor to apply.
         * @param rotation The rotation angle in degrees.
         * @param uvOrigin The UV origin for rotation (default is { 0.5f, 0.5f }).
         * @param color The color tint to apply.
         * @param instance A pointer to the instance to draw.
         */
        void Draw(Vector2 pos, float sx, float sy, float rotation, Vector2 uvOrigin, Color color, const Instance * const instance) const;

        /**
         * @brief Draw the sprite within a destination rectangle with additional parameters for origin, rotation, and color for a specific instance.
         * @param dest The destination rectangle to draw the sprite.
         * @param origin The origin point for rotation.
         * @param rot The rotation angle in degrees.
         * @param color The color tint to apply.
         * @param instance A pointer to the instance to draw.
         */
        void Draw(Rectangle dest, Vector2 origin, float rot, Color color, const Instance * const instance) const;

        /**
         * @brief Draw the sprite at a specific position and color for a specific instance using default values.
         * @param pos The position to draw the sprite.
         * @param color The color tint to apply (default is WHITE).
         * @param keyInstance The key identifying the instance (default is "main").
         */
        void Draw(Vector2 pos, Color color = WHITE, const std::string& keyInstance = "main") const
        {
            Draw(pos, color, GetInstance(keyInstance));
        }

        /**
         * @brief Draw the sprite with additional parameters for scaling, rotation, and UV origin for a specific instance using default values.
         * @param pos The position to draw the sprite.
         * @param scale The scale factor to apply.
         * @param rotation The rotation angle in degrees.
         * @param uvOrigin The UV origin for rotation (default is { 0.5f, 0.5f }).
         * @param color The color tint to apply (default is WHITE).
         * @param keyInstance The key identifying the instance (default is "main").
         */
        void Draw(Vector2 pos, float scale, float rotation, Vector2 uvOrigin = { 0.5f, 0.5f }, Color color = WHITE, const std::string& keyInstance = "main") const
        {
            Draw(pos, scale, rotation, uvOrigin, color, GetInstance(keyInstance));
        }

        /**
         * @brief Draw the sprite with additional parameters for scaling, rotation, and UV origin for a specific instance using default values.
         * @param pos The position to draw the sprite.
         * @param sx The horizontal scale factor to apply.
         * @param sy The vertical scale factor to apply.
         * @param rotation The rotation angle in degrees.
         * @param uvOrigin The UV origin for rotation (default is { 0.5f, 0.5f }).
         * @param color The color tint to apply (default is WHITE).
         * @param keyInstance The key identifying the instance (default is "main").
         */
        void Draw(Vector2 pos, float sx, float sy, float rotation, Vector2 uvOrigin = { 0.5f, 0.5f }, Color color = WHITE, const std::string& keyInstance = "main") const
        {
            Draw(pos, sx, sy, rotation, uvOrigin, color, GetInstance(keyInstance));
        }

        /**
         * @brief Draw the sprite within a destination rectangle with additional parameters for origin, rotation, and color for a specific instance using default values.
         * @param dest The destination rectangle to draw the sprite.
         * @param origin The origin point for rotation.
         * @param rot The rotation angle in degrees.
         * @param color The color tint to apply (default is WHITE).
         * @param keyInstance The key identifying the instance (default is "main").
         */
        void Draw(Rectangle dest, Vector2 origin, float rot, Color color = WHITE, const std::string& keyInstance = "main") const
        {
            Draw(dest, origin, rot, color, GetInstance(keyInstance));
        }

        // MISC METHODS //

        /**
         * @brief Get the grid size of the sprite (number of rows and columns).
         * @return A Vector2 representing the number of rows and columns.
         */
        raylib::Vector2 GetGrid() const
        {
            return { static_cast<float>(rows), static_cast<float>(cols) };
        }

        /**
         * @brief Get the size of each frame in the sprite.
         * @return A Vector2 representing the size of each frame.
         */
        raylib::Vector2 GetFrameSize() const
        {
            return frameSize;
        }

        /**
         * @brief Get the center of each frame in the sprite.
         * @return A Vector2 representing the center of each frame.
         */
        raylib::Vector2 GetFrameCenter() const
        {
            return frameCenter;
        }
    };

}}

#endif //RAYFLEX_GFX_2D_SPRITE_HPP
