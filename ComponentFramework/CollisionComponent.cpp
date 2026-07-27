#include "CollisionComponent.h"
#include "PhysicsComponent.h"

Vec3 CollisionComponent::GetPosition() {
    if (!tc) {
        Actor* actor = dynamic_cast<Actor*>(parent);
        if (actor) tc = actor->GetComponent<TransformComponent>();
    }
    return tc ? tc->GetPosition() : Vec3();
}

Ref<PhysicsComponent> CollisionComponent::GetPhysicsComponent() {
    Actor* actor = dynamic_cast<Actor*>(parent);
    return actor ? actor->GetComponent<PhysicsComponent>() : nullptr;
}

CollisionComponent::CollisionComponent(Component* parent_, float radius_)
    : Component(parent_), collidertype(ColliderType::SPHERE), radius(radius_)
{
}
CollisionComponent::CollisionComponent(Component* parent_, MATHEX::Plane plane_)
    : Component(parent_), collidertype(ColliderType::PLANE), plane(plane_)
{}

CollisionComponent::CollisionComponent(Component* parent_, AABB aabb_)
    : Component(parent_), collidertype(ColliderType::AABB), aabb(aabb_)
{}
