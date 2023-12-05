#include "TextureUnmanaged.hpp"
#include "raylib.h"
#include <rayflex.hpp>

using namespace rf;

struct Character
{
    gfx2d::Sprite::Instance *inst;
    gfx2d::Sprite *sprite;

    raylib::Vector2 position;
    float speed, speedAnim;
    float dirX;

    Character(gfx2d::Sprite& _sprite, const std::string& _keyInstance, const std::string& _keyAnimation, core::RandomGenerator& gen)
    {
        inst = _sprite.NewInstance(_keyInstance, _keyAnimation);
        sprite = &_sprite;

        speed = GetRandomValue(100, 200);
        speedAnim = speed / 100;

        position = raylib::Vector2(gen.Random<float>(32, GetScreenWidth()-32), GetScreenHeight() - 32);
        dirX = GetRandomValue(0,1) ? -1 : 1;
    }

    void Update(const rf::core::Renderer& renderer, float dt)
    {
        sprite->Update(speedAnim * dt, inst);
        position.x += dirX * speed * dt;

        if (dirX > 0 && position.x + 16 > renderer.GetWidth())
        {
            position.x = renderer.GetWidth() - 16;
            dirX *= -1;
        }
        else if (dirX < 0 && position.x - 16 < 16)
        {
            position.x = 8;
            dirX *= -1;
        }
    }

    void Draw() const
    {
        sprite->Draw(position, 2*dirX, 2, 0, { 0.5f, 0.5f }, WHITE, inst);
    }
};

class Game : public core::State
{
  private:
    int counter = 0;
    gfx2d::Sprite sprite;
    core::RandomGenerator gen;
    gfx2d::ParticleSystem pSys;
    std::vector<Character> characters;

  public:
    void Enter() override
    {
        pSys = gfx2d::ParticleSystem(1024);
        pSys.SetRadius(2.0f, 4.0f);
        pSys.SetVelocity({-400, -400}, {400, 400});

        raylib::Texture2D* tex = app->assetManager.Get<raylib::Texture2D>("sprite");

        sprite.Load(tex, 4, 4, {
            0, 0, static_cast<float>(tex->GetWidth()), static_cast<float>(tex->GetHeight())
        });

        sprite.NewAnimation("A", 0, 3, 0.1f, true);
        sprite.NewAnimation("B", 4, 7, 0.1f, true);
        sprite.NewAnimation("C", 8, 11, 0.1f, true);
        sprite.NewAnimation("D", 12, 15, 0.1f, true);

        for (int i = 0; i < 32; i++)
        {
            char keyInstance = 'A' + i;
            characters.emplace_back(sprite,
                std::string(1, keyInstance),
                std::string(1, gen.RandomChar('A', 'D')),
                gen);
        }
    }

    void Update(const float dt) override
    {
        for (auto & character : characters)
        {
            character.Update(app->renderer, dt);
        }

        pSys.SetPosition(app->GetMousePosition());

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            pSys.Emit(4);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            for (int i = 0; i < 4; i++)
            {
                pSys.SetColor(gen.RandomColor());
                pSys.Emit();
            }
            pSys.SetColor(WHITE);
        }

        pSys.Update(dt);
    }

    void Draw() override
    {
        DrawRectangle(0, 0, app->renderer.GetWidth(), app->renderer.GetHeight(), DARKGRAY);
        for (const auto & character : characters) character.Draw();
        pSys.Draw();
    }
};

int main()
{
    core::App app("GFX 2D - Sprites & Particles", 800, 600, FLAG_WINDOW_RESIZABLE);
    app.AddState<Game>("game");

    app.assetManager.Add<raylib::Texture2D>("sprite",
        TextFormat("%s/../../../examples/resources/images/spritesheet.png", GetApplicationDirectory()));

    return app.Run("game");
}
