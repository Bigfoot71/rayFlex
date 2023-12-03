#include "phys2d/rfPhysics.hpp"
#include <Vector2.hpp>

using namespace rf;

void phys2d::DrawShape(const Shape* shape, const Transform& transform, const Color& color)
{
    switch (shape->GetType())
    {
        case Shape::e_circle:
        {
            const auto circleShape = (CircleShape*)(shape);
            DrawCircleLines(
                transform.p.x + circleShape->m_p.x,
                transform.p.y + circleShape->m_p.y,
                circleShape->m_radius, color);
        } break;

        case Shape::e_chain:
        {
            const auto chainShape = (ChainShape*)(shape);
            const int vertexCount = chainShape->m_count;

            for (int i = 1; i < vertexCount; i++)
            {
                const Vector2& vertexA = chainShape->m_vertices[i - 1];
                const Vector2& vertexB = chainShape->m_vertices[i];

                const raylib::Vector2 pA = {
                    transform.p.x + vertexA.x,
                    transform.p.y + vertexA.y
                };
                const raylib::Vector2 pB = {
                    transform.p.x + vertexB.x,
                    transform.p.y + vertexB.y
                };

                DrawLineV(pA, pB, color);
            }
        } break;

        case Shape::e_edge:
        {
            const auto edgeShape = (EdgeShape*)(shape);

            const Vector2& vertexA = edgeShape->m_vertex1;
            const Vector2& vertexB = edgeShape->m_vertex2;

            const raylib::Vector2 pA = {
                transform.p.x + vertexA.x,
                transform.p.y + vertexA.y
            };
            const raylib::Vector2 pB = {
                transform.p.x + vertexB.x,
                transform.p.y + vertexB.y
            };

            DrawLineV(pA, pB, color);
        } break;

        case Shape::e_polygon:
        {
            const auto polygonShape = (PolygonShape*)(shape);
            const float s = transform.q.s, c = transform.q.c;
            const int vertexCount = polygonShape->m_count;

            for (int i = 0; i < vertexCount; i++)
            {
                int j = (((i + 1) < vertexCount) ? (i + 1) : 0); // next vertice

                const Vector2& vertexA = polygonShape->m_vertices[i];
                const Vector2& vertexB = polygonShape->m_vertices[j];

                const ::Vector2 pA = {
                    transform.p.x + (vertexA.x * c - vertexA.y * s),
                    transform.p.y + (vertexA.x * s + vertexA.y * c)
                };

                const ::Vector2 pB = {
                    transform.p.x + (vertexB.x * c - vertexB.y * s),
                    transform.p.y + (vertexB.x * s + vertexB.y * c)
                };

                DrawLineV(pA, pB, color);
            }
        } break;

        default: break;
    }
}

void phys2d::DrawBody(const phys2d::Body* body)
{
    // GREEN: when the body is awake
    // YELLOW: when the body is asleep
    // TRANSPARENT: when the body is deactivated

    Transform transform = body->GetTransform();

    DrawCircle(transform.p.x, transform.p.y, 2,
        ColorAlpha(PURPLE, body->IsEnabled() ? 1.0f : 0.5f));

    const Color color = ColorAlpha(
        body->IsAwake() ? GREEN : YELLOW,
        body->IsEnabled() ? 1.0f : 0.5f);

    for (auto fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
    {
        DrawShape(fixture->GetShape(), transform, color);
    }
}

void phys2d::DrawWorld(const phys2d::World* world)
{
    for (auto body = world->GetBodyList(); body; body = body->GetNext())
    {
        DrawBody(body);
    }
}