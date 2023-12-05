#include <rayflex.hpp>

using namespace rf;

class Demo : public core::State
{
  private:
    static constexpr float gravity = 9.81f;
    static constexpr float timeStep = 1.0f / 60.0f;

    raylib::Camera3D camera;
    phys3d::World *world;

  public:
    void Enter() override
    {
        camera = raylib::Camera3D(
            { 10.0f, 5.0f, 10.0f },
            { 0.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            60.0f);

        world = new phys3d::World(
            { 0, -10, 0 });

        // Add floor object
        world->AddObject<phys3d::Cube>(
            Vector3{0,0,0},
            Vector3{0,0,0},
            Vector3{8,0.5,8},
            0.0, GRAY
        );

        core::RandomGenerator gen;

        for (size_t i = 0; i < 30; i++)
        {
            if (gen.Random<uint8_t>(0, 1))
            {
                world->AddObject<phys3d::Cube>(
                    gen.RandomVec3({ -5, 10, -5 }, { 5, 15, 5 }),
                    gen.RandomVec3({ -3, -3, -3 }, { 3, 3, 3 }),
                    gen.RandomVec3({ 1, 1, 1 }, { 3, 3, 3 }),
                    1.0f, gen.RandomColor());
            }
            else
            {
                world->AddObject<phys3d::Sphere>(
                    gen.RandomVec3({ -5, 10, -5 }, { 5, 15, 5 }),
                    gen.RandomVec3({ -3, -3, -3 }, { 3, 3, 3 }),
                    gen.Random<float>(1, 3), 32, 32, 1.0f, gen.RandomColor());
            }
        }
    }

    void Exit() override
    {
        delete world;
    }

    void Update(float dt) override
    {
        camera.Update(CAMERA_ORBITAL);
        world->Step(timeStep, 10); // Update physics
    }

    void Draw(const core::Renderer& target) override
    {
        target.Clear();

        camera.BeginMode();
            DrawGrid(10, 1.0);
            world->Draw();
        camera.EndMode();
    }
};

int main()
{
    core::App app("PHYS 3D - Basic", 800, 600);
    app.AddState<Demo>("demo");
    return app.Run("demo");
}