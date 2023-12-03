#ifndef RAYFLEX_PHYS_3D_WORLD_HPP
#define RAYFLEX_PHYS_3D_WORLD_HPP

#include <btBulletDynamicsCommon.h>
#include <Vector3.hpp>
#include <iterator>

namespace rf { namespace phys3d {

    class Object;

    /**
     * @brief Class representing a 3D physics world using Bullet3.
     */
    class World
    {
      private:
        btDefaultCollisionConfiguration* collisionConfig;   ///< Configuration for Bullet collision detection stack allocator
        btCollisionDispatcher* dispatcher;                  ///< Collision dispatcher, handles collision events

        btBroadphaseInterface* overlappingPairCache;        ///< Broadphase interface, detects overlapping objects
        btSequentialImpulseConstraintSolver* solver;        ///< Constraint solver

        btDiscreteDynamicsWorld* dynamicsWorld;             ///< The world where physics takes place
        btAlignedObjectArray<Object*> objects;              ///< Vector to store physics objects

      public:
        /**
         * @brief Constructor for the physics world.
         * @param gravity Gravity vector for the world.
         */
        World(const Vector3& gravity = { 0, -9.81, 0 });

        /**
         * @brief Destructor for the physics world.
         */
        ~World();

        /**
         * @brief Adds a physics object to the world.
         * @tparam tObject Type of the object to be added.
         * @tparam Args Variadic template for constructor arguments.
         * @param args Constructor arguments for the object.
         * @return Pointer to the added object.
         */
        template <typename tObject, typename... Args>
        tObject* AddObject(Args... args)
        {
            tObject *object = new tObject(args...);
            dynamicsWorld->addRigidBody(object->body);
            objects.push_back(object);
            return object;
        }

        /**
         * @brief Advances the simulation by a specified time step.
         * @param timeStep Time step for the simulation.
         * @param maxSubSteps Maximum sub-steps.
         * @param fixedTimeStep Fixed time step.
         */
        void Step(btScalar timeStep, int maxSubSteps = 1, btScalar fixedTimeStep = btScalar(0.016666667))
        {
            dynamicsWorld->stepSimulation(timeStep, maxSubSteps, fixedTimeStep); 
        }

        /**
         * @brief Draws the physics objects in the world.
         */
        void Draw() const;

        /**
         * @brief Provides an iterator to the beginning of the world objects.
         * @return Iterator pointing to the first object.
         */
        auto begin() { return &objects[0]; }

        /**
         * @brief Provides an iterator to the end of the world objects.
         * @return Iterator pointing to the last object.
         */
        auto end() { return &objects[objects.size() - 1]; }
    };

}}

#endif //RAYFLEX_PHYS_3D_WORLD_HPP
