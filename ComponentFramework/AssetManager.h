#pragma once
#include <string>
#include <iostream>
#include <unordered_map> 
#include "Component.h"
#include "Actor.h"
#include "Debug.h"
#include "tinyxml2.h"
using namespace tinyxml2;

class AssetManager {
private:
	std::unordered_map<const char*, Ref<Component>> componentCatalog;
	std::unordered_map<const char*, Ref<Actor>>     actorCatalog;
	const char* filename;

	//those are called inside ReadManifest
	void BuildComponents(XMLElement* componentsRoot);
	void BuildActors(XMLElement* actorsRoot);

public:
	AssetManager(const char* filename_);
	~AssetManager();
	bool OnCreate();

	template<typename ComponentTemplate>
	void RemoveAllComponents();

	void ReadManifest();

	Ref<Actor> GetActor(const char* name) const;


	//Add existing component
//we check if it's there using .get() 
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

	template<typename ComponentTemplate, typename ... Args>
	void AddComponent(const char* name, Args&& ... args_) {
		Ref<ComponentTemplate> t = std::make_shared<ComponentTemplate>(std::forward<Args>(args_)...);
		componentCatalog[name] = t;
	}

	template<typename ComponentTemplate>
	Ref<ComponentTemplate> GetComponent(const char* name) const {
		auto id = componentCatalog.find(name);
#ifdef _DEBUG
		if (id == componentCatalog.end()) {
			Debug::Error("Can't find requested component", __FILE__, __LINE__);
			return Ref<ComponentTemplate>(nullptr);
		}
#endif 
		return std::dynamic_pointer_cast<ComponentTemplate>(id->second);
	}

};