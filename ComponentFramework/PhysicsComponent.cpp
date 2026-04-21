#include "PhysicsComponent.h"
#include <QMath.h>
#include <MMath.h>
#include "Debug.h"

PhysicsComponent::PhysicsComponent(Component* parent_, float mass_)
	: Component(parent_), mass(mass_), vel(), acc(), angularVel(), angularAcc()
{
	Actor* actor = dynamic_cast<Actor*>(parent_);
	if (actor) {
		tc = actor->GetComponent<TransformComponent>();
		if (tc) {
			Vec3 p = tc->GetPosition();
			std::cout << "tc found, pos: " << p.x << ", " << p.y << ", " << p.z << std::endl;
		}
		else {
			std::cout << "tc is NULL" << std::endl;
		}
	}
	else {
		std::cout << "parent is not an Actor or is null" << std::endl;
	}
}
//here goes Umer Noor's stuff
void PhysicsComponent::Update(float deltaTime) {
	tc->Translate(vel * deltaTime + 0.5f * acc * deltaTime * deltaTime);
	vel += acc * deltaTime;
	angularVel += angularAcc * deltaTime;
	UpdateOrientation(deltaTime);
}

void PhysicsComponent::ApplyTorque(Vec3 torque)
{
	float r = tc->GetScale().x;
	rotationalInertia = (2.0f / 5.0f) * mass * r * r;

	if (fabs(rotationalInertia) > VERY_SMALL) {
		angularAcc = torque / rotationalInertia;
	}
}

void PhysicsComponent::UpdateOrientation(float deltaTime)
{
	// Remember, the direction of the angVel is the rotn axis
	// Magnitude is the angle
	float angleRadians = VMath::mag(angularVel) * deltaTime; 
	// If angle is zero, get outta here
	if (angleRadians < VERY_SMALL) { // BEWARE! Don't use == here for floats
		return;
	}
	Vec3 axis = VMath::normalize(angularVel);
	float angleDegrees = angleRadians * RADIANS_TO_DEGREES;
	tc->Rotate(QMath::angleAxisRotation(angleDegrees, axis));
}

void PhysicsComponent::UpdateAngularVel(Vec3 surfaceNormal)
{
	float r = tc->GetScale().x;  
	angularVel = VMath::cross(vel, surfaceNormal) / r;
}
