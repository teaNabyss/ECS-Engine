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
	std::unordered_map<std::string, Ref<Component>> componentCatalog;
	std::unordered_map<std::string, Ref<Actor>>     actorCatalog;
	const char* filename;

	//those are called inside ReadManifest
	void BuildComponents(XMLElement* componentsRoot);
	void BuildActors(XMLElement* actorsRoot);

protected:
	std::vector<Ref<Component>> components;

public:
	AssetManager(const char* filename_);
	~AssetManager();
	bool OnCreate();

	template<typename ComponentTemplate>
	void RemoveAllComponents();

	void ReadManifest();

	Ref<Actor> GetActor(const char* name) const;

	const std::unordered_map<std::string, Ref<Actor>>& GetActorCatalog() const {
		return actorCatalog;
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


	template<typename ActorTemplate, typename ... Args>
	void AddActor(const char* name, Args&& ... args_) {
		Ref<ActorTemplate> t = std::make_shared<ActorTemplate>(std::forward<Args>(args_)...);
		actorCatalog[name] = t;
	}
};