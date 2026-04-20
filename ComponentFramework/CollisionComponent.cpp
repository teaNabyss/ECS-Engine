#include "CollisionComponent.h"

CollisionComponent::CollisionComponent(Component* parent_, float radius_)
    : Component(parent_), radius(radius_)
{}

CollisionComponent::CollisionComponent(Component* parent_, MATHEX::Plane plane_)
    : Component(parent_), collidertype(ColliderType::PLANE), plane(plane_)
{}

CollisionComponent::CollisionComponent(Component* parent_, AABB aabb_)
    : Component(parent_), collidertype(ColliderType::AABB), aabb(aabb_)
{}
