#include <rayflex.hpp>

using namespace rf;

class Game : public core::State
{
  private:
    gfx3d::Camera *camera;
    gfx3d::Lights *lights;
    gfx3d::Light *l1;
    gfx3d::Light *l2;

    raylib::Model *cube;
    float angle = 0.0f;

    raylib::Model *plane;
    float phase = 0.0f;

  public:
    void Enter() override
    {
        camera = new gfx3d::Camera{
            { 0, 20, -20 },
            { 0, 0, 20 },
            { 0, 1, 0 },
            60.0f,
            800.0f/600.0f,
            0.1f,
            100.0f
        };

        camera->movementSpeed = 1.0f;

        lights = new gfx3d::Lights(0.1f, 2);

        l1 = lights->AddShadowLight({
            { 0, 30, 40 },
            { 0, 0, 20 },
            { 0, 1, 0 },
            60.0f,
            1.0f,
            0.1f,
            100.0f
        }, 512, BLUE);

        l2 = lights->AddShadowLight({
            { 0, 30, 0 },
            { 0, 0, 20 },
            { 0, 1, 0 },
            60.0f,
            1.0f,
            0.1f,
            100.0f
        }, 512, RED);

        cube = new raylib::Model(GenMeshCube(10, 10, 10));
        angle = 0.0f;

        plane = new raylib::Model(GenMeshPlane(40, 40, 1, 1));
        phase = 0.0f;

        DisableCursor();
    }

    void Exit() override
    {
        EnableCursor();
        delete camera;
        delete lights;
        delete cube;
        delete plane;
    }

    void Update(float dt) override
    {
        phase += 2.0f * dt, angle += 45.0f * dt;

        Vector3 p1 = l1->GetPosition();
        p1.x += std::cos(phase) * 20.0f * dt;
        p1.z -= std::sin(phase) * 20.0f * dt;
        l1->SetPosition(p1);

        Vector3 p2 = l2->GetPosition();
        p2.x -= std::cos(phase) * 20.0f * dt;
        p2.z += std::sin(phase) * 20.0f * dt;
        l2->SetPosition(p2);

        camera->Update(CAMERA_FIRST_PERSON);
        lights->Update(*camera);

        lights->DrawDepth(*plane, { 0, -5, 20 }, {}, 0, { 1, 1, 1 }, GRAY);
        lights->DrawDepth(*cube, { 0, 0, 20 }, { 0, 1, 0 }, angle, { 1, 1, 1 }, WHITE);
    }

    void Draw() override
    {
        camera->BeginMode();
            lights->Draw(*plane, { 0, -5, 20 }, {}, 0, { 1, 1, 1 }, GRAY);
            lights->Draw(*cube, { 0, 0, 20 }, { 0, 1, 0 }, angle, { 1, 1, 1 }, WHITE);
            DrawSphere(l1->GetPosition(), 1.0f, l1->GetColor());
            DrawSphere(l2->GetPosition(), 1.0f, l2->GetColor());
        camera->EndMode();
    }
};

int main()
{
    core::App app("GFX3D - Lights with shadows", 800, 600);
    app.AddState<Game>("demo");
    return app.Run("demo");
}
