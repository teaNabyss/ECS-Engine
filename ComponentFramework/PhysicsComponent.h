#pragma once
#include "TransformComponent.h"
#include "Component.h"
#include "Actor.h"

using namespace MATH;

class PhysicsComponent : public Component {
    friend class CollisionSystem;
private:
    Vec3  vel;
    Vec3 angularVel;
    Vec3  acc; // = Vec3(0.0f, -9.8f, 0.0f);
    Vec3 angularAcc;
    float mass;
    float rotationalInertia;
    Ref <TransformComponent> tc;

public:
    PhysicsComponent(Component* parent_, float mass_);
    void SetVelocity(Vec3 vel_) { vel = vel_; }
    Vec3 GetVelocity() { return vel; };
    void Update(float deltaTime);
    void ApplyTorque(Vec3 torque);
    void UpdateOrientation(float deltaTime);
    void UpdateAngularVel(Vec3 surfaceNormal);

    bool OnCreate() override;
    void OnDestroy() override;
    void Render() const override;

};