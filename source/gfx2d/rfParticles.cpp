#include "gfx2d/rfParticles.hpp"
#include <utility>
#include <ctime>

using namespace rf;

/* PARTICLE SYSTEM */

gfx2d::ParticleSystem::ParticleSystem(uint32_t maxParticles)
: position({0,0})
, minVel({-10.0f, -10.0f})
, maxVel({10.0f, 10.0f})
, minRadius(1.0f)
, maxRadius(2.0f)
, lifeTime(1.0f)
, gravity(9.81f)
, color(WHITE)
, gen(std::time(nullptr))
, particles(nullptr)
, numParticles(0)
, maxParticles(maxParticles)
{
    velXDistribution = std::uniform_real_distribution<float>(this->minVel.x, this->maxVel.x);
    velYDistribution = std::uniform_real_distribution<float>(this->minVel.y, this->maxVel.y);
    radiusDistribution = std::uniform_real_distribution<float>(minRadius, maxRadius);
    particles = new Particle[maxParticles];
    numParticles = 0;
}

gfx2d::ParticleSystem::~ParticleSystem()
{
    delete[] particles;
    maxParticles = 0;
    numParticles = 0;
}

gfx2d::ParticleSystem::ParticleSystem(ParticleSystem&& other) noexcept
{
    // Transférez les ressources de 'other' vers 'this'
    position = std::move(other.position);
    minVel = std::move(other.minVel);
    maxVel = std::move(other.maxVel);
    minRadius = other.minRadius;
    maxRadius = other.maxRadius;
    lifeTime = other.lifeTime;
    gravity = other.gravity;
    color = other.color;
    gen = std::move(other.gen);
    velXDistribution = std::move(other.velXDistribution);
    velYDistribution = std::move(other.velYDistribution);
    radiusDistribution = std::move(other.radiusDistribution);
    particles = std::exchange(other.particles, nullptr);
    maxParticles = std::exchange(other.maxParticles, 0);
    numParticles = std::exchange(other.numParticles, 0);
}

gfx2d::ParticleSystem& gfx2d::ParticleSystem::operator=(gfx2d::ParticleSystem&& other) noexcept
{
    if (this != &other)
    {
        position = std::move(other.position);
        minVel = std::move(other.minVel);
        maxVel = std::move(other.maxVel);
        minRadius = other.minRadius;
        maxRadius = other.maxRadius;
        lifeTime = other.lifeTime;
        gravity = other.gravity;
        color = other.color;
        gen = std::move(other.gen);
        velXDistribution = std::move(other.velXDistribution);
        velYDistribution = std::move(other.velYDistribution);
        radiusDistribution = std::move(other.radiusDistribution);
        particles = other.particles;
        maxParticles = other.maxParticles;
        numParticles = other.numParticles;

        other.particles = nullptr;
        other.maxParticles = 0;
        other.numParticles = 0;
    }
    return *this;
}

void gfx2d::ParticleSystem::Emit(uint32_t num)
{
    for (uint8_t i = 0; i < num && numParticles < maxParticles; i++)
    {
        const Vector2 velocity = { velXDistribution(gen), velYDistribution(gen) };
        const float radius = radiusDistribution(gen);

        particles[numParticles++] = {
            .position   = this->position,
            .velocity   = velocity,
            .color      = this->color,
            .lifeTime   = this->lifeTime,
            .time       = this->lifeTime,
            .radius     = radius
        };
    }
}

void gfx2d::ParticleSystem::Update(float dt)
{
    for (int i = numParticles-1; i >= 0; i--)
    {
        if (!(particles[i].Update(gravity, dt)))
        {
            particles[i] = particles[--numParticles];
        }
    }
}

void gfx2d::ParticleSystem::Draw() const
{
    for (int i = 0; i < numParticles; i++)
    {
        particles[i].Draw();
    }
}
