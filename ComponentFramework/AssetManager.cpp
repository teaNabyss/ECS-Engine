#include "AssetManager.h"
#include "CameraActor.h"
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
    doc.LoadFile(filename);

    if (doc.Error()) {
        std::cout << doc.ErrorIDToName(doc.ErrorID()) << std::endl;
        return;
    }

    XMLElement* root = doc.RootElement();

    XMLElement* componentsNode = root->FirstChildElement("Components");
    if (componentsNode) BuildComponents(componentsNode);

    XMLElement* actorsNode = root->FirstChildElement("Actors");
    if (actorsNode) BuildActors(actorsNode);
}
    // build each component declared in file
    void AssetManager::BuildComponents(XMLElement * componentsRoot) {
        for (XMLElement* e = componentsRoot->FirstChildElement("Component");
            e != nullptr; e = e->NextSiblingElement("Component"))
        {
            const char* name = e->Attribute("name");
            const char* type = e->Attribute("type");

            if (!name || !type) continue;

            //Each component recievs different variables
            //Going through each type of component and save it's info in catalog
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
            else if (strcmp(type, "MeshComponent") == 0) {
                const char* meshFile = e->Attribute("mesh");
                AddComponent<MeshComponent>(name, nullptr, meshFile);
            }
            else if (strcmp(type, "MaterialComponent") == 0) {
                const char* textureFile = e->Attribute("texture");
                AddComponent<MaterialComponent>(name, nullptr, textureFile);
            }
            else if (strcmp(type, "PhysicsComponent") == 0) {
                float mass = e->FloatAttribute("mass");
                AddComponent<PhysicsComponent>(name, nullptr, mass);
            }
            else if (strcmp(type, "ShaderComponent") == 0) {
                const char* vert = e->Attribute("vert");
                const char* frag = e->Attribute("frag");
                const char* tessCtrl = e->Attribute("tessCtrl");  
                const char* tessEval = e->Attribute("tessEval");  
                const char* geom = e->Attribute("geom");      

                AddComponent<ShaderComponent>(name, nullptr, vert, frag, tessCtrl, tessEval, geom);
            }
            else if (strcmp(type, "TransformComponent") == 0) {
                Vec3 pos{
                    e->FloatAttribute("px", 0.0f),
                    e->FloatAttribute("py", 0.0f),
                    e->FloatAttribute("pz", 0.0f)
                };
                Vec3 axis{
                    e->FloatAttribute("ox", 0.0f),
                    e->FloatAttribute("oy", 0.0f),
                    e->FloatAttribute("oz", 0.0f)
                };
                Quaternion orientation{
                    e->FloatAttribute("ow", 1.0f),  // w first
                    axis
                };                Vec3 scale{
                    e->FloatAttribute("sx", 1.0f),
                    e->FloatAttribute("sy", 1.0f),
                    e->FloatAttribute("sz", 1.0f)
                };

                AddComponent<TransformComponent>(name, nullptr, pos, orientation, scale);
            }

        }
    }

    //loop throuh all actors and add components 
    void AssetManager::BuildActors(XMLElement* actorsRoot) {
        for (XMLElement* e = actorsRoot->FirstChildElement("Actor");
            e != nullptr; e = e->NextSiblingElement("Actor"))
        {
            const char* name = e->Attribute("name");
            const char* type = e->Attribute("type");
            if (!name || !type) continue;

            std::string typeStr = type;

            if (typeStr == "CameraActor") {
                float fovy = e->FloatAttribute("fovy", 45.0f);
                float aspect = e->FloatAttribute("aspect", 16.0f / 9.0f);
                float near = e->FloatAttribute("near", 0.5f);
                float far = e->FloatAttribute("far", 100.0f);

                AddActor<CameraActor>(name, nullptr, fovy, aspect, near, far);
            }

            // i don't like this way
            else if (typeStr == "CheckerSet") {
                int startRow = e->IntAttribute("startRow", 0);
                int endRow = e->IntAttribute("endRow", 3);
                float spacing = e->FloatAttribute("spacing", 1.0f);
                const char* mesh = e->Attribute("mesh");
                const char* material = e->Attribute("material");
                const char* shader = e->Attribute("shader");

                for (int row = startRow; row < endRow; row++) {
                    for (int col = 0; col < 8; col++) {
                        if ((row + col) % 2 == 0) continue;

                        std::string pieceName = std::string(name) + "_" +
                            std::to_string(row) + "_" +
                            std::to_string(col);

                        Ref<Actor> actor = std::make_shared<Actor>(nullptr);
                        if (mesh)     actor->AddComponent(GetComponent<MeshComponent>(mesh));
                        if (material) actor->AddComponent(GetComponent<MaterialComponent>(material));
                        if (shader)   actor->AddComponent(GetComponent<ShaderComponent>(shader));

                        Vec3 pos{ (col - 3.5f) * spacing, -1.5f, (row - 3.5f + -5.0f) * spacing };                        
                        Quaternion orientation = QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));
                        Vec3 scale{ 0.08f, 0.08f, 0.08f };
                        actor->AddComponent<TransformComponent>(nullptr, pos, orientation, scale);

                        actor->OnCreate();  
                        actorCatalog[pieceName] = actor;
                    }
                }
            }
            else if (typeStr == "Actor"){
                // regular Actor with components attached
                Ref<Actor> actor = std::make_shared<Actor>(nullptr);

                const char* collision = e->Attribute("collision");
                const char* mesh = e->Attribute("mesh");
                const char* material = e->Attribute("material");
                const char* physics = e->Attribute("physics");
                const char* shader = e->Attribute("shader");
                const char* transform = e->Attribute("transform");

                // if there is such component, we will add it
                if (collision) actor->AddComponent(GetComponent<CollisionComponent>(collision));
                if (mesh)      actor->AddComponent(GetComponent<MeshComponent>(mesh));
                if (material)  actor->AddComponent(GetComponent<MaterialComponent>(material));
                if (physics)   actor->AddComponent(GetComponent<PhysicsComponent>(physics));
                if (shader)    actor->AddComponent(GetComponent<ShaderComponent>(shader));
                if (transform) actor->AddComponent(GetComponent<TransformComponent>(transform));

                actor->OnCreate();
                actorCatalog[name] = actor;
            }
        }
    }Ref<Actor> AssetManager::GetActor(const char* name) const {
    auto id = actorCatalog.find(name);
     if (id == actorCatalog.end()) {
        Debug::Error("Can't find requested actor", __FILE__, __LINE__);           
        return nullptr;
     }

     return id->second;
}