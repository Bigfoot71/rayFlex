#ifndef RAYFLEX_PHYS_3D_OBJECT_HPP
#define RAYFLEX_PHYS_3D_OBJECT_HPP
#ifdef SUPPORT_PHYS_3D

#include "LinearMath/btTransform.h"
#include <btBulletDynamicsCommon.h>
#include <raylib-cpp.hpp>
#include <cstring>

namespace rf { namespace phys3d {

    /**
     * @brief Enum class representing the shape of a 3D physics object.
     */
    enum class Shape
    {
        None,     ///< No specific shape
        Mesh,     ///< Mesh shape (can only be static, use Capsule or Hull for a dynamic body with a mesh)
        Hull,     ///< Hull shape
        Cube,     ///< Cube shape
        Sphere,   ///< Sphere shape
        Capsule   ///< Capsule shape
    };

    // NOTE: Classes from this header are used internally by phys3d::World

    /**
     * @brief Class representing a 3D physics object.
     */
    class Object
    {
      public:
        static constexpr float Static = 0.0f; ///< Constant representing a static object.

        btCollisionShape *colliderShape;  ///< Bullet's collision shape for the object.
        raylib::Model *model;             ///< Raylib's model for rendering.
        btRigidBody *body;                ///< Bullet's rigid body for the physics simulation.
        const Shape shape;                ///< Shape of the object.
        Color color;                      ///< Color of the object.

        bool drawWires = false;           ///< Flag to draw wires for debugging.

        /**
         * @brief Constructor for the Object class.
         * @param _shape Shape of the object.
         * @param _color Color of the object.
         */
        Object(Shape _shape = Shape::None, const Color& _color = WHITE)
            : colliderShape(nullptr), model(nullptr), body(nullptr), shape(_shape), color(_color)
        { }

        /**
         * @brief Destructor for the Object class.
         */
        virtual ~Object()
        {
            if (body != nullptr)
            {
                if (body->getMotionState())
                {
                    delete body->getMotionState();
                }
                delete body;
                body = nullptr;
            }
            if (colliderShape != nullptr)
            {
                delete colliderShape;
                colliderShape = nullptr;
            }
        }

        /**
         * @brief Checks if the object is ready.
         * @return True if the object has a valid body and motion state.
         */
        bool IsReady() const
        {
            return body && body->getMotionState();
        }

        /**
         * @brief Gets the transform matrix of the object.
         * @return The transformation matrix.
         */
        Matrix GetTransform() const;

        /**
         * @brief Gets the position vector of the object.
         * @return The position vector.
         */
        Vector3 GetPosition() const;

        /**
         * @brief Gets the quaternion of the object's rotation.
         * @return The rotation quaternion.
         */
        Quaternion GetQuaternion() const;

        /**
         * @brief Gets the rotation axis vector of the object.
         * @return The rotation axis vector.
         */
        Vector3 GetRotationAxis() const;

        /**
         * @brief Gets the rotation angle of the object.
         * @return The rotation angle.
         */
        float GetAngle() const;

        /**
         * @brief Draws the object using its model.
         */
        void Draw() const;

        /**
         * @brief Sets the shader for the object's model.
         * @param shader The shader to set.
         */
        void SetShader(const raylib::Shader& shader)
        {
            model->materials->shader = shader;
        }

      protected:
        /**
         * @brief Creates a Bullet rigid body for the object.
         * @param position Initial position of the object.
         * @param rotation Initial rotation of the object.
         * @param mass Mass of the object (0.0f for static objects).
         * @return Pointer to the created rigid body.
         */
        btRigidBody* CreateBody(const Vector3& position,
                                const Vector3& rotation,
                                float mass);

        /**
         * @brief Unloads the Raylib model associated with the object.
         */
        inline void UnloadModel()
        {
            if (model != nullptr)
            {
                delete model;
                model = nullptr;
            }
        }
    };

    // NOTES: These derivations below allow simplified generation of shapes associated with a 3D model corresponding to the shape via phys3d::World

    /**
     * @brief Class representing a cube-shaped 3D physics object.
     */
    class Cube : public Object
    {
      public:
        /**
         * @brief Constructor for the Cube class.
         * @param position Initial position of the cube.
         * @param rotation Initial rotation of the cube.
         * @param size Size of the cube.
         * @param mass Mass of the cube (0.0f for static cubes).
         * @param _color Color of the cube.
         */
        Cube(const Vector3& position = { 0, 0, 0 },
             const Vector3& rotation = { 0, 0, 0 },
             const Vector3& size = { 0, 0, 0 },
             float mass = 0.0f, const Color& _color = WHITE);

        /**
         * @brief Destructor for the Cube class.
         */
        ~Cube() { UnloadModel(); }
    };

    /**
     * @brief Class representing a sphere-shaped 3D physics object.
     */
    class Sphere : public Object
    {
      public:
        /**
         * @brief Constructor for the Sphere class.
         * @param position Initial position of the sphere.
         * @param rotation Initial rotation of the sphere.
         * @param radius Radius of the sphere.
         * @param rings Number of rings for the sphere model.
         * @param slices Number of slices for the sphere model.
         * @param mass Mass of the sphere (0.0f for static spheres).
         * @param _color Color of the sphere.
         */
        Sphere(const Vector3& position = { 0, 0, 0 },
               const Vector3& rotation = { 0, 0, 0 },
               float radius = 0, int rings = 0, int slices = 0,
               float mass = 0.0f, const Color& _color = WHITE);

        /**
         * @brief Destructor for the Sphere class.
         */
        ~Sphere() { UnloadModel(); }
    };

}}

#endif //SUPPORT_PHYS_3D
#endif //RAYFLEX_PHYS_3D_OBJECT_HPP
