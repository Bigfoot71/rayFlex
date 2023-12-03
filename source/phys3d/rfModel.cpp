#include "phys3d/rfModel.hpp"

using namespace rf;

// PUBLIC //

phys3d::Model::Model(
    const Vector3& position,
    const Vector3& rotation,
    raylib::Model* _model,
    Shape _shape, float mass,
    const Color& _color)
: Object(_shape, _color)
, allocatedMapDiffuse(false)
, allocatedModel(false)
{
    model = _model;
    assert(model); assert(shape);
    if (shape == Shape::Mesh && mass != 0) mass = 0;
    colliderShape = CreateCollisionShapeFromModel();
    body = CreateBody(position, rotation, mass);
}

phys3d::Model::Model(
    const Vector3& position,
    const Vector3& rotation,
    const std::string& modelPath,
    const std::string& diffusePath,
    Shape _shape, float mass,
    const Color& _color)
: Object(_shape, _color)
, allocatedMapDiffuse(true)
, allocatedModel(true)
{
    assert(shape);
    if (shape == Shape::Mesh && mass != 0) mass = 0;
    model = new raylib::Model(modelPath);
    mapDiffuse = new raylib::Texture2D(diffusePath);
    model->materials->maps[MATERIAL_MAP_DIFFUSE].texture = *mapDiffuse;
    colliderShape = CreateCollisionShapeFromModel();
    body = CreateBody(position, rotation, mass);
}

// PRIVATE //

btCollisionShape* phys3d::Model::GenShapeMesh(const Mesh& mesh)
{
    btTriangleMesh* triMesh = new btTriangleMesh(
        mesh.vertexCount >= std::numeric_limits<uint16_t>::max(),
        false);

    triMesh->preallocateVertices(mesh.vertexCount);

    const Vector3 *vertices = reinterpret_cast<Vector3*>(mesh.vertices);

    for (int i = 0; i < mesh.vertexCount; i += 3)
    {
        const Vector3& vertexA = vertices[i];
        const Vector3& vertexB = vertices[i+1];
        const Vector3& vertexC = vertices[i+2];

        triMesh->addTriangle(
            btVector3(vertexA.x, vertexA.y, vertexA.z),
            btVector3(vertexB.x, vertexB.y, vertexB.z),
            btVector3(vertexC.x, vertexC.y, vertexC.z));
    }

    return new btBvhTriangleMeshShape(triMesh, true);
}

btCollisionShape* phys3d::Model::GenShapeHull(const Mesh& mesh)
{
    return new btConvexHullShape(mesh.vertices, mesh.vertexCount, 3 * sizeof(float));
}

btCollisionShape* phys3d::Model::GenShapeCube(const Mesh& mesh)
{
    const raylib::BoundingBox bb(mesh);
    const Vector3 size = (raylib::Vector3(bb.max) - bb.min);

    return new btBoxShape(btVector3(
        btScalar(size.x * 0.5f),
        btScalar(size.y * 0.5f),
        btScalar(size.z * 0.5f)));
}

btCollisionShape* phys3d::Model::GenShapeSphere(const Mesh& mesh)
{
    const raylib::BoundingBox bb(mesh);
    const Vector3 size = (raylib::Vector3(bb.max) - bb.min);

    const float diameter = (size.x > size.y)
        ? ( (size.x > size.z) ? size.x : size.z )
        : ( (size.y > size.z) ? size.y : size.z );

    return new btSphereShape(btScalar(diameter * 0.5f));
}

btCollisionShape* phys3d::Model::GenShapeCapsule(const Mesh& mesh)
{
    const raylib::BoundingBox bb(mesh);
    const Vector3 size = (raylib::Vector3(bb.max) - bb.min);
    const float radius = ( (size.x > size.z) ? size.x : size.z ) * 0.5f;
    return new btCapsuleShape(btScalar(radius), btScalar(size.z * 0.5f));
}

btCollisionShape* phys3d::Model::CreateCollisionShapeFromModel()
{
    btCollisionShape *(phys3d::Model::*generator)(const Mesh& mesh) = nullptr;

    switch (shape)
    {
        case Shape::Mesh:       generator = &Model::GenShapeMesh;       break;
        case Shape::Hull:       generator = &Model::GenShapeHull;       break;
        case Shape::Cube:       generator = &Model::GenShapeCube;       break;
        case Shape::Sphere:     generator = &Model::GenShapeSphere;     break;
        case Shape::Capsule:    generator = &Model::GenShapeCapsule;    break;

        default:
        {
            std::runtime_error("ERROR: Type de forme donné à phys3d::Model non supporté.");
        }
    }

    btCollisionShape *cShape = nullptr;

    if (model->meshCount == 1)
    {
        cShape = (this->*generator)(model->meshes[0]);
    }
    else
    {
        btTransform transform; transform.setIdentity();
        btCompoundShape* compoundShape = new btCompoundShape();

        for (int i = 0; i < model->meshCount; i++)
        {
            compoundShape->addChildShape(transform,
                (this->*generator)(model->meshes[i]));
        }

        cShape = compoundShape;
    }

    return cShape;
}
