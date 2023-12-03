#include "gfx3d/rfSprite.hpp"

using namespace rf;

void gfx3d::Sprite::DrawBillboardSuperPro(
    const Camera3D& camera,
    Rectangle source,
    const raylib::Vector3& position,
    const raylib::Vector3& up,
    const raylib::Vector2& size,
    const raylib::Vector2& origin,
    float rotation,
    Color tint)
const
{
    // NOTE: Allows you to render the billboard as a raylib base texture.
    // This means being able, for example, to display the texture "upside down" by inverting
    // one of the source sizes. Special thanks to JeffM (Jeffery Myers) for the help provided.

    bool flipX = source.width < 0;
    bool flipY = source.height < 0;

    source.width = std::abs(source.width);
    source.height = std::abs(source.height);

    // NOTE: Billboard size will maintain source rectangle aspect ratio, size will represent billboard width
    Vector2 sizeRatio = { size.x * source.width / source.height, size.y };

    Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);

    raylib::Vector3 right(matView.m0, matView.m4, matView.m8);
    //raylib::Vector3 up(matView.m1, matView.m5, matView.m9);

    raylib::Vector3 rightScaled = right * (sizeRatio.x * 0.5f);
    raylib::Vector3 upScaled = up * (sizeRatio.y * 0.5f);

    raylib::Vector3 p1 = rightScaled + upScaled;
    raylib::Vector3 p2 = rightScaled - upScaled;

    raylib::Vector3 topLeft = -p2;
    raylib::Vector3 topRight = p1;
    raylib::Vector3 bottomRight = p2;
    raylib::Vector3 bottomLeft = -p1;

    if (rotation != 0.0f)
    {
        float sinRotation = std::sin(rotation * DEG2RAD);
        float cosRotation = std::cos(rotation * DEG2RAD);

        // NOTE: (-1, 1) is the range where origin.x, origin.y is inside the texture
        float rotateAboutX = sizeRatio.x * origin.x / 2;
        float rotateAboutY = sizeRatio.y * origin.y / 2;

        float xtvalue, ytvalue;
        float rotatedX, rotatedY;

        xtvalue = right.DotProduct(topLeft) - rotateAboutX;                 // Project points to x and y coordinates on the billboard plane
        ytvalue = up.DotProduct(topLeft) - rotateAboutY;
        rotatedX = xtvalue * cosRotation - ytvalue * sinRotation + rotateAboutX;    // Rotate about the point origin
        rotatedY = xtvalue * sinRotation + ytvalue * cosRotation + rotateAboutY;
        topLeft = (up * rotatedY) + (right * rotatedX);                             // Translate back to cartesian coordinates

        xtvalue = right.DotProduct(topRight) - rotateAboutX;
        ytvalue = up.DotProduct(topRight) - rotateAboutY;
        rotatedX = xtvalue * cosRotation - ytvalue * sinRotation + rotateAboutX;
        rotatedY = xtvalue * sinRotation + ytvalue * cosRotation + rotateAboutY;
        topRight = (up * rotatedY) + (right * rotatedX);

        xtvalue = right.DotProduct(bottomRight) - rotateAboutX;
        ytvalue = up.DotProduct(bottomRight) - rotateAboutY;
        rotatedX = xtvalue * cosRotation - ytvalue * sinRotation + rotateAboutX;
        rotatedY = xtvalue * sinRotation + ytvalue * cosRotation + rotateAboutY;
        bottomRight = (up * rotatedY) + (right * rotatedX);

        xtvalue = right.DotProduct(bottomLeft) - rotateAboutX;
        ytvalue = up.DotProduct(bottomLeft) - rotateAboutY;
        rotatedX = xtvalue * cosRotation - ytvalue * sinRotation + rotateAboutX;
        rotatedY = xtvalue * sinRotation + ytvalue * cosRotation + rotateAboutY;
        bottomLeft = (up * rotatedY) + (right * rotatedX);
    }

    // Translate points to the draw center (position)
    topLeft += position, topRight += position;
    bottomRight += position, bottomLeft += position;

    rlSetTexture(texture->id);

    rlBegin(RL_QUADS);
    rlColor4ub(tint.r, tint.g, tint.b, tint.a);

    // Bottom-left corner for texture and quad
    float u = flipX ? (source.x + source.width) / texture->width : source.x / texture->width;
    float v = flipY ? (source.y + source.height) / texture->height : source.y / texture->height;
    rlTexCoord2f(u,v);
    rlVertex3f(topLeft.x, topLeft.y, topLeft.z);

    // Top-left corner for texture and quad
    u = flipX ? (source.x + source.width) / texture->width : source.x / texture->width;
    v = !flipY ? (source.y + source.height) / texture->height : source.y / texture->height;
    rlTexCoord2f(u,v);
    rlVertex3f(bottomLeft.x, bottomLeft.y, bottomLeft.z);

    // Top-right corner for texture and quad
    u = !flipX ? (source.x + source.width) / texture->width : source.x / texture->width;
    v = !flipY ? (source.y + source.height) / texture->height : source.y / texture->height;
    rlTexCoord2f(u, v);
    rlVertex3f(bottomRight.x, bottomRight.y, bottomRight.z);

    // Bottom-right corner for texture and quad
    u = !flipX ? (source.x + source.width) / texture->width : source.x / texture->width;
    v = flipY ? (source.y + source.height) / texture->height : source.y / texture->height;
    rlTexCoord2f(u, v);
    rlVertex3f(topRight.x, topRight.y, topRight.z);
    rlEnd();

    rlSetTexture(0);
}

void gfx3d::Sprite::DrawBillboard(Camera3D& camera, Vector3& pos, Color color, const Instance * const instance) const
{
    texture->DrawBillboard(camera, instance->frameRec, pos, { 0.0f, 1.0f, 0.0f }, { 1.f, 1.f }, {0,0}, 0.0f, color);
}

void gfx3d::Sprite::DrawBillboard(Camera3D& camera, Vector3& pos, Vector2 size, Color color, const Instance * const instance) const
{
    Rectangle srcFrameRect = instance->frameRec;

    if (size.x < 0) srcFrameRect.width *= -1, size.x *= -1;
    if (size.y < 0) srcFrameRect.height *= -1, size.y *= -1;

    DrawBillboardSuperPro(camera,
        srcFrameRect, pos,
        {0.0f, 1.0f, 0.0f}, size,
        {}, 0.0f, color);
}
