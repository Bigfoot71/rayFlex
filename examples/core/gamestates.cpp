#include <rayflex.hpp>

using namespace rf;

class Title : public core::State
{
  public:
    raylib::Text text = { "This is the title state", 32, WHITE };

  public:
    void Update(float dt) override
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            app->Transition("game", 0.5f);
        }
    }

    void Draw() override
    {
        DrawRectangle(0, 0, app->renderer.GetWidth(), app->renderer.GetHeight(), GREEN);
        text.Draw((app->renderer.GetSize() - text.MeasureEx()) * 0.5f, 0);
    }
};

class Game : public core::State
{
  public:
    raylib::Text text = { "This is the game state", 32, WHITE };

  public:
    void Update(float dt) override
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            app->Transition("title", 0.5f);
        }
    }

    void Draw() override
    {
        DrawRectangle(0, 0, app->renderer.GetWidth(), app->renderer.GetHeight(), BLUE);
        text.Draw((app->renderer.GetSize() - text.MeasureEx()) * 0.5f, 0);
    }
};

int main()
{
    core::App app("Core - Game States", 800, 600, 60, FLAG_WINDOW_RESIZABLE);
    app.AddState<Title>("title");
    app.AddState<Game>("game");
    return app.Run("title");
}
