#ifndef RAYFLEX_PHYS_2D_HPP
#define RAYFLEX_PHYS_2D_HPP

#include <box2d/box2d.h>
#include <Color.hpp>

namespace rf { namespace phys2d {

    using World = b2World;
    using Body = b2Body;
    using Shape = b2Shape;
    using Fixture = b2Fixture;
    using Joint = b2Joint;
    using DistanceJoint = b2DistanceJoint;
    using FrictionJoint = b2FrictionJoint;
    using GearJoint = b2GearJoint;
    using MotorJoint = b2MotorJoint;
    using MouseJoint = b2MouseJoint;
    using PrismaticJoint = b2PrismaticJoint;
    using PulleyJoint = b2PulleyJoint;
    using RevoluteJoint = b2RevoluteJoint;
    using WeldJoint = b2WeldJoint;
    using WheelJoint = b2WheelJoint;

    using BodyDef = b2BodyDef;
    using FixtureDef = b2FixtureDef;
    using JointDef = b2JointDef;
    using DistanceJointDef = b2DistanceJointDef;
    using FrictionJointDef = b2FrictionJointDef;
    using GearJointDef = b2GearJointDef;
    using MotorJointDef = b2MotorJointDef;
    using MouseJointDef = b2MouseJointDef;
    using PrismaticJointDef = b2PrismaticJointDef;
    using PulleyJointDef = b2PulleyJointDef;
    using RevoluteJointDef = b2RevoluteJointDef;
    using WeldJointDef = b2WeldJointDef;
    using WheelJointDef = b2WheelJointDef;

    using Vector2 = b2Vec2;
    using Vector3 = b2Vec3;
    using Transform = b2Transform;

    using PolygonShape = b2PolygonShape;
    using CircleShape = b2CircleShape;
    using EdgeShape = b2EdgeShape;
    using ChainShape = b2ChainShape;

    using Contact = b2Contact;
    using Filter = b2Filter;

    /* DEBUG FUNCTIONS */

    void DrawShape(const Shape* shape, const Transform& transform, const Color& color = WHITE);
    void DrawBody(const Body* body);
    void DrawWorld(const World* world);

}}

#endif //RAYFLEX_PHYS_2D_HPP
