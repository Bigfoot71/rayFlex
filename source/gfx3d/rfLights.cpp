#include "gfx3d/rfLights.hpp"
#include <cstring>
#include <string>
#include <rlgl.h>

using namespace rf;

gfx3d::ShadowMap::ShadowMap(int _width, int _height)
: width(_width), height(_height)
{
    id = rlLoadFramebuffer(width, height);
    rlEnableFramebuffer(id);
    
    depth.id = rlLoadTextureDepth(width, height, false);
    depth.width = width;
    depth.height = height;
    depth.format = 19;
    depth.mipmaps = 1;

    rlTextureParameters(depth.id, RL_TEXTURE_WRAP_S, RL_TEXTURE_WRAP_CLAMP);
    rlTextureParameters(depth.id, RL_TEXTURE_WRAP_T, RL_TEXTURE_WRAP_CLAMP);
    rlFramebufferAttach(id, depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

    rlDisableFramebuffer();

#ifdef PLATFORM_DESKTOP
    // Set bilinear filter to depth texture
    rlTextureParameters(depth.id, RL_TEXTURE_MIN_FILTER, RL_TEXTURE_FILTER_LINEAR);
    rlTextureParameters(depth.id, RL_TEXTURE_MAG_FILTER, RL_TEXTURE_FILTER_LINEAR);
#endif
}

gfx3d::ShadowMap::~ShadowMap()
{
    rlUnloadTexture(depth.id);
    rlUnloadFramebuffer(id);
    id = depth.id = 0;
}

void gfx3d::ShadowMap::BeginMode(const Rectangle& bounds) const
{
    rlDrawRenderBatchActive();
    rlEnableFramebuffer(id);

    // Defini la zone du tampon où il faudra travaillé
    rlEnableScissorTest();
    rlScissor(bounds.x, bounds.y, bounds.width, bounds.height);

    // Set viewport to framebuffer size
    rlViewport(bounds.x, bounds.y, bounds.width, bounds.height);

    rlMatrixMode(RL_PROJECTION);       // Switch to projection matrix
    rlLoadIdentity();                       // Reset current matrix (projection)

    // Set orthographic projection to current framebuffer size
    // NOTE: Configured top-left corner as (0, 0)
    rlOrtho(0, bounds.width, bounds.height, 0, 0, 1);

    rlMatrixMode(RL_MODELVIEW);        // Switch back to modelview matrix
    rlLoadIdentity();                       // Reset current matrix (modelview)

    // Disable front face rendering (to render back faces)
    // This is done to prevent Peter Panning due to bias and avoid acne
    // https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
    rlSetCullFace(RL_CULL_FACE_FRONT);

    rlDisableColorBlend();
}

void gfx3d::ShadowMap::EndMode() const
{
    rlEnableColorBlend();
    rlDisableTexture();

    rlDisableScissorTest();

    rlDrawRenderBatchActive();
    rlDisableFramebuffer();

    rlViewport(0, 0, GetScreenWidth(), GetScreenHeight());

    rlMatrixMode(RL_PROJECTION);
    rlLoadIdentity();

    rlOrtho(0, GetScreenWidth(), GetScreenHeight(), 0, 0, 1);

    rlMatrixMode(RL_MODELVIEW);
    rlLoadIdentity();

    // Reinitialise le backface culling
    rlSetCullFace(RL_CULL_FACE_BACK);
}

void gfx3d::ShadowMap::Clear() const
{
    rlDrawRenderBatchActive();
    rlEnableFramebuffer(id);
    rlViewport(0, 0, width, height);
    rlClearColor(255, 255, 255, 255);   // Clear la tampon en blanc (max distance)
    rlClearScreenBuffers();
    rlDrawRenderBatchActive();
    rlDisableFramebuffer();
    rlViewport(0, 0, GetScreenWidth(), GetScreenHeight());
}

void gfx3d::ShadowMap::Draw(raylib::Shader& shader, float x, float y, float w, float h)
{
/*  Shadow map rendering shader example

    "#version 330\n"
    "in vec2 fragTexCoord;"
    "out vec4 finalColor;"
    "uniform sampler2D texture0;"
    "const float nearClip = 0.1;"
    "const float farClip = 1000.0;"
    "float LinearizeDepth(float depth)"
    "{"
        "float z = depth * 2.0 - 1.0;"
        "return (2.0 * nearClip * farClip) / (farClip + nearClip - z * (farClip - nearClip));"
    "}"
    "void main()"
    "{"
        "vec2 uv = vec2(fragTexCoord.x, 1.0 - fragTexCoord.y);"
        "float depth = LinearizeDepth(texture(texture0, uv).r);"
        "finalColor.rgb = vec3(depth) / farClip;"
        "finalColor.a = 1.0;"
    "}"

*/
    shader.BeginMode();
        DrawTexturePro(depth,
            { 0, 0, static_cast<float>(depth.width), static_cast<float>(depth.height) },
            { x, y, w, h }, { 0, 0 },
            0, WHITE);
    shader.EndMode();
}

/* LIGHT(s) IMPLEMENTATION */

/** 
    TODO: Reorder the light shader (model shader) to be able to use the transpose of the TBN matrix 
          to calculate viewDir and lightDir in tangent space. This way, we can obtain them only in the
          vertex shader and use them as they are, instead of using TBN to calculate normals from the
          normalMap in world space for each texel in the fragment shader.

          Consequently, we can rely on interpolation in the fragment shader for viewDir and lightDir. 
          This would save resources, but it requires restructuring and rethinking the logic for cases 
          where there is no normalMap.

          See: https://learnopengl.com/Advanced-Lighting/Normal-Mapping
*/

#if defined(PLATFORM_DESKTOP)

    // SHADOW SHADER

    constexpr char vertShadow[] =
        "#version 330\n"
        "in vec3 vertexPosition;"
        "uniform mat4 mvp;"
        "void main()"
        "{"
            "gl_Position = mvp * vec4(vertexPosition, 1.0);"
        "}";

    constexpr char fragShadow[] =
        "#version 330\n"
        "void main()"
        "{"
            "gl_FragDepth = gl_FragCoord.z;"
        "}";

    // LIGHT SHADER

    constexpr char vertLight[] =
        "#version 330\n"
        "#define MAX_LIGHTS %i\n"
        "struct Light {"
            "mat4 matrix;"
            "vec4 mapBounds;"
            "vec3 position;"
            "vec3 direction;"
            "vec3 color;"
            "float cutoff;"
            "float radius;"
            "int shadow;"
            "int enabled;"
        "};"
        "in vec3 vertexPosition;"
        "in vec2 vertexTexCoord;"
        "in vec3 vertexNormal;"
        "in vec4 vertexTangent;"
        "uniform int useSpecularMap;"
        "uniform int useNormalMap;"
        "uniform mat4 mvp;"
        "uniform mat4 matModel;"
        "uniform mat4 matNormal;"
        "uniform Light lights[MAX_LIGHTS];"
        "flat out int fragUseSpecularMap;"
        "flat out int fragUseNormalMap;"
        "out vec3 fragPosition;"
        "out vec3 fragNormal;"
        "out vec2 fragTexCoord;"
        "out mat3 fragTBN;"                     // TBN: For calculating normals in world space from the normalMap
        "out vec4 shadowPos[MAX_LIGHTS];"
        "const float normalOffset = 0.1;"
        "mat3 GetMatTBN()"
        "{"
            "vec3 T = vec3(vertexTangent);"
            "vec3 B = cross(vertexNormal, T);"
            "vec3 N = vertexNormal;"
            "return mat3(T, B, N);"
        "}"
        "void main()"
        "{"
            "fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));"
            "fragNormal = normalize(vec3(matNormal * vec4(vertexNormal, 0.0)));"
            "for (int i = 0; i < MAX_LIGHTS; i++)"
            "{"
                "if (lights[i].enabled == 1 && lights[i].shadow == 1)"
                "{"
                    "vec3 lightDir = normalize(lights[i].position - fragPosition);"
                    "float cosAngle = clamp(1.0 - dot(lightDir, fragNormal), 0, 1);"
                    "vec3 scaledNormalOffset = fragNormal * (normalOffset * cosAngle);"
                    "shadowPos[i] = lights[i].matrix * vec4(fragPosition + scaledNormalOffset, 1.0);"
                "}"
            "}"
            "fragUseSpecularMap = useSpecularMap;"
            "fragUseNormalMap = useNormalMap;"
            "fragTexCoord = vertexTexCoord;"
            "if (useNormalMap == 1) fragTBN = GetMatTBN();"
            "gl_Position = mvp * vec4(vertexPosition, 1.0);"
        "}";

    constexpr char fragLight[] =
        "#version 330\n"
        "#define MAX_LIGHTS %i\n"
        "struct Light {"
            "mat4 matrix;"
            "vec4 mapBounds;"
            "vec3 position;"
            "vec3 direction;"
            "vec3 color;"
            "float cutoff;"
            "float radius;"
            "int shadow;"
            "int enabled;"
        "};"
        "flat in int fragUseSpecularMap;"
        "flat in int fragUseNormalMap;"
        "in vec3 fragPosition;"
        "in vec3 fragNormal;"
        "in vec2 fragTexCoord;"
        "in mat3 fragTBN;"                  // TBN: For calculating normals in world space from the normalMap
        "in vec4 shadowPos[MAX_LIGHTS];"
        "uniform Light lights[MAX_LIGHTS];"
        "uniform vec4 colDiffuse;"
        "uniform sampler2D texture0;"       // DIFFUSE
        "uniform sampler2D texture1;"       // SPECULAR
        "uniform sampler2D texture2;"       // NORMAL
        "uniform float texelMapSize;"
        "uniform sampler2D shadowMap;"
        "uniform vec3 viewPos;"
        "uniform vec3 ambient;"
        "const float spotSoftness = 0.65;"
        "out vec4 finalColor;"
        "float ShadowCalc(int i, vec4 p, float bias)"
        "{"
            "vec3 projCoords = p.xyz / p.w;"
            "projCoords = projCoords * 0.5 + 0.5;"
            "projCoords.xy *= lights[i].mapBounds.zw;"
            "projCoords.xy += lights[i].mapBounds.xy;"
            "projCoords.z = projCoords.z - bias;"
            "float depth = projCoords.z;"
            "float shadow = 0.0;"
            "for (int x = -1; x <= 1; ++x)"
            "{"
                "for (int y = -1; y <= 1; ++y)"
                "{"
                    "float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelMapSize).r; "
                    "shadow += step(depth, pcfDepth);"
                "}"
            "}"
            "shadow /= 9.0;"
            "if (projCoords.z > 1.0 || projCoords.x > 1.0 || projCoords.y > 1.0)"
            "{"
                "shadow = 1.0;"
            "}"
            "return shadow;"
        "}"
        "void main()"
        "{"
            "vec3 texelDiffuse = texture(texture0, fragTexCoord).rgb * colDiffuse.rgb;"
            "vec3 texelSpecular = (fragUseSpecularMap == 1) ? texture(texture1, fragTexCoord).rgb : vec3(1.0);"
            "vec3 normal = (fragUseNormalMap == 1) ? normalize(fragTBN * (2.0 * texture(texture2, fragTexCoord).rgb - 1.0)) : fragNormal;"
            "vec3 resultColor = vec3(0.0);"
            "for (int i = 0; i < MAX_LIGHTS; i++)"
            "{"
                "if (lights[i].enabled != 1) continue;"
                "vec3 lightRaw = lights[i].position - fragPosition;"
                "vec3 lightDir = normalize(lightRaw);"
                "float lightDistSqr = dot(lightRaw, lightRaw);"
                "vec3 viewDir = normalize(viewPos - fragPosition);"
                "vec3 h = normalize(lightDir + viewDir);"
                "float attenuation = clamp(1.0 - lightDistSqr / (lights[i].radius * lights[i].radius), 0, 1);"
                "attenuation *= attenuation;"
                "float NdL = max(0.0, dot(lightDir, normal));"
                "float NdH = max(0.0, dot(h, normal));"
                "float theta = dot(-lightDir, normalize(lights[i].direction));"
                "float epsilon = lights[i].cutoff - spotSoftness;"
                "float spot = clamp((theta - lights[i].cutoff) / epsilon, 0.0, 1.0);"
                "float diff = NdL * attenuation;"
                "if (lights[i].shadow == 1)"
                "{"
                    "diff *= ShadowCalc(i, shadowPos[i], 0.0) * spot;"
                "}"
                "vec3 lightColor = texelDiffuse * lights[i].color;"                             // diffuse * lightColor
                "vec3 specularity = texelSpecular * (pow(NdH, 64.0) * diff);"                   // specular * specIntensity
                "resultColor += (lightColor * diff + specularity) + (lightColor * ambient);"
            "}"
            "finalColor = vec4(resultColor, 1.0);"
        "}";

#else

    // SHADOW SHADER

    constexpr char vertShadow[] =
        "#version 100\n"
        "attribute vec3 vertexPosition;"
        "uniform mat4 mvp;"
        "void main()"
        "{"
            "gl_Position = mvp * vec4(vertexPosition, 1.0);"
        "}";

    constexpr char fragShadow[] =
        "#version 100\n"
        "void main()"
        "{"
            "gl_FragColor = vec4(vec3(gl_FragCoord.z), 1.0);"
        "}";

    // LIGHT SHADER

    constexpr char vertLight[] =
        "#version 100\n"
        "#define MAX_LIGHTS %i\n"
        "struct Light {"
            "mat4 matrix;"
            "vec4 mapBounds;"
            "vec3 position;"
            "vec3 direction;"
            "vec3 color;"
            "float cutoff;"
            "float radius;"
            "int shadow;"
            "int enabled;"
        "};"
        "attribute vec3 vertexPosition;"
        "attribute vec2 vertexTexCoord;"
        "attribute vec3 vertexNormal;"
        "attribute vec4 vertexTangent;"
        "uniform int useSpecularMap;"
        "uniform int useNormalMap;"
        "uniform mat4 mvp;"
        "uniform mat4 matModel;"
        "uniform mat4 matNormal;"
        "uniform Light lights[MAX_LIGHTS];"
        "flat varying int fragUseSpecularMap;"
        "flat varying int fragUseNormalMap;"
        "varying vec3 fragPosition;"
        "varying vec3 fragNormal;"
        "varying vec2 fragTexCoord;"
        "varying mat3 fragTBN;"                     // TBN: For calculating normals in world space from the normalMap
        "varying vec4 shadowPos[MAX_LIGHTS];"
        "const float normalOffset = 0.1;"
        "mat3 GetMatTBN()"
        "{"
            "vec3 T = vec3(vertexTangent);"
            "vec3 B = cross(vertexNormal, T);"
            "vec3 N = vertexNormal;"
            "return mat3(T, B, N);"
        "}"
        "void main()"
        "{"
            "fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));"
            "fragNormal = normalize(vec3(matNormal * vec4(vertexNormal, 0.0)));"
            "for (int i = 0; i < MAX_LIGHTS; i++)"
            "{"
                "if (lights[i].enabled == 1 && lights[i].shadow == 1)"
                "{"
                    "vec3 lightDir = normalize(lights[i].position - fragPosition);"
                    "float cosAngle = clamp(1.0 - dot(lightDir, fragNormal), 0, 1);"
                    "vec3 scaledNormalOffset = fragNormal * (normalOffset * cosAngle);"
                    "shadowPos[i] = lights[i].matrix * vec4(fragPosition + scaledNormalOffset, 1.0);"
                "}"
            "}"
            "fragUseSpecularMap = useSpecularMap;"
            "fragUseNormalMap = useNormalMap;"
            "fragTexCoord = vertexTexCoord;"
            "if (useNormalMap == 1) fragTBN = GetMatTBN();"
            "gl_Position = mvp * vec4(vertexPosition, 1.0);"
        "}";

    constexpr char fragLight[] =
        "#version 100\n"
        "#define MAX_LIGHTS %i\n"
        "struct Light {"
            "mat4 matrix;"
            "vec4 mapBounds;"
            "vec3 position;"
            "vec3 direction;"
            "vec3 color;"
            "float cutoff;"
            "float radius;"
            "int shadow;"
            "int enabled;"
        "};"
        "flat varying int fragUseSpecularMap;"
        "flat varying int fragUseNormalMap;"
        "varying vec3 fragPosition;"
        "varying vec3 fragNormal;"
        "varying vec2 fragTexCoord;"
        "varying mat3 fragTBN;"             // For calculating normals in world space from the normalMap
        "varying vec4 shadowPos[MAX_LIGHTS];"
        "uniform Light lights[MAX_LIGHTS];"
        "uniform vec4 colDiffuse;"
        "uniform sampler2D texture0;"       // DIFFUSE
        "uniform sampler2D texture1;"       // SPECULAR
        "uniform sampler2D texture2;"       // NORMAL
        "uniform float texelMapSize;"
        "uniform sampler2D shadowMap;"
        "uniform vec3 viewPos;"
        "uniform vec3 ambient;"
        "const float spotSoftness = 0.65;"
        "float ShadowCalc(int i, vec4 p, float bias)"
        "{"
            "vec3 projCoords = p.xyz / p.w;"
            "projCoords = projCoords * 0.5 + 0.5;"
            "projCoords.xy *= lights[i].mapBounds.zw;"
            "projCoords.xy += lights[i].mapBounds.xy;"
            "projCoords.z = projCoords.z - bias;"
            "float depth = projCoords.z;"
            "float shadow = 0.0;"
            "for (int x = -1; x <= 1; ++x)"
            "{"
                "for (int y = -1; y <= 1; ++y)"
                "{"
                    "float pcfDepth = texture2D(shadowMap, projCoords.xy + vec2(x, y) * texelMapSize).r; "
                    "shadow += step(depth, pcfDepth);"
                "}"
            "}"
            "shadow /= 9.0;"
            "if (projCoords.z > 1.0 || projCoords.x > 1.0 || projCoords.y > 1.0)"
            "{"
                "shadow = 1.0;"
            "}"
            "return shadow;"
        "}"
        "void main()"
        "{"
            "vec3 texelDiffuse = texture2D(texture0, fragTexCoord).rgb * colDiffuse.rgb;"
            "vec3 texelSpecular = (fragUseSpecularMap == 1) ? texture2D(texture1, fragTexCoord).rgb : vec3(1.0);"
            "vec3 normal = (fragUseNormalMap == 1) ? normalize(fragTBN * (2.0 * texture2D(texture2, fragTexCoord).rgb - 1.0)) : fragNormal;"
            "vec3 resultColor = vec3(0.0);"
            "for (int i = 0; i < MAX_LIGHTS; i++)"
            "{"
                "if (lights[i].enabled != 1) continue;"
                "vec3 lightRaw = lights[i].position - fragPosition;"
                "vec3 lightDir = normalize(lightRaw);"
                "float lightDistSqr = dot(lightRaw, lightRaw);"
                "vec3 viewDir = normalize(viewPos - fragPosition);"
                "vec3 h = normalize(lightDir + viewDir);"
                "float attenuation = clamp(1.0 - lightDistSqr / (lights[i].radius * lights[i].radius), 0, 1);"
                "attenuation *= attenuation;"
                "float NdL = max(0.0, dot(lightDir, normal));"
                "float NdH = max(0.0, dot(h, normal));"
                "float theta = dot(-lightDir, normalize(lights[i].direction));"
                "float epsilon = lights[i].cutoff - spotSoftness;"
                "float spot = clamp((theta - lights[i].cutoff) / epsilon, 0.0, 1.0);"
                "float diff = NdL * attenuation;"
                "if (lights[i].shadow == 1)"
                "{"
                    "diff *= ShadowCalc(i, shadowPos[i], 0.0) * spot;"
                "}"
                "vec3 lightColor = texelDiffuse * lights[i].color;"                             // diffuse * lightColor
                "vec3 specularity = texelSpecular * (pow(NdH, 64.0) * diff);"                   // specular * specIntensity
                "resultColor += (lightColor * diff + specularity) + (lightColor * ambient);"
            "}"
            "gl_FragColor = vec4(resultColor, 1.0);"
        "}";

#endif

// LIGHT - PRIVATE //

gfx3d::Light::Light(raylib::Shader& _modelShader, ShadowMap* _shadowMap, uint16_t lightNum, const gfx3d::Camera& _caster, float _radius, const Color& _color)
: modelShader(_modelShader)
, shadowMap(_shadowMap)
, boundsMap({0,0,0,0})
, caster(_caster)
, radius(_radius)
, color(_color)
, enabled(true)
{
    caster.aspect = 1.0f;

    std::string uniform("lights[" + std::to_string(lightNum) + "].");

	locsLightModelShader[LOC_LIGHT_MAT] = modelShader.GetLocation(uniform+"matrix");
	locsLightModelShader[LOC_LIGHT_POS] = modelShader.GetLocation(uniform+"position");
	locsLightModelShader[LOC_LIGHT_DIR] = modelShader.GetLocation(uniform+"direction");
    locsLightModelShader[LOC_LIGHT_COLOR] = modelShader.GetLocation(uniform+"color");
    locsLightModelShader[LOC_LIGHT_CUTOFF] = modelShader.GetLocation(uniform+"cutoff");
    locsLightModelShader[LOC_LIGHT_RADIUS] = modelShader.GetLocation(uniform+"radius");
    locsLightModelShader[LOC_LIGHT_BOUNDS] = modelShader.GetLocation(uniform+"mapBounds");
    locsLightModelShader[LOC_LIGHT_SHADOW] = modelShader.GetLocation(uniform+"shadow");
    locsLightModelShader[LOC_LIGHT_ENABLED] = modelShader.GetLocation(uniform+"enabled");

    int shadow = _shadowMap != nullptr ? 1 : 0;
    modelShader.SetValue(locsLightModelShader[LOC_LIGHT_SHADOW], &shadow, SHADER_UNIFORM_INT);

    this->SetPosition(caster.position, false);
    this->SetTarget(caster.target, false);
    this->SetFovY(caster.fovy, false);

    this->UpdateMatrix();

    this->SetRadius(radius);
    this->SetColor(color);
    this->SetActive(true);
}

void gfx3d::Light::SetShadowMapBounds(const Rectangle& bounds)
{
    boundsMap = bounds;

    const float nBox[4] = {
        bounds.x / shadowMap->width,
        bounds.y / shadowMap->height,
        bounds.width / shadowMap->width,
        bounds.height / shadowMap->height
    };

    modelShader.SetValue(
        locsLightModelShader[LOC_LIGHT_BOUNDS],
        nBox, SHADER_UNIFORM_VEC4);
}

void gfx3d::Light::BeginDepthMode()
{
    shadowMap->BeginMode(boundsMap);
    caster.BeginMode();
}

void gfx3d::Light::EndDepthMode()
{
    caster.EndMode();
    shadowMap->EndMode();
}

void gfx3d::Light::ClearBuffer()
{
    shadowMap->Clear();
}

// LIGHT - PUBLIC //

Vector3 gfx3d::Light::GetPosition()
{
    return caster.position;
}

Vector3 gfx3d::Light::GetTarget()
{
    return caster.target;
}

float gfx3d::Light::GetFovY()
{
    return caster.fovy;
}

float gfx3d::Light::GetRadius()
{
    return radius;
}

Color gfx3d::Light::GetColor()
{
    return color;
}

bool gfx3d::Light::IsActive()
{
    return enabled;
}

void gfx3d::Light::UpdateMatrix()
{
    modelShader.SetValue(locsLightModelShader[LOC_LIGHT_MAT], MatrixMultiply(caster.GetViewMatrix(), caster.GetProjectionMatrix()));
}

void gfx3d::Light::SetPosition(const Vector3& position, bool updateMatrix)
{
    caster.position = position;
    Vector3 casterDirection = caster.GetDirection();
    modelShader.SetValue(locsLightModelShader[LOC_LIGHT_POS], reinterpret_cast<float*>(&caster.position), SHADER_UNIFORM_VEC3);
    modelShader.SetValue(locsLightModelShader[LOC_LIGHT_DIR], reinterpret_cast<float*>(&casterDirection), SHADER_UNIFORM_VEC3);
    if (updateMatrix) this->UpdateMatrix();
}

void gfx3d::Light::SetTarget(const Vector3& target, bool updateMatrix)
{
    caster.target = target;
    Vector3 casterDirection = caster.GetDirection();
    modelShader.SetValue(locsLightModelShader[LOC_LIGHT_DIR], reinterpret_cast<float*>(&casterDirection), SHADER_UNIFORM_VEC3);
    if (updateMatrix) this->UpdateMatrix();
}

void gfx3d::Light::SetFovY(float fovy, bool updateMatrix)
{
    caster.fovy = fovy;
    float cutoff = std::cos(DEG2RAD * fovy * 0.46f);
    modelShader.SetValue(locsLightModelShader[LOC_LIGHT_CUTOFF], &cutoff, SHADER_UNIFORM_FLOAT);
    if (updateMatrix) this->UpdateMatrix();
}

void gfx3d::Light::SetRadius(float radius)
{
    this->radius = radius;
    modelShader.SetValue(locsLightModelShader[LOC_LIGHT_RADIUS], &radius, SHADER_UNIFORM_FLOAT);
}

void gfx3d::Light::SetColor(const Color& color)
{
    this->color = color;
    Vector4 _color = raylib::Color(color).Normalize();
    modelShader.SetValue(locsLightModelShader[LOC_LIGHT_COLOR], reinterpret_cast<float*>(&_color), SHADER_UNIFORM_VEC3);
}

void gfx3d::Light::SetActive(bool active)
{
    enabled = active;
    int _enabled = active ? true : false;
    modelShader.SetValue(locsLightModelShader[LOC_LIGHT_ENABLED], &_enabled, SHADER_UNIFORM_INT);
}

void gfx3d::Light::SetCaster(const Camera& caster)
{
    this->caster = caster;
    SetPosition(caster.position, false);
    SetTarget(caster.target, false);
    SetFovY(caster.fovy, false);
    UpdateMatrix();
}

// LIGHTS - PUBLIC //

gfx3d::Lights::Lights(Color _ambient, uint16_t _maxLights, uint16_t _mapSize, const char* vertModel, const char* fragModel, bool isData)
: shadowShader(0), modelShader(0)
, bufferSize(_maxLights*_mapSize)
, mapSize(_mapSize)
, maxLights(_maxLights)
, shadowNum(0)
, ambient(_ambient)
{
    this->Load(vertModel, fragModel, isData);
}

gfx3d::Lights::Lights(float _ambient, uint16_t _maxLights, uint16_t _mapSize, const char* vertModel, const char* fragModel, bool isData)
: shadowShader(0), modelShader(0)
, bufferSize(_maxLights*_mapSize)
, mapSize(_mapSize)
, maxLights(_maxLights)
, shadowNum(0)
, ambient({
    static_cast<uint8_t>(_ambient*255),
    static_cast<uint8_t>(_ambient*255),
    static_cast<uint8_t>(_ambient*255),
    0})
{
    this->Load(vertModel, fragModel, isData);
}

void gfx3d::Lights::LoadModelShader(const char* vert, const char* frag, bool isData)
{
    if ((vert || frag) && !isData)
    {
        modelShader = raylib::Shader::Load(vert, frag);
        return;
    }

    const char *vsModel = vert == nullptr ? vertLight : vert;
    const char *fsModel = frag == nullptr ? fragLight : frag;

    const unsigned int vsLen = std::strlen(vsModel)+1;
    const unsigned int fsLen = std::strlen(fsModel)+1;

    char vsFormated[vsLen], fsFormated[fsLen];

    std::snprintf(vsFormated, vsLen, vsModel, maxLights);
    std::snprintf(fsFormated, fsLen, fsModel, maxLights);

    modelShader = raylib::Shader::LoadFromMemory(vsFormated, fsFormated);
}

void gfx3d::Lights::Load(const char* vertModel, const char* fragModel, bool isData)
{
    // Pré-allocation pour les lumières
    sources.reserve(maxLights);

    // Création de la shadow map
    if (bufferSize > 0) shadowMap = new ShadowMap(bufferSize, bufferSize);
    else shadowMap = nullptr;

    // Chargement du shaders de profondeur (shadow) et de model
    if (shadowMap) shadowShader = raylib::Shader::LoadFromMemory(vertShadow, fragShadow);
    LoadModelShader(vertModel, fragModel, isData);

    // Recuperation des locations de vue et de shadow map (TODO: ajouté shadow map à raylib ?)
	modelShader.locs[SHADER_LOC_VECTOR_VIEW] = modelShader.GetLocation("viewPos");
    modelShader.locs[SHADER_LOC_MAP_HEIGHT] = modelShader.GetLocation("shadowMap");

    // Recuperation des locations uniques
    locsLightModelShader[LOC_AMBIENT_COLOR] = modelShader.GetLocation("ambient");
    locsLightModelShader[LOC_TEXEL_MAP_SIZE] = modelShader.GetLocation("texelMapSize");

    // Recuperation des locations de permission d'utilisation des material samplers
    locUseSpecularMapModelShader = modelShader.GetLocation("useSpecularMap");
    locUseNormalMapModelShader = modelShader.GetLocation("useNormalMap");

    // Application de la lumiere d'ambiance
    Vector4 ambientCol = raylib::Color(ambient).Normalize();
    modelShader.SetValue(locsLightModelShader[LOC_AMBIENT_COLOR], reinterpret_cast<float*>(&ambientCol), SHADER_UNIFORM_VEC3);
}

gfx3d::Lights::~Lights()
{
    for (auto light : sources)
    {
        delete light;
    }
}

gfx3d::Light* gfx3d::Lights::AddLight(const gfx3d::Camera& caster, float radius, const Color& color)
{
    if (sources.size() < maxLights)
    {
        Light *light = new Light(modelShader, nullptr, static_cast<uint16_t>(sources.size()), caster, radius, color);
        sources.push_back(light);
        return light;
    }
    return nullptr;
}

gfx3d::Light* gfx3d::Lights::AddShadowLight(const gfx3d::Camera& caster, float radius, const Color& color)
{
    constexpr auto nearestUpperSquare = [](uint32_t x)
    {
        x--;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        x++;
        return x;
    };

    if (shadowMap == nullptr)
    {
        TraceLog(LOG_DEBUG, "Lumière avec ombre demandé sans shadow map.");
        return AddLight(caster, radius, color);
    }

    if (sources.size() < maxLights)
    {
        Light *light = new Light(modelShader, shadowMap, static_cast<uint16_t>(sources.size()), caster, radius, color);
        sources.push_back(light);

        float targetMapSize = static_cast<float>(bufferSize) / nearestUpperSquare(++shadowNum);
        Rectangle bounds = { 0, 0, targetMapSize, targetMapSize };

        for (auto src : sources)
        {
            src->SetShadowMapBounds(bounds);
            bounds.x += targetMapSize;
            if (bounds.x >= bufferSize)
            {
                bounds.y += targetMapSize;
                bounds.x = 0;
            }
        }

        float texelMapSize = 1.0f / targetMapSize;
        modelShader.SetValue(locsLightModelShader[LOC_TEXEL_MAP_SIZE], &texelMapSize, SHADER_UNIFORM_FLOAT);

        return light;
    }
    return nullptr;
}

void gfx3d::Lights::Update(const Camera& camera)
{
    modelShader.SetValue(
        modelShader.locs[SHADER_LOC_VECTOR_VIEW],
        reinterpret_cast<const float*>(&camera.position),
        SHADER_UNIFORM_VEC3);

    for (auto& light : sources)
    {
        light->ClearBuffer();
    }
}

void gfx3d::Lights::DrawDepth(const raylib::Model& model, const Vector3& position, const Vector3& rotationAxis, float rotationAngle, const Vector3& scale, const Color& tint)
{
    for (int i = 0; i < model.materialCount; i++)
    {
        model.materials[i].shader = shadowShader;
        model.materials[i].maps[MATERIAL_MAP_HEIGHT].texture = shadowMap->depth;
    }

    for (auto& light : sources)
    {
        light->BeginDepthMode();
            model.Draw(position, rotationAxis, rotationAngle, scale, tint);
        light->EndDepthMode();
    }
}

void gfx3d::Lights::Draw(raylib::Model& model, const Vector3& position, const Vector3& rotationAxis, float rotationAngle, const Vector3& scale, const raylib::Color& tint)
{
    raylib::Matrix matTransform = raylib::Matrix::Scale(scale.x, scale.y, scale.z)
                                * raylib::Matrix::Rotate(rotationAxis, rotationAngle*DEG2RAD)
                                * raylib::Matrix::Translate(position.x, position.y, position.z);

    matTransform = MatrixMultiply(model.transform, matTransform);

    for (int i = 0; i < model.meshCount; i++)
    {
        Material &material = model.materials[model.meshMaterial[i]];

        material.shader = modelShader;

        int useSpecularMap = material.maps[MATERIAL_MAP_SPECULAR].texture.id > 1 ? 1 : 0;
        int useNormalMap = material.maps[MATERIAL_MAP_NORMAL].texture.id > 1 ? 1 : 0;

        modelShader.SetValue(locUseSpecularMapModelShader, &useSpecularMap, SHADER_UNIFORM_INT);
        modelShader.SetValue(locUseNormalMapModelShader, &useNormalMap, SHADER_UNIFORM_INT);

        Color color = material.maps[MATERIAL_MAP_DIFFUSE].color;
        raylib::Color colorTint = raylib::Color(color).Normalize() * tint.Normalize();

        model.materials[model.meshMaterial[i]].maps[MATERIAL_MAP_DIFFUSE].color = colorTint;
        DrawMesh(model.meshes[i], model.materials[model.meshMaterial[i]], matTransform);
        model.materials[model.meshMaterial[i]].maps[MATERIAL_MAP_DIFFUSE].color = color;
    }
}
