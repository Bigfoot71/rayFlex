#include "gfx3d/rfCamera.hpp"
#include "Vector3.hpp"
#include "raylib.h"
#include <rlgl.h>

using namespace rf;

gfx3d::Camera::Camera(const Vector3& _position, const Vector3& _target, const Vector3& _up, float _fovy, float _aspect, float _near, float _far, bool _orthographic)
: position(_position)
, target(_target)
, up(_up)
, aspect(_aspect)
, fovy(_fovy)
, near(_near)
, far(_far)
, orthographic(_orthographic)
{
    this->SetAspect();
}

void gfx3d::Camera::MoveForward(float distance, bool moveInWorldPlane)
{
    raylib::Vector3 forward = GetForward();

    if (moveInWorldPlane)
    {
        // Project vector onto world plane
        forward.y = 0;
        forward = forward.Normalize();
    }

    // Scale by distance
    forward *= distance;

    // Move position and target
    position += forward;
    target += forward;
}

void gfx3d::Camera::MoveUp(float distance)
{
    raylib::Vector3 up = GetUp();

    // Scale by distance
    up *= distance;

    // Move position and target
    position += up;
    target += up;
}

void gfx3d::Camera::MoveRight(float distance, bool moveInWorldPlane)
{
    raylib::Vector3 right = GetRight();

    if (moveInWorldPlane)
    {
        // Project vector onto world plane
        right.y = 0;
        right = right.Normalize();
    }

    // Scale by distance
    right *= distance;

    // Move position and target
    position += right;
    target += right;
}

void gfx3d::Camera::MoveToTarget(float delta)
{
    float distance = position.Distance(target);

    // Apply delta
    distance += delta;

    // Distance must be greater than 0
    if (distance < 0) distance = 0.001f;

    // Set new distance by moving the position along the forward vector
    position = target + GetForward() * -distance;
}

// NOTE: angle must be provided in radians
void gfx3d::Camera::SetYaw(float angle, bool rotateAroundTarget)
{
    // Rotation axis
    raylib::Vector3 up = GetUp();

    // View vector
    raylib::Vector3 targetPosition = target - position;

    // Rotate view vector around up axis
    targetPosition = Vector3RotateByAxisAngle(targetPosition, up, angle);

    if (rotateAroundTarget)
    {
        // Move position relative to target
        position = target - targetPosition;
    }
    else // rotate around camera.position
    {
        // Move target relative to position
        target = position + targetPosition;
    }
}

// NOTE: angle must be provided in radians
void gfx3d::Camera::SetPitch(float angle, bool lockView, bool rotateAroundTarget, bool rotateUp)
{
    // View vector
    raylib::Vector3 targetPosition = target - position;

    if (lockView)
    {
        // In these camera modes we clamp the Pitch angle
        // to allow only viewing straight up or down.

        // Clamp view up
        float maxAngleUp = Vector3Angle(up, targetPosition);
        maxAngleUp -= 0.001f; // avoid numerical errors
        if (angle > maxAngleUp) angle = maxAngleUp;

        // Clamp view down
        float maxAngleDown = Vector3Angle(-up, targetPosition);
        maxAngleDown *= -1.0f; // downwards angle is negative
        maxAngleDown += 0.001f; // avoid numerical errors
        if (angle < maxAngleDown) angle = maxAngleDown;
    }

    // Rotation axis
    raylib::Vector3 right = GetRight();

    // Rotate view vector around right axis
    targetPosition = Vector3RotateByAxisAngle(targetPosition, right, angle);

    if (rotateAroundTarget)
    {
        // Move position relative to target
        position = target - targetPosition;
    }
    else // rotate around camera.position
    {
        // Move target relative to position
        target = position + targetPosition;
    }

    if (rotateUp)
    {
        // Rotate up direction around right axis
        up = Vector3RotateByAxisAngle(up, right, angle);
    }
}

// NOTE: angle must be provided in radians
void gfx3d::Camera::SetRoll(float angle)
{
    // Rotation axis
    Vector3 forward = GetForward();

    // Rotate up direction around forward axis
    up = Vector3RotateByAxisAngle(up, forward, angle);
}

Vector3 gfx3d::Camera::GetDirection()
{
    return (target - position).Normalize();
}

Matrix gfx3d::Camera::GetViewMatrix()
{
    return MatrixLookAt(position, target, up);
}

Matrix gfx3d::Camera::GetProjectionMatrix()
{
    if (orthographic)
    {
        double top = fovy/2.0;
        double right = top*aspect;
        return MatrixOrtho(-right, right, -top, top, near, far);
    }
    return MatrixPerspective(fovy*DEG2RAD, aspect, near, far);
}

void gfx3d::Camera::Translate(const Vector3& delta)
{
    MoveForward(delta.z);
    MoveRight(delta.x);
    MoveUp(delta.y);
}

// NOTE: angle must be provided in degrees
void gfx3d::Camera::Rotate(const Vector3& delta, bool lockView)
{
    SetPitch(-delta.y * DEG2RAD, lockView);
    SetYaw(-delta.x * DEG2RAD);
    SetRoll(delta.z * DEG2RAD);
}

void gfx3d::Camera::SetAspect(const float aspect)
{
    if (aspect == 0)
    {
        const float sw = GetScreenWidth();
        const float sh = GetScreenHeight();
        this->aspect = (sw > sh) ? sw / sh : sh / sw;
    }
}

