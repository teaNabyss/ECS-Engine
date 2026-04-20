#include "AssetManager.h"
#include "CollisionComponent.h"
#include "MaterialComponent.h"
#include "MeshComponent.h"
#include "PhysicsComponent.h"
#include "ShaderComponent.h"
#include "TransformComponent.h"

AssetManager::AssetManager(const char* filename_) {
	filename = filename_;
}

AssetManager::~AssetManager() {}

template<typename ComponentTemplate>
void AssetManager::RemoveAllComponents() {
	componentCatalog.clear();
}
                                                                   

void AssetManager::ReadManifest() {
	std::cout << "ReadManifest\n";
	XMLDocument doc;
	doc.LoadFile("Assets.xml");
	bool status = doc.Error();
	if (status) {
		std::cout << doc.ErrorIDToName(doc.ErrorID()) << std::endl;
		return;
	}
	/// Jump to the first node or "root"
	XMLElement* rootData = doc.RootElement();

	/// Loop over all the elements under the first node
    for (XMLElement* e = rootData->FirstChildElement(); e != nullptr; e = e->NextSiblingElement()) {

        /// Print the name of the element
        std::cout << "Element [" << e->Value() << "]: ";

        /// If there is text within the element (not null), print it
        if (e->GetText() != nullptr) {
            std::cout << e->GetText() << '\n';
        }

        /// loop over all the attributes (if any) 
        for (const XMLAttribute* a = e->FirstAttribute(); a != nullptr; a = a->Next()) {
            std::cout << "Attribute [" << a->Name() << ": " << a->Value() << "] ";
        }
        std::cout << '\n';
    }
}

    void AssetManager::BuildComponents(XMLElement * componentsRoot) {
        for (XMLElement* e = componentsRoot->FirstChildElement("Component");
            e != nullptr; e = e->NextSiblingElement("Component"))
        {
            const char* name = e->Attribute("name");
            const char* type = e->Attribute("type");

            if (!name || !type) continue;

            //this one was annoying
            if (strcmp(type, "CollisionComponent") == 0) {
                const char* shape = e->Attribute("shape");

                if (strcmp(shape, "sphere") == 0) {
                    float r = e->FloatAttribute("radius", 1.0f);
                    AddComponent<CollisionComponent>(name, nullptr, r);
                }
                else if (strcmp(shape, "plane") == 0) {
                    MATHEX::Plane p;
                    p.x = e->FloatAttribute("nx", 0.0f);
                    p.y = e->FloatAttribute("ny", 1.0f);
                    p.z = e->FloatAttribute("nz", 0.0f);
                    p.d = e->FloatAttribute("d", 0.0f);
                    AddComponent<CollisionComponent>(name, nullptr, p);
                }
                else if (strcmp(shape, "aabb") == 0) {
                    AABB box;
                    box.center.x = e->FloatAttribute("cx", 0.0f);
                    box.center.y = e->FloatAttribute("cy", 0.0f);
                    box.center.z = e->FloatAttribute("cz", 0.0f);
                    box.half.x = e->FloatAttribute("hx", 1.0f);
                    box.half.y = e->FloatAttribute("hy", 1.0f);
                    box.half.z = e->FloatAttribute("hz", 1.0f);
                    AddComponent<CollisionComponent>(name, nullptr, box);
                }
            }
            //this one's not
            else if (strcmp(type, "MeshComponent") == 0) {
                const char* meshFile = e->Attribute("mesh");
                AddComponent<MeshComponent>(name, nullptr, meshFile);
            }
            else if (strcmp(type, "MeshComponent") == 0) {
                const char* textureFile = e->Attribute("mesh");
                AddComponent<MeshComponent>(name, nullptr, textureFile);
            }

        }
    }

    void AssetManager::BuildActors(XMLElement * actorsRoot) {
        for (XMLElement* e = actorsRoot->FirstChildElement("Actor");
            e != nullptr; e = e->NextSiblingElement("Actor"))
        {
            const char* name = e->Attribute("name");
            if (!name) continue;

            Ref<Actor> actor = std::make_shared<Actor>();

            // Attach whichever components are listed as attributes
            const char* meshName = e->Attribute("mesh");
            if (meshName) {
                actor->AddComponent(GetComponent<MeshComponent>(meshName));
            }

            const char* collisionName = e->Attribute("collision");
            if (collisionName) {
                actor->AddComponent(GetComponent<CollisionComponent>(collisionName));
            }

            actorCatalog[name] = actor;
        }
    }

Ref<Actor> AssetManager::GetActor(const char* name) const {
    auto id = actorCatalog.find(name);
     if (id == actorCatalog.end()) {
        Debug::Error("Can't find requested actor", __FILE__, __LINE__);           
        return nullptr;
     }

     return id->second;
}