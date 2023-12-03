#ifndef RAYFLEX_GFX_3D_LIGHTS_HPP
#define RAYFLEX_GFX_3D_LIGHTS_HPP

#include <functional>
#include <Shader.hpp>
#include <Matrix.hpp>
#include <Model.hpp>
#include <Color.hpp>

#include "./rfCamera.hpp"
#include "raylib.h"

namespace rf { namespace gfx3d {

    /**
     * @brief Structure representing a shadow map for shadow rendering.
     */
    struct ShadowMap
    {
        unsigned int id;   ///< The OpenGL framebuffer ID.
        int width;         ///< The width of the shadow map.
        int height;        ///< The height of the shadow map.
        Texture2D depth;   ///< The depth texture of the shadow map.

        /**
         * @brief Construct a new Shadow Map object with the specified width and height.
         * @param _width Width of the shadow map.
         * @param _height Height of the shadow map.
         */
        ShadowMap(int _width, int _height);

        /**
         * @brief Destroy the Shadow Map object.
         */
        ~ShadowMap();

        /**
         * @brief Begin the shadow map rendering mode.
         * @param bounds The viewport bounds for rendering to the shadow map.
         */
        void BeginMode(const Rectangle& bounds) const;

        /**
         * @brief End the shadow map rendering mode.
         */
        void EndMode() const;

        /**
         * @brief Clear the shadow map.
         * This function activates the framebuffer, clears it to white (maximum distance), and deactivates it.
         */
        void Clear() const;

        /**
         * @brief Draw the shadow map for debugging purposes.
         * @param shader The shader used for drawing the shadow map.
         * @param x X-coordinate of the draw position.
         * @param y Y-coordinate of the draw position.
         * @param w Width of the draw area.
         * @param h Height of the draw area.
         */
        void Draw(raylib::Shader& shader, float x = 0, float y = 0, float w = 256, float h = 256);
    };

    /**
     * @brief Class representing a light source in a 3D scene with shadow casting capabilities.
     */
    class Light
    {
      private:
        friend class Lights;                    ///< Declare the Lights class as a friend to allow access to private members.

      private:
        raylib::Shader& modelShader;            ///< Reference to the model shader used for rendering.
        int locsLightModelShader[9];            ///< Array of shader locations for light properties.
        ShadowMap *shadowMap;                   ///< Pointer to the shadow map associated with the light.
        Rectangle boundsMap;                    ///< Rectangle defining the area allocated to this light in the depth buffer.

        Camera caster;                          ///< Camera used for shadow casting.
        float radius;                           ///< Radius of the light source's influence.
        Color color;                            ///< Color of the light.
        bool enabled;                           ///< Flag indicating whether the light is enabled or not.

        /**
         * @brief Construct a new Light object with the specified parameters.
         * @param _modelShader Reference to the model shader.
         * @param _shadowMap Pointer to the associated shadow map.
         * @param lightNum The index of the light (used for shader locations).
         * @param _caster The casting camera used for shadows.
         * @param _radius Radius of the light source's influence.
         * @param _color Color of the light.
         */
        Light(raylib::Shader& _modelShader, ShadowMap* _shadowMap, uint16_t lightNum,
              const Camera& _caster, float _radius = 512, const Color& _color = WHITE);

        /**
         * @brief Set the bounds of the shadow map for this light.
         * @param bounds The bounds of the shadow map.
         */
        void SetShadowMapBounds(const Rectangle& bounds);

        /**
         * @brief Begin rendering to the depth buffer of the light for shadow calculations.
         */
        void BeginDepthMode();

        /**
         * @brief End rendering to the depth buffer of the light for shadow calculations.
         */
        void EndDepthMode();

        /**
         * @brief Clear the depth buffer.
         */
        void ClearBuffer();

