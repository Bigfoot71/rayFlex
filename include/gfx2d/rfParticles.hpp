#ifndef RAYFLEX_GFX_2D_PARTICLES_HPP
#define RAYFLEX_GFX_2D_PARTICLES_HPP
#ifdef SUPPORT_GFX_2D

#include <Vector2.hpp>
#include <random>

namespace rf { namespace gfx2d {

    /**
     * @brief Structure representing a 2D particle with position, velocity, color, lifetime, and radius.
     */
    struct Particle
    {
        raylib::Vector2 position;   ///< The position of the particle.
        raylib::Vector2 velocity;   ///< The velocity of the particle.
        Color color;                ///< The color of the particle.
        float lifeTime;             ///< The total lifetime of the particle.
        float time;                 ///< The remaining lifetime of the particle.
        float radius;               ///< The radius of the particle.

        /**
         * @brief Updates the particle's position and lifetime.
         * @param gravity The gravitational force affecting the particle.
         * @param dt The time step for the update.
         * @return True if the particle is still alive, false if it has expired.
         */
        bool Update(float gravity, float dt)
        {
            position += velocity * dt;
            velocity.y += gravity * dt;
            return (time -= dt) > 0.0f;
        }

        /**
         * @brief Draws the particle on the screen.
         */
        void Draw() const
        {
            position.DrawCircle(radius, ColorAlpha(color, time / lifeTime));
        }
    };

    /**
     * @brief Class for managing a system of 2D particles.
     */
    class ParticleSystem
    {
      private:
        Vector2 position;                                           ///< The emission position for new particles.
        Vector2 minVel, maxVel;                                     ///< The range of initial velocities for new particles.
        float minRadius, maxRadius;                                 ///< The range of radii for new particles.
        float lifeTime;                                             ///< The lifetime of each emitted particle.
        float gravity;                                              ///< The gravitational force affecting the particles.
        Color color;                                                ///< The color of the emitted particles.
        std::mt19937 gen;                                           ///< Random number generator.
        std::uniform_real_distribution<float> velXDistribution;     ///< Distribution for randomizing particle X velocity.
        std::uniform_real_distribution<float> velYDistribution;     ///< Distribution for randomizing particle Y velocity.
        std::uniform_real_distribution<float> radiusDistribution;   ///< Distribution for randomizing particle radius.
        Particle* particles;                                        ///< Array to hold active particles.
        uint32_t numParticles;                                      ///< The current number of active particles.
        uint32_t maxParticles;                                      ///< The maximum number of particles allowed.

      public:
        /**
         * @brief Default constructor for the ParticleSystem class.
         */
        ParticleSystem() : particles(nullptr), numParticles(0), maxParticles(0) { }

        /**
         * @brief Parameterized constructor for the ParticleSystem class.
         * @param maxParticles The maximum number of particles allowed in the system.
         */
        ParticleSystem(uint32_t maxParticles);

        /**
         * @brief Destructor for the ParticleSystem class.
         */
        ~ParticleSystem();

        /**
         * @brief Deleted copy constructor to prevent copying ParticleSystem instances.
         */
        ParticleSystem(const ParticleSystem&) = delete;

        /**
         * @brief Deleted copy assignment operator to prevent copying ParticleSystem instances.
         */
        ParticleSystem& operator=(const ParticleSystem&) = delete;

        /**
         * @brief Move constructor for the ParticleSystem class.
         * @param other The ParticleSystem instance to move.
         */
        ParticleSystem(ParticleSystem&& other) noexcept;

        /**
         * @brief Move assignment operator for the ParticleSystem class.
         * @param other The ParticleSystem instance to move.
         * @return A reference to the moved ParticleSystem instance.
         */
        ParticleSystem& operator=(ParticleSystem&& other) noexcept;

        /**
         * @brief Sets the emission position for new particles.
         * @param position The emission position.
         */
        void SetPosition(const Vector2& position)
        {
            this->position = position;
        }

        /**
         * @brief Sets the lifetime for each emitted particle.
         * @param lifeTime The lifetime value.
         */
        void SetLifeTime(float lifeTime)
        {
            this->lifeTime = lifeTime;
        }

        /**
         * @brief Sets the gravitational force affecting the particles.
         * @param gravity The gravitational force.
         */
        void SetGravity(float gravity)
        {
            this->gravity = gravity;
        }

        /**
         * @brief Sets the color of emitted particles.
         * @param color The color of emitted particles.
         */
        void SetColor(const Color& color)
        {
            this->color = color;
        }

        /**
         * @brief Sets the range of radii for new particles.
         * @param minRadius The minimum radius.
         * @param maxRadius The maximum radius.
         */
        void SetRadius(float minRadius, float maxRadius)
        {
            radiusDistribution = std::uniform_real_distribution<float>(minRadius, maxRadius);
            this->minRadius = minRadius, this->maxRadius = maxRadius;
        }

        /**
         * @brief Sets the range of initial velocities for new particles.
         * @param minVel The minimum velocity.
         * @param maxVel The maximum velocity.
         */
        void SetVelocity(const Vector2& minVel, const Vector2& maxVel)
        {
            velXDistribution = std::uniform_real_distribution<float>(minVel.x, maxVel.x);
            velYDistribution = std::uniform_real_distribution<float>(minVel.y, maxVel.y);
            this->minVel = minVel, this->maxVel = maxVel;
        }

        /**
         * @brief Clears the particle system by resetting the number of particles to zero.
         */
        void Clear()
        {
            numParticles = 0;
        }

        /**
         * @brief Emits a specified number of new particles.
         * @param num The number of particles to emit.
         */
        void Emit(uint32_t num = 1);

        /**
         * @brief Updates the state of all active particles.
         * @param dt The time step for the update.
         */
        void Update(float dt);

        /**
         * @brief Draws all active particles on the screen.
         */
        void Draw() const;
    };

}}

#endif //SUPPORT_GFX_2D
#endif //RAYFLEX_GFX_2D_PARTICLES_HPP
