#include <rayflex.hpp>

using namespace rf;

class Box
{
  private:
    raylib::Rectangle rect;
    Vector2 rectOrigin;

  public:
    phys2d::Body *body;

    Box(phys2d::World* world, const Rectangle& _rect)
    : rect(_rect), rectOrigin(rect.GetSize() * 0.5f)
    {
        phys2d::BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(rect.x, rect.y);
        body = world->CreateBody(&bodyDef);

        phys2d::PolygonShape boxShape;
        boxShape.SetAsBox(rectOrigin.x, rectOrigin.y);

        // Défini le coefficient de restitution
        phys2d::FixtureDef fixtureDef;
        fixtureDef.shape = &boxShape;
        fixtureDef.density = 10.0f;
        fixtureDef.restitution = 0.5f;
        body->CreateFixture(&fixtureDef);
    }

    inline void Update()
    {
        phys2d::Vector2 position = body->GetPosition();
        rect.x = position.x, rect.y = position.y;
    }

    inline void Draw(const Color& color) const
    {
        rect.Draw(rectOrigin, body->GetAngle() * RAD2DEG, color);
    }
};

class Rect
{
  public:
    phys2d::Body *body;
    raylib::Rectangle rect;

    Rect(phys2d::World* world, float x, float y, float width, float height)
    : rect(x - width * 0.5f, y - height * 0.5f, width, height)
    {
        phys2d::BodyDef bodyDef;
        bodyDef.position.Set(x, y);
        body = world->CreateBody(&bodyDef);

        phys2d::PolygonShape wallShape;
        wallShape.SetAsBox(width, height);
        body->CreateFixture(&wallShape, 0.0f);
    }

    void Draw(const Color& color)
    {
        rect.DrawLines(color);
    }
};

class Mouse
{
  private:
    phys2d::Body *body;
    float radius;

  public:
    Mouse(phys2d::World* world, float _radius) : radius(_radius)
    {
        phys2d::BodyDef circleBodyDef;
        circleBodyDef.type = b2_kinematicBody;
        circleBodyDef.position.Set(0, 0);
        body = world->CreateBody(&circleBodyDef);

        phys2d::CircleShape circleShape;
        circleShape.m_radius = radius;
        circleShape.m_p.Set(0, 0);

        phys2d::FixtureDef fixtureDef;
        fixtureDef.shape = &circleShape;
        fixtureDef.density = 1.0f;
        fixtureDef.restitution = 10.0f;
        body->CreateFixture(&fixtureDef);

        body->SetEnabled(false);
    }

    void Update(const std::vector<Box>& boxes)
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            body->SetEnabled(true);
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            phys2d::Vector2 mousePosition(GetMouseX(), GetMouseY());
            body->SetTransform(mousePosition, body->GetAngle());
        }
        else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            body->SetEnabled(false);
        }
    }
};

constexpr unsigned char map[] = {
    0, 0, 1, 0, 0, 0, 1, 0, 0,
    0, 1, 0, 1, 0, 1, 0, 1, 0,
    1, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 1,
    0, 1, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 1, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 1, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0,
};

class Demo : public core::State
{
  private:
    static constexpr float gravity = 9.81f;
    static constexpr float timeStep = 1.0f / 60.0f;

    std::unique_ptr<phys2d::World> world;

    std::unique_ptr<Mouse> circleMouse;
    std::vector<Box> boxes;

    std::unique_ptr<Rect> groundBody;
    std::unique_ptr<Rect> ceilingBody;
    std::unique_ptr<Rect> leftWallBody;
    std::unique_ptr<Rect> rightWallBody;

  public:
    void Enter() override
    {
        // Initialise le monde
        world = std::make_unique<phys2d::World>(
            phys2d::Vector2(0, gravity));

        // Créer des murs statiques
        const float sw = app->GetResolution().x;
        const float sh = app->GetResolution().y;

        groundBody = std::make_unique<Rect>(world.get(), sw * 0.5f, sh - 5.0f, sw, 10.0f);
        ceilingBody = std::make_unique<Rect>(world.get(), sw * 0.5f, 5.0f, sw, 10.0f);
        leftWallBody = std::make_unique<Rect>(world.get(), 5.0f, sh * 0.5f, 10.0f, sh);
        rightWallBody = std::make_unique<Rect>(world.get(), sw - 5.0f, sh * 0.5f, 10.0f, sh);

        // Créer quelques boîtes dynamiques
        int heartWidth = 9 * 20; // Largeur totale du coeur
        int heartHeight = 9 * 20; // Hauteur totale du coeur

        for (unsigned int i = 0; i < sizeof(map); i++)
        {
            if (map[i])
            {
                float x = (sw - heartWidth) * 0.5f + (i % 9) * 20.0f;
                float y = (sh - heartHeight) * 0.5f + (i / 9.0f) * 20.0f;

                boxes.push_back(Box(world.get(), { x, y, 20, 20 }));
            }
        }

        // Créé le corps d'interaction de la souris (cercle dynamique)
        circleMouse = std::make_unique<Mouse>(world.get(), 20.0f);
    }

    void Exit() override
    {
        world = nullptr;
        boxes.clear();
    }

    void Update(float dt) override
    {
        world->Step(timeStep, 6, 2);
        for (Box& box : boxes) box.Update();
        circleMouse->Update(boxes);
    }

    void Draw(const core::Renderer& target) override
    {
        target.Clear();
        for (Box& box : boxes) box.Draw(RED);
        phys2d::DrawWorld(world.get());
    }
};

int main()
{
    core::App app("PHYS 2D - Basic", 800, 600);

    app.AddState<Demo>("demo");
    return app.Run("demo");
}