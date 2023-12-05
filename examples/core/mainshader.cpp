#include <rayflex.hpp>

using namespace rf;

class Game : public core::State
{
  public:
    void Update(float dt) override
    {
        *app->assetManager.Get<float>("shaderTime") += dt;

        app->assetManager.Get<raylib::Shader>("shader")->SetValue(
            *app->assetManager.Get<int>("locShaderTime"),
            app->assetManager.Get<float>("shaderTime"),
            SHADER_UNIFORM_FLOAT);
    }

    void Draw(const core::Renderer& target) override
    {
        target.Clear();
        constexpr char txt[] = "Hello World ! :D";
        raylib::Color(WHITE).DrawText(txt,
            (target.GetWidth() - MeasureText(txt, 64)) * 0.5f,
            (target.GetHeight() - 64) * 0.5f, 64);
    }
};

int main()
{
    core::App app("Core - Main Shader", 800, 600);

    raylib::Shader *shader = app.assetManager.Add<raylib::Shader>("shader", LoadShaderFromMemory(nullptr,
        R"(
            #version 330
            in vec2 fragTexCoord;
            in vec4 fragColor;
            uniform sampler2D texture0;
            uniform float time;
            out vec4 finalColor;
            vec3 hsv2rgb(vec3 c)
            {
                vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
                vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
                return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
            }
            void main()
            {
                vec2 uv = vec2(fragTexCoord.x, fragTexCoord.y + sin(time + fragTexCoord.x * 5.0) * 0.05);
                finalColor = texture(texture0, uv) * vec4(hsv2rgb(vec3((cos(2.0 * time + uv.y * 10.0) + 1.0) * 0.5, 1.0, 1.0)), 1.0);
            }
        )"
    )).first->second.Get<raylib::Shader>();

    app.assetManager.Add("locShaderTime", shader->GetLocation("time"));
    app.assetManager.Add("shaderTime", 0.0f);

    app.SetMainShader(shader);

    app.AddState<Game>("game");
    return app.Run("game");
}
