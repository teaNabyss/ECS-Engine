#pragma once
#include "Component.h"
#include "Plane.h"
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

	public:
		CollisionComponent(Component* parent_, float radius_);
		CollisionComponent(Component* parent_, MATHEX::Plane plane_);
		CollisionComponent(Component* parent_, AABB aabb_);
		bool OnCreate() { return true; }
		void OnDestroy() {}
		void Update(const float deltaTime_) {}
		void Render()const {}
};