      public:
        /**
         * @brief Enumerations for shader locations related to light properties.
         */
        enum LocsModelShader {
            LOC_LIGHT_MAT,          ///< Light matrix location.
            LOC_LIGHT_POS,          ///< Light position location.
            LOC_LIGHT_DIR,          ///< Light direction location.
            LOC_LIGHT_COLOR,        ///< Light color location.
            LOC_LIGHT_CUTOFF,       ///< Light cutoff location.
            LOC_LIGHT_RADIUS,       ///< Light radius location.
            LOC_LIGHT_BOUNDS,       ///< Light bounds location.
            LOC_LIGHT_SHADOW,       ///< Light shadow map location.
            LOC_LIGHT_ENABLED       ///< Light enabled flag location.
        };

        /**
         * @brief Get the position of the light.
         * @return The position of the light.
         */
        Vector3 GetPosition();

        /**
         * @brief Get the target position of the light.
         * @return The target position of the light.
         */
        Vector3 GetTarget();

        /**
         * @brief Get the vertical field of view (FOVY) of the light.
         * @return The FOVY of the light.
         */
        float GetFovY();

        /**
         * @brief Get the radius of the light's influence.
         * @return The radius of the light.
         */
        float GetRadius();

        /**
         * @brief Get the color of the light.
         * @return The color of the light.
         */
        Color GetColor();

        /**
         * @brief Check if the light is active.
         * @return True if the light is active, false otherwise.
         */
        bool IsActive();

        /**
         * @brief Update the matrix of the light based on its position, target, and FOVY.
         */
        void UpdateMatrix();

        /**
         * @brief Set the position of the light.
         * @param position The new position of the light.
         * @param updateMatrix If true, update the light matrix after setting the position.
         */
        void SetPosition(const Vector3& position, bool updateMatrix = true);

        /**
         * @brief Set the target position of the light.
         * @param target The new target position of the light.
         * @param updateMatrix If true, update the light matrix after setting the target.
         */
        void SetTarget(const Vector3& target, bool updateMatrix = true);

        /**
         * @brief Set the vertical field of view (FOVY) of the light caster.
         * @param fovy The new FOVY of the light caster.
         * @param updateMatrix If true, update the light matrix after setting the FOVY.
         */
        void SetFovY(float fovy, bool updateMatrix = true);

        /**
         * @brief Set the radius of the light's influence.
         * @param radius The new radius of the light.
         */
        void SetRadius(float radius);

        /**
         * @brief Set the color of the light.
         * @param color The new color of the light.
         */
        void SetColor(const Color& color);

        /**
         * @brief Set the active state of the light.
         * @param active True to enable the light, false to disable it.
         */
        void SetActive(bool active);

        /**
         * @brief Set the casting camera for the light.
         * @param caster The new casting camera for shadows.
         */
        void SetCaster(const Camera& caster);
    };

    /**
     * @brief Class representing a manager for 3D lights in a scene.
     */
    class Lights
    {
      private:
        raylib::Shader shadowShader;            ///< Shader for shadow mapping.
        raylib::Shader modelShader;             ///< Shader for light models.
        ShadowMap *shadowMap;                   ///< Pointer to the shadow map used for shadows.
        uint32_t bufferSize;                    ///< Size of the buffer used for shadow maps.
        uint16_t mapSize;                       ///< Size of the shadow map texture.

        std::vector<Light*> sources;            ///< Vector containing pointers to the light sources.
        uint16_t maxLights;                     ///< Maximum number of lights supported.
        uint16_t shadowNum;                     ///< Number of shadow-casting lights.

        int locsLightModelShader[2];            ///< Shader locations for light model properties.
        int locUseNormalMapModelShader;         ///< Shader location for the use of normal maps.
        int locUseSpecularMapModelShader;       ///< Shader location for the use of specular maps.

        Color ambient;                          ///< Ambient color of the scene lighting.

      private:
        /**
         * @brief Load the model shader from vertex and fragment shader sources.
         * @param vert The vertex shader source code.
         * @param frag The fragment shader source code.
         * @param isData Flag indicating whether the shader sources are stored in data or files.
         */
        void LoadModelShader(const char* vert, const char* frag, bool isData);

