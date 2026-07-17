#pragma once
#include "TransformComponent.h"
#include "CollisionComponent.h"
#include "Component.h"
#include "Actor.h"

using namespace MATH;

class PhysicsComponent : public Component {

private:
    Vec3  vel;
    Vec3 angularVel;
    Vec3  acc; // = Vec3(0.0f, -9.8f, 0.0f);
    Vec3 angularAcc;
    float mass;
    float rotationalInertia;
    Ref <TransformComponent> tc;
    Ref<CollisionComponent> cc;

    bool isGrounded = false;
    Vec3 groundNormal = Vec3(0.0f, 1.0f, 0.0f);
public:
    PhysicsComponent(Component* parent_, float mass_);

    void SetVelocity(Vec3 vel_) { vel = vel_; }
    Vec3 GetVelocity() { return vel; };

    void SetTransform(Ref<TransformComponent> tc_) { tc = tc_; }
    float GetMass() { return mass; }

    void SetGrounded(bool grounded) { isGrounded = grounded; }
    bool IsGrounded() { return isGrounded; }
    void SetGroundNormal(Vec3 n) { groundNormal = n; }
    Vec3 GetGroundNormal() { return groundNormal; }

    void Update(float deltaTime);
    void UpdateRolling(float deltaTime);
    void ApplyTorque(Vec3 torque);
    void UpdateOrientation(float deltaTime);
    void UpdateAngularVel(Vec3 surfaceNormal);

    bool OnCreate() override;
    void OnDestroy() override;
    void Render() const override;

};