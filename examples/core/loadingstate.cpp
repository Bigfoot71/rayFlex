#include <rayflex.hpp>

using namespace rf;

class Loading : public core::LoadingState
{
  public:
    float progress;

  public:
    void Task() override
    {
        progress = 0.0f;
        while (progress < 1.0f)
        {
            progress += GetFrameTime() * 0.25f;
            WaitTime(GetFrameTime());
        }
    }

    void Draw() override
    {
        raylib::Color(WHITE).DrawText("Loading...", 100, 100, 32);
        DrawCircleSector({ 400, 300 }, 32, 0, 360 * progress, 36, raylib::Color(Vector3{360 * progress, 1.0f, 1.0f}));
        DrawCircleSectorLines({ 400, 300 }, 64, 0, 360 * progress, 36, raylib::Color(Vector3{360 * progress, 1.0f, 1.0f}));
    }
};

class Game : public core::State
{
  public:
    raylib::Text txt1 = { "This is the game state", 32, WHITE };
    raylib::Text txt2 = { "Press SPACE to restart loading", 24, WHITE };

  public:
    void Update(float dt) override
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            app->Loading<Loading>();
        }
    }

    void Draw() override
    {
        DrawRectangle(0, 0, app->renderer.GetWidth(), app->renderer.GetHeight(), BLUE);
        txt1.Draw((app->renderer.GetSize() - txt1.MeasureEx()) * 0.5f, 0);
        txt2.Draw({ (app->renderer.GetWidth() - txt2.MeasureEx().x) * 0.5f, app->renderer.GetHeight() * 0.5f + 32 }, 0);
    }
};

int main()
{
    core::App app("Core - Loading State", 800, 600, FLAG_WINDOW_RESIZABLE);
    app.AddState<Game>("game");

    app.Loading<Loading>();
    return app.Run("game");
}