        /**
         * @brief Load shaders for the lights.
         * @param vertModel Vertex shader source code for light models.
         * @param fragModel Fragment shader source code for light models.
         * @param isData Flag indicating whether the shader sources are stored in data or files.
         */
        void Load(const char* vertModel, const char* fragModel, bool isData);

      public:
        /**
         * @brief Enumerations for shader locations related to model shader properties.
         */
        enum LocsModelShader {
            LOC_AMBIENT_COLOR,                  ///< Ambient color location.
            LOC_TEXEL_MAP_SIZE                  ///< Texel map size location.
        };

        /**
         * @brief Constructor for the Lights class.
         * @param _ambient Ambient color of the scene lighting.
         * @param maxLights Maximum number of lights supported.
         * @param bufferSize Size of the buffer used for shadow maps.
         * @param vertModel Vertex shader source code for light models.
         * @param fragModel Fragment shader source code for light models.
         * @param isData Flag indicating whether the shader sources are stored in data or files.
         */
        Lights(Color _ambient = { 25, 25, 25, 0 }, uint16_t maxLights = 1, uint16_t bufferSize = 1024, const char* vertModel = nullptr, const char* fragModel = nullptr, bool isData = true);

        /**
         * @brief Constructor for the Lights class.
         * @param _ambient Ambient intensity of the scene lighting.
         * @param maxLights Maximum number of lights supported.
         * @param bufferSize Size of the buffer used for shadow maps.
         * @param vertModel Vertex shader source code for light models.
         * @param fragModel Fragment shader source code for light models.
         * @param isData Flag indicating whether the shader sources are stored in data or files.
         */
        Lights(float _ambient = 0.1f, uint16_t maxLights = 1, uint16_t bufferSize = 1024, const char* vertModel = nullptr, const char* fragModel = nullptr, bool isData = true);

        /**
         * @brief Destructor for the Lights class.
         */
        ~Lights();

        /**
         * @brief Add a light that does not cast shadows.
         * @param caster Camera representing the light source.
         * @param radius Radius of the light influence.
         * @param color Color of the light.
         * @return Pointer to the added light.
         */
        Light* AddLight(const Camera& caster, float radius = 512, const Color& color = WHITE);

        /**
         * @brief Add a light that casts shadows.
         * @param caster Camera representing the light source.
         * @param radius Radius of the light influence.
         * @param color Color of the light.
         * @return Pointer to the added light.
         */
        Light* AddShadowLight(const Camera& caster, float radius = 512, const Color& color = WHITE);

        /**
         * @brief Update the lights according to the user's camera.
         * @param camera Camera representing the user's viewpoint.
         */
        void Update(const Camera& camera);

        /**
         * @brief Render models from each light's perspective to write to the depth buffer.
         * @param model Model to render.
         * @param position Model position.
         * @param rotationAxis Model rotation axis.
         * @param rotationAngle Model rotation angle.
         * @param scale Model scale.
         * @param tint Model tint color.
         */
        void DrawDepth(const raylib::Model& model, const Vector3& position = {}, const Vector3& rotationAxis = {}, float rotationAngle = 0, const Vector3& scale = { 1, 1, 1 }, const Color& tint = WHITE);

        /**
         * @brief Final rendering of models with the light model shader.
         * @param model Model to render.
         * @param position Model position.
         * @param rotationAxis Model rotation axis.
         * @param rotationAngle Model rotation angle.
         * @param scale Model scale.
         * @param tint Model tint color.
         */
        void Draw(raylib::Model& model, const Vector3& position = {}, const Vector3& rotationAxis = {}, float rotationAngle = 0, const Vector3& scale = { 1, 1, 1 }, const raylib::Color& tint = WHITE);

        /**
         * @brief Draw the shadow map for debugging purposes.
         * @param shader Shader to use for drawing the shadow map.
         * @param x X-coordinate of the debug display.
         * @param y Y-coordinate of the debug display.
         * @param w Width of the debug display.
         * @param h Height of the debug display.
         */
        void DrawMap(raylib::Shader& shader, float x = 0, float y = 0, float w = 256, float h = 256);
    };

}}

#endif //RAYFLEX_GFX_3D_LIGHTS_HPP
