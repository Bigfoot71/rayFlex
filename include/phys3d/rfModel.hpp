#ifndef RAYFLEX_PHYS_3D_MODEL_HPP
#define RAYFLEX_PHYS_3D_MODEL_HPP
#ifdef SUPPORT_PHYS_3D

#include "./rfObject.hpp"

namespace rf { namespace phys3d {

    /**
     * @brief Class representing a 3D physics model object.
     */
    class Model : public Object
    {
      private:
        raylib::Texture2D *mapDiffuse = nullptr;
        bool allocatedMapDiffuse = false;
        bool allocatedModel = false;

      private:
        /**
         * @brief Generates a Bullet collision shape from a mesh.
         * @param mesh The mesh from which to generate the shape.
         * @return Pointer to the generated collision shape.
         */
        btCollisionShape* GenShapeMesh(const Mesh& mesh);

        /**
         * @brief Generates a Bullet collision shape from a convex hull.
         * @param mesh The mesh from which to generate the shape.
         * @return Pointer to the generated collision shape.
         */
        btCollisionShape* GenShapeHull(const Mesh& mesh);

        /**
         * @brief Generates a Bullet collision shape for a cube.
         * @param mesh The mesh from which to generate the shape.
         * @return Pointer to the generated collision shape.
         */
        btCollisionShape* GenShapeCube(const Mesh& mesh);

        /**
         * @brief Generates a Bullet collision shape for a sphere.
         * @param mesh The mesh from which to generate the shape.
         * @return Pointer to the generated collision shape.
         */
        btCollisionShape* GenShapeSphere(const Mesh& mesh);

        /**
         * @brief Generates a Bullet collision shape for a capsule.
         * @param mesh The mesh from which to generate the shape.
         * @return Pointer to the generated collision shape.
         */
        btCollisionShape* GenShapeCapsule(const Mesh& mesh);

        /**
         * @brief Creates a Bullet collision shape from the model.
         * @return Pointer to the created collision shape.
         */
        btCollisionShape* CreateCollisionShapeFromModel();

      public:
        /**
         * @brief Constructor for the Model class with an existing raylib model.
         * @param position Initial position of the model.
         * @param rotation Initial rotation of the model.
         * @param _model Pointer to the raylib model.
         * @param _shape Shape of the physics model.
         * @param mass Mass of the model (Static for static models).
         * @param _color Color of the model.
         */
        Model(const Vector3& position,
              const Vector3& rotation,
              raylib::Model* _model,
              Shape _shape = Shape::Mesh,
              float mass = Static,
              const Color& _color = WHITE);

        /**
         * @brief Constructor for the Model class with a model loaded from a file.
         * @param position Initial position of the model.
         * @param rotation Initial rotation of the model.
         * @param modelPath Path to the model file.
         * @param diffusePath Path to the diffuse texture file.
         * @param _shape Shape of the physics model.
         * @param mass Mass of the model (Static for static models).
         * @param _color Color of the model.
         */
        Model(const Vector3& position,
              const Vector3& rotation,
              const std::string& modelPath,
              const std::string& diffusePath,
              Shape _shape = Shape::Mesh,
              float mass = Static,
              const Color& _color = WHITE);

        /**
         * @brief Destructor for the Model class.
         */
        ~Model()
        {
            if (allocatedModel)
            {
                UnloadModel();
            }
            if (allocatedMapDiffuse && mapDiffuse != nullptr)
            {
                delete mapDiffuse;
            }
        }
    };

}}

#endif //SUPPORT_PHYS_3D
#endif //RAYFLEX_PHYS_3D_MODEL_HPP
