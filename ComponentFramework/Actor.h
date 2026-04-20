#pragma once
#include <vector>
#include <iostream>
#include "Component.h"
#include "Matrix.h"
using namespace MATH;

//types of objects (based on possible collision type)
//doesn't necessarily has to be only boxes with type BOX
enum class ActorType {
	SPHERE,
	BOX,
	PLANE,
	LIGHT
};

class Actor : public Component {
	Actor(const Actor&) = delete;
	Actor(Actor&&) = delete;
	Actor& operator= (const Actor&) = delete;
	Actor& operator=(Actor&&) = delete;

protected:
	std::vector<Ref<Component>> components;

public:
	ActorType type;
	Actor(Component* parent_);
	~Actor();
	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;

	void SetType(ActorType type_);
	ActorType GetType() const;

	//Add existing component
	//we check if it's there using .get() (doing it raaaw)
	template<typename ComponentTemplate>
	void AddComponent(Ref<ComponentTemplate> component_) {
		if (GetComponent<ComponentTemplate>().get() != nullptr) {
#ifdef _DEBUG
			std::cerr << "WARNING: Trying to add a component type that is already added - ignored\n";
#endif
			return;
		}
		//here it is
		components.push_back(component_);
	}

	// Add new component				pass the args of new component aswell 
	template<typename ComponentTemplate, typename ... Args>
	void AddComponent(Args&& ... args_) {
		/// before you add the component ask if you have the component in the list already,
		/// if so - don't add a second one. 
		if (GetComponent<ComponentTemplate>().get() != nullptr) {
#ifdef _DEBUG
			std::cerr << "WARNING: Trying to add a component type that is already added - ignored\n";
#endif
			return;
		}
		/// Finish building the component and add the component to the list
		//ya, i don't fully get it, here we go with how we feel about it, no thoughts
		components.push_back(std::make_shared<ComponentTemplate>(std::forward<Args>(args_)...));
	}
	// Getter
	template<typename ComponentTemplate>
	Ref<ComponentTemplate> GetComponent() const {
		for (auto component : components) {
			if (dynamic_cast<ComponentTemplate*>(component.get())) {
				/// This is a dynamic cast designed for shared_ptr's
				/// https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast
				return std::dynamic_pointer_cast<ComponentTemplate>(component);
			}
		}
		return Ref<ComponentTemplate>(nullptr);
	}

	template<typename ComponentTemplate>
	void RemoveComponent() {
		for (size_t i = 0; i < components.size(); i++) {
			if (dynamic_cast<ComponentTemplate*>(components[i].get()) != nullptr) {
				components[i]->OnDestroy();
				components.erase(components.begin() + i);
				break;
			}
		}
	}


	void ListComponents() const {};

	//templated functions
	template<typename ComponentTemplate>
	void RemoveAllComponents();
	Matrix4 GetModelMatrix();
};