void gfx3d::Camera::Update(int mode)
{
    Vector2 mousePositionDelta = GetMouseDelta();

    const bool moveInWorldPlane = ((mode == CAMERA_FIRST_PERSON) || (mode == CAMERA_THIRD_PERSON));
    const bool rotateAroundTarget = ((mode == CAMERA_THIRD_PERSON) || (mode == CAMERA_ORBITAL));
    const bool lockView = ((mode == CAMERA_FIRST_PERSON) || (mode == CAMERA_THIRD_PERSON) || (mode == CAMERA_ORBITAL));
    const bool rotateUp = (mode == CAMERA_FREE);

    if (mode == CAMERA_ORBITAL)
    {
        // Orbital can just orbit
        Matrix rotation = MatrixRotate(GetUp(), orbitalSpeed*GetFrameTime());
        raylib::Vector3 view = (position - target).Transform(rotation);
        position = target + view;
    }
    else
    {
        // Camera rotation
        if (IsKeyDown(KEY_DOWN)) SetPitch(-rotationSpeed, lockView, rotateAroundTarget, rotateUp);
        if (IsKeyDown(KEY_UP)) SetPitch(rotationSpeed, lockView, rotateAroundTarget, rotateUp);
        if (IsKeyDown(KEY_RIGHT)) SetYaw(-rotationSpeed, rotateAroundTarget);
        if (IsKeyDown(KEY_LEFT)) SetYaw(rotationSpeed, rotateAroundTarget);
        if (IsKeyDown(KEY_Q)) SetRoll(-rotationSpeed);
        if (IsKeyDown(KEY_E)) SetRoll( rotationSpeed);

        SetYaw(-mousePositionDelta.x*mouseMoveSensitivity, rotateAroundTarget);
        SetPitch(-mousePositionDelta.y*mouseMoveSensitivity, lockView, rotateAroundTarget, rotateUp);

        // Camera movement
        if (IsKeyDown(KEY_W)) MoveForward(movementSpeed, moveInWorldPlane);
        if (IsKeyDown(KEY_A)) MoveRight(-movementSpeed, moveInWorldPlane);
        if (IsKeyDown(KEY_S)) MoveForward(-movementSpeed, moveInWorldPlane);
        if (IsKeyDown(KEY_D)) MoveRight(movementSpeed, moveInWorldPlane);
        if (IsKeyDown(KEY_SPACE)) MoveUp(movementSpeed);
        if (IsKeyDown(KEY_LEFT_SHIFT)) MoveUp(-movementSpeed);
    }

    if ((mode == CAMERA_THIRD_PERSON) || (mode == CAMERA_ORBITAL))
    {
        // Zoom target distance
        MoveToTarget(-GetMouseWheelMove());
        if (IsKeyPressed(KEY_KP_SUBTRACT)) MoveToTarget(2.0f);
        if (IsKeyPressed(KEY_KP_ADD)) MoveToTarget(-2.0f);
    }

}

void gfx3d::Camera::Update(const Vector3& movement, const Vector3& rotation, float zoom)
{
    Rotate(rotation);
    Translate(movement);
    if (zoom) MoveToTarget(zoom);
}

gfx3d::Camera& gfx3d::Camera::BeginMode()
{
    rlDrawRenderBatchActive();      // Update and draw internal render batch

    rlMatrixMode(RL_PROJECTION);    // Switch to projection matrix
    rlPushMatrix();                 // Save previous matrix, which contains the settings for the 2d ortho projection
    rlLoadIdentity();               // Reset current matrix (projection)

    if (orthographic)
    {
        // Setup orthographic projection
        double top = fovy/2.0;
        double right = top*aspect;

        rlOrtho(-right, right, -top,top, near, far);
    }
    else
    {
        // Setup perspective projection
        double top = near*tan(fovy*0.5*DEG2RAD);
        double right = top*aspect;

        rlFrustum(-right, right, -top, top, near, far);
    }

    rlMatrixMode(RL_MODELVIEW);     // Switch back to modelview matrix
    rlLoadIdentity();                   // Reset current matrix (modelview)

    // Setup Camera view
    rlMultMatrixf(MatrixToFloat(GetViewMatrix()));      // Multiply modelview matrix by view matrix (camera)

    rlEnableDepthTest();            // Enable DEPTH_TEST for 3D

    return *this;
}

gfx3d::Camera& gfx3d::Camera::EndMode()
{
    EndMode3D();
    return *this;
}

void gfx3d::Camera::Print()
{
    TraceLog(LOG_DEBUG, "*~~~~~~~~~~~~~~ CAMERA ~~~~~~~~~~~~~~*");
    TraceLog(LOG_DEBUG, "Position: %f %f %f", position.x, position.y, position.z);
    TraceLog(LOG_DEBUG, "Target: %f %f %f", target.x, target.y, target.z);
    TraceLog(LOG_DEBUG, "Up: %f %f %f", up.x, up.y, up.z);
    TraceLog(LOG_DEBUG, "Aspect: %f", aspect);
    TraceLog(LOG_DEBUG, "FOV: %f", fovy);
    TraceLog(LOG_DEBUG, "Near: %f", near);
    TraceLog(LOG_DEBUG, "Far: %f", far);
    TraceLog(LOG_DEBUG, "orthographic: %s", orthographic ? "True" : "False");
    TraceLog(LOG_DEBUG, "*~~~~~~~~~~~~~~ END ~~~~~~~~~~~~~~~~~*");
}
