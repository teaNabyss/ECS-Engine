#pragma once
#include "Component.h"
#include "Plane.h"
#include "Actor.h"
#include "TransformComponent.h"
class PhysicsComponent;
using namespace MATH;

enum class ColliderType {
	SPHERE,
	AABB,
	PLANE
};
struct AABB {
	Vec3 center;
	Vec3 half;
	};

class CollisionComponent : public Component {
	friend class CollisionSystem;
	CollisionComponent(const CollisionComponent&) = delete;
	CollisionComponent(CollisionComponent&&) = delete;
	CollisionComponent& operator = (const CollisionComponent&) = delete;
	CollisionComponent& operator = (CollisionComponent&&) = delete;
	protected:

			ColliderType collidertype;
			float radius;
			MATHEX::Plane plane;
			AABB aabb;
			Ref<TransformComponent> tc;

	public:
		CollisionComponent(Component* parent_, float radius_);
		CollisionComponent(Component* parent_, MATHEX::Plane plane_);
		CollisionComponent(Component* parent_, AABB aabb_);
		bool OnCreate() { return true; }
		void OnDestroy() {}
		void Update(const float deltaTime_) {}
		void Render()const {}

		//this is not a position of an object in world
		//which stores Transform component
		//this is a position of collider 
		// for example: in sphere collider position is in center, 
		// but position of an actor that has sphere collider (character)
		// may be at their feet
		Vec3 GetPosition(); 
		float GetRadius() { return radius; }

		Ref<PhysicsComponent> GetPhysicsComponent();
};

