#include "phys3d/rfObject.hpp"
#include "phys3d/rfWorld.hpp"

using namespace rf;

phys3d::World::World(const Vector3& gravity)
{
    collisionConfig = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfig);

    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver;

    dynamicsWorld = new btDiscreteDynamicsWorld(
        dispatcher, overlappingPairCache,
        solver, collisionConfig);
    
    dynamicsWorld->setGravity(
        btVector3(gravity.x, gravity.y, gravity.z));
}

phys3d::World::~World()
{
    for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        dynamicsWorld->removeCollisionObject(obj);
    }

    // Destroy objects
    for (int i = 0; i < objects.size(); i++)
    {
        delete objects[i];
    }

    // Destroy world
    delete dynamicsWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfig;
}

void phys3d::World::Draw() const
{
    for (int i = 0; i < objects.size(); i++)
    {
        objects.at(i)->Draw();
    }
}