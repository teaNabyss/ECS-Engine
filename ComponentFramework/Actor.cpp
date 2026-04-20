#include "Actor.h"
#include "Debug.h"
#include "TransformComponent.h"
#include "MMath.h"
Actor::Actor(Component* parent_) :Component(parent_) {}

bool Actor::OnCreate() {
	if (isCreated) return true;
	Debug::Info("Loading assets for Actor: ", __FILE__, __LINE__);
	for (auto component : components) {
		if (component->OnCreate() == false) {
			Debug::Error("Loading assets for Actor/Components: ", __FILE__, __LINE__);
			isCreated = false;
			return isCreated;
		}
	}
	isCreated = true;
	return isCreated;
}

Actor::~Actor() {
	OnDestroy();
}

void Actor::OnDestroy() {
	Debug::Info("Deleting assets for Actor: ", __FILE__, __LINE__);
}



void Actor::Update(const float deltaTime) {
	std::cout << "Hello from Update\n";
}

void Actor::Render()const {}

void Actor::SetType(ActorType type_) {
	type = type_;
}

ActorType Actor::GetType() const {
	return type;
}

template<typename ComponentTemplate>
void Actor::RemoveAllComponents() {
	components.clear();
}

void Actor::ListComponents() const {
	std::cout << typeid(*this).name() << " contains the following components:\n";
	for (const auto& component : components) {
		std::cout << typeid(*component).name() << std::endl;
	}
	std::cout << '\n';
}

/*
Matrix4 Actor::GetModelMatrix() {
	TransformComponent* transform = GetComponent<TransformComponent>();
	Matrix4 modelMatrix;

	if (transform) {
		modelMatrix = transform->GetTransformMatrix();
	}
	else {
		modelMatrix.loadIdentity();
	}
	if (parent) { /// <== LOOK AT IT! Think!
		modelMatrix = dynamic_cast<Actor*>(parent)->GetModelMatrix() * modelMatrix;
	}
	return modelMatrix;
}*/
