#include "phys3d/rfObject.hpp"

using namespace rf;

// BASE CLASS IMPLEMENTATION //

btRigidBody* phys3d::Object::CreateBody(const Vector3& position, const Vector3& rotation, float mass)
{
    btScalar objectMass(mass);
    btVector3 localInertia(0, 0, 0);

    // Objects with 0.0 mass are static
    if (mass != 0.0) colliderShape->calculateLocalInertia(
        mass, localInertia);

    // Set default motion state with location and rotation
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(
        btQuaternion(btScalar(rotation.z), btScalar(rotation.y), btScalar(rotation.x)),
        btVector3(position.x, position.y, position.z)));

    // Return rigid body
    btRigidBody::btRigidBodyConstructionInfo rb_info(objectMass, motionState, colliderShape, localInertia);
    return new btRigidBody(rb_info);
}

Matrix phys3d::Object::GetTransform() const
{
    if (!IsReady()) return {};

    btTransform btTrans;
    body->getMotionState()->getWorldTransform(btTrans);

    float mat[16];
    btTrans.getOpenGLMatrix(mat);
    
    Matrix transform;
    std::memcpy(&transform, mat, sizeof(Matrix));

    return MatrixTranspose(transform);
}

Vector3 phys3d::Object::GetPosition() const
{
    if (!IsReady()) return Vector3Zero();

    btTransform btTrans;
    body->getMotionState()->getWorldTransform(btTrans);

    return {
        static_cast<float>(btTrans.getOrigin().getX()),
        static_cast<float>(btTrans.getOrigin().getY()),
        static_cast<float>(btTrans.getOrigin().getZ())
    };
}

Quaternion phys3d::Object::GetQuaternion() const
{
    if (!IsReady()) return {};

    btTransform btTrans;
    body->getMotionState()->getWorldTransform(btTrans);

    btQuaternion quat = btTrans.getRotation();

    return {
        static_cast<float>(quat.getX()),
        static_cast<float>(quat.getY()),
        static_cast<float>(quat.getZ()),
        static_cast<float>(quat.getW())
    };
}

Vector3 phys3d::Object::GetRotationAxis() const
{
    if (!IsReady()) return {};

    btTransform btTrans;
    body->getMotionState()->getWorldTransform(btTrans);

    btQuaternion quat = btTrans.getRotation();

    return {
        static_cast<float>(quat.getAxis().getX()),
        static_cast<float>(quat.getAxis().getY()),
        static_cast<float>(quat.getAxis().getZ())
    };
}

float phys3d::Object::GetAngle() const
{
    if (!(body && body->getMotionState())) return 0;

    btTransform btTrans;
    body->getMotionState()->getWorldTransform(btTrans);

    return btTrans.getRotation().getAngle() * RAD2DEG;
}

void phys3d::Object::Draw() const
{
    if (!IsReady()) return;

    // Get the transform of the body
    btTransform btTrans;
    body->getMotionState()->getWorldTransform(btTrans);

    // Get position from transform
    Vector3 position = {
        static_cast<float>(btTrans.getOrigin().getX()),
        static_cast<float>(btTrans.getOrigin().getY()),
        static_cast<float>(btTrans.getOrigin().getZ())
    };

    // Get rotation from transform
    btQuaternion quat = btTrans.getRotation();

    Vector3 axis = {
        static_cast<float>(quat.getAxis().getX()),
        static_cast<float>(quat.getAxis().getY()),
        static_cast<float>(quat.getAxis().getZ())
    };

    float angle = quat.getAngle() * RAD2DEG;

    model->Draw(
        position, axis, angle,
        {1,1,1,}, color);

    if (drawWires)
    {
        model->DrawWires(
            position, axis, angle, {1,1,1}, {
                static_cast<uint8_t>(color.r/2),
                static_cast<uint8_t>(color.g/2),
                static_cast<uint8_t>(color.b/2),
                color.a});
    }
}

// CUBE CLASS IMPLEMENTATION //

phys3d::Cube::Cube(
    const Vector3& position,
    const Vector3& rotation,
    const Vector3& size,
    float mass,
    const Color& _color)
: Object(Shape::Cube, _color)
{
    colliderShape = new btBoxShape(
        btVector3(
            btScalar(size.x * 0.5f),
            btScalar(size.y * 0.5f),
            btScalar(size.z * 0.5f)));

    model = new raylib::Model(raylib::Mesh::Cube(
        size.x, size.y, size.z));

    body = CreateBody(position, rotation, mass);
}

// SPHERE CLASS IMPLEMENTATION //

phys3d::Sphere::Sphere(
    const Vector3& position,
    const Vector3& rotation,
    float radius, int rings, int slices,
    float mass, const Color& _color)
: Object(Shape::Sphere, _color)
{
    colliderShape = new btSphereShape(btScalar(radius));
    model = new raylib::Model(raylib::Mesh::Sphere(radius, rings, slices));
    body = CreateBody(position, rotation, mass);
}
