#ifndef RAYFLEX_GFX_3D_CAMERA_HPP
#define RAYFLEX_GFX_3D_CAMERA_HPP
#ifdef SUPPORT_GFX_3D

#include <Vector3.hpp>
#include <Vector4.hpp>

namespace rf { namespace gfx3d {

    /**
     * @brief A struct representing a 3D camera.
     */
    struct Camera
    {
        // Camera settings
        float movementSpeed         = 0.09f;    ///< Movement speed of the camera.
        float rotationSpeed         = 0.03f;    ///< Rotation speed of the camera.
        float orbitalSpeed          = 0.5f;     ///< Orbital speed in radians per second.
        float mouseMoveSensitivity  = 0.003f;   ///< Mouse movement sensitivity. TODO: Make it independent of framerate.
        float mouseWheelSensitivity = 1.5f;     ///< Mouse wheel sensitivity.

        // Camera position and orientation
        raylib::Vector3 position;               ///< Camera position.
        raylib::Vector3 target;                 ///< Camera target position.
        raylib::Vector3 up;                     ///< Camera up vector.
        float aspect;                           ///< Aspect ratio of the camera.
        float fovy;                             ///< Field of view in the Y direction in degrees.
        float near;                             ///< Near clipping plane distance.
        float far;                              ///< Far clipping plane distance.

        bool orthographic;                      ///< Flag indicating if the camera is in orthographic projection mode.

        /**
         * @brief Constructs a Camera object.
         * @param _position Initial camera position.
         * @param _target Initial camera target.
         * @param _up Initial up vector for the camera.
         * @param _fovy Field of view in the Y direction in degrees.
         * @param _aspect Aspect ratio of the camera.
         * @param _near Near clipping plane distance.
         * @param _far Far clipping plane distance.
         * @param _orthographic Flag indicating if the camera is in orthographic projection mode.
         */
        Camera(const Vector3& _position,
               const Vector3& _target,
               const Vector3& _up = { 0, 1, 0 },
               float _fovy = 60.0f,
               float _aspect = 0,
               float _near = 0.1f,
               float _far = 1000.0f,
               bool _orthographic = false);

        /**
         * @brief Get the right vector of the camera.
         * @return The right vector of the camera.
         */
        inline raylib::Vector3 GetRight() const { return GetForward().CrossProduct(GetUp()); }

        /**
         * @brief Get the forward vector of the camera.
         * @return The forward vector of the camera.
         */
        inline raylib::Vector3 GetForward() const { return (target - position).Normalize(); }

        /**
         * @brief Get the up vector of the camera.
         * @return The up vector of the camera.
         */
        inline raylib::Vector3 GetUp() const { return up.Normalize(); }

        /**
         * @brief Move the camera forward.
         * @param distance Distance to move.
         * @param moveInWorldPlane Flag to move in the world plane.
         */
        void MoveForward(float distance, bool moveInWorldPlane = true);

        /**
         * @brief Move the camera up.
         * @param distance Distance to move.
         */
        void MoveUp(float distance);

        /**
         * @brief Move the camera right.
         * @param distance Distance to move.
         * @param moveInWorldPlane Flag to move in the world plane.
         */
        void MoveRight(float distance, bool moveInWorldPlane = true);

        /**
         * @brief Move the camera towards its target.
         * @param delta Delta time.
         */
        void MoveToTarget(float delta);

        /**
         * @brief Set the yaw (rotation around the Y-axis) of the camera.
         * @param angle Yaw angle in radians.
         * @param rotateAroundTarget Flag to rotate around the target.
         */
        void SetYaw(float angle, bool rotateAroundTarget = false);

        /**
         * @brief Set the pitch (rotation around the X-axis) of the camera.
         * @param angle Pitch angle in radians.
         * @param lockView Flag to lock the view.
         * @param rotateAroundTarget Flag to rotate around the target.
         * @param rotateUp Flag to rotate up.
         */
        void SetPitch(float angle, bool lockView = true, bool rotateAroundTarget = false, bool rotateUp = false);

        /**
         * @brief Set the roll (rotation around the Z-axis) of the camera.
         * @param angle Roll angle in radians.
         */
        void SetRoll(float angle);

        /**
         * @brief Get the direction vector of the camera.
         * @return The direction vector of the camera.
         */
        Vector3 GetDirection();

        /**
         * @brief Get the view matrix of the camera.
         * @return The view matrix of the camera.
         */
        Matrix GetViewMatrix();

        /**
         * @brief Get the projection matrix of the camera.
         * @return The projection matrix of the camera.
         */
        Matrix GetProjectionMatrix();

        /**
         * @brief Translate the camera by a delta vector.
         * @param delta Translation vector.
         */
        void Translate(const Vector3& delta);

        /**
         * @brief Rotate the camera by a delta vector.
         * @param delta Rotation vector.
         * @param lockView Flag to lock the view.
         */
        void Rotate(const Vector3& delta, bool lockView = true);

        /**
         * @brief Set the aspect ratio of the camera.
         * @param aspect Aspect ratio value.
         */
        void SetAspect(const float aspect = 0);

        /**
         * @brief Update the camera based on the specified mode.
         * @param mode Update mode.
         */
        void Update(int mode);

        /**
         * @brief Update the camera with specified movement, rotation, and zoom values.
         * @param movement Translation vector.
         * @param rotation Rotation vector.
         * @param zoom Zoom value.
         */
        void Update(const Vector3& movement, const Vector3& rotation, float zoom = 0);

        /**
         * @brief Begin camera mode.
         * @return A reference to the camera.
         */
        Camera& BeginMode();

        /**
         * @brief End camera mode.
         * @return A reference to the camera.
         */
        Camera& EndMode();

        /**
         * @brief Print camera information for debugging purposes.
         */
        void Print();
    };
}}

#endif //SUPPORT_GFX_3D
#endif //RAYFLEX_GFX_3D_CAMERA_HPP
