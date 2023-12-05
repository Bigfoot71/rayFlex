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

    void Draw(const core::Renderer& target) override
    {
        DrawRectangle(0, 0, target.GetWidth(), target.GetHeight(), GREEN);
        text.Draw((target.GetSize() - text.MeasureEx()) * 0.5f, 0);
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

    void Draw(const core::Renderer& target) override
    {
        target.Clear(BLUE);
        text.Draw((target.GetSize() - text.MeasureEx()) * 0.5f, 0);
    }
};

int main()
{
    core::App app("Core - Game States", 800, 600, FLAG_WINDOW_RESIZABLE);
    app.AddState<Title>("title");
    app.AddState<Game>("game");
    return app.Run("title");
}
