#include <iostream>
#include "TransformComponent.h"
#include "MMath.h"
#include "QMath.h"
#include "Actor.h"
using namespace MATH;

TransformComponent::TransformComponent(Component* parent_) :Component(parent_) {
	pos = Vec3(0.0f, 0.0f, 0.0f);
	orientation = Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0f));
	scale = Vec3(1.0f, 1.0f, 1.0f);

}

TransformComponent::TransformComponent(Component* parent_, Vec3 pos_, Quaternion orientation_, Vec3 scale_) :
	Component{ parent_ }, pos{ pos_ }, orientation{ orientation_ }, scale{ scale_ } {}

TransformComponent::~TransformComponent() {}

bool TransformComponent::OnCreate() {
	if (isCreated == true) return true;
	isCreated = true;
	return true;
}

void TransformComponent::OnDestroy() {}

void TransformComponent::Update(const float deltatime){}

void TransformComponent::Render()const {}

Matrix4 TransformComponent::GetTransformMatrix() const {
    return MMath::translate(pos) * MMath::scale(scale) * MMath::toMatrix4(orientation);
}

Matrix4 TransformComponent::GetModelMatrix() const {
    Matrix4 ModelMatrix = GetTransformMatrix();

    if (parent) {
        auto* parentActor = dynamic_cast<Actor*>(parent);
        if (parentActor) {
            auto transform = parentActor->GetComponent<TransformComponent>();
            if (transform) {
                ModelMatrix = transform->GetModelMatrix() * ModelMatrix;
            }
        }
    }

    return ModelMatrix;
}