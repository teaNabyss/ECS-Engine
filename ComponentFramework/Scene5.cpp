#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Scene5.h"
#include <MMath.h>
#include "Debug.h"

#include "MaterialComponent.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"

Scene5::Scene5() :
	drawInWireMode{ false }, assetManager{nullptr}, camera{ nullptr } {
	Debug::Info("Created Scene5: ", __FILE__, __LINE__);
}

Scene5::~Scene5() {
	Debug::Info("Deleted Scene5: ", __FILE__, __LINE__);
}

bool Scene5::OnCreate() {
	lightPos1 = Vec3(0.0f, 5.0f, 0.0f);
	lightPos2 = Vec3(0.0f, 5.0f, -15.0f);

	assetManager = std::make_shared<AssetManager>("XML/SkyBoxDEBUG.xml");
	assetManager->ReadManifest();

	for (auto& pair : assetManager->GetActorCatalog()) {
		Ref<Actor> actor = std::dynamic_pointer_cast<Actor>(pair.second);
		if (actor) {
			actors[pair.first] = actor;
			std::cout << "Added actor: " << pair.first << "\n";
		}
	}


	for (auto& pair : actors) {
		Ref<Actor> actor = pair.second;
		Ref<TransformComponent> tc = actor->GetComponent<TransformComponent>();
		std::cout << pair.first << " has transform: " << (tc != nullptr ? "YES" : "NO") << "\n";
	}

	std::cout << "Total actors in scene: " << actors.size() << "\n";

	camera = std::dynamic_pointer_cast<CameraActor>(assetManager->GetActorCatalog().at("mainCamera"));
	camera->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.0f, -5.0f), Quaternion());

	Ref<SkyBox> skybox = std::dynamic_pointer_cast<SkyBox>(assetManager->GetActorCatalog().at("skybox"));
	camera->SetSkyBox(skybox);

	camera->OnCreate();

	return true;
}

void Scene5::OnDestroy() {
	actors.clear();
}
void Scene5::HandleEvents(const SDL_Event &sdlEvent) {
	switch( sdlEvent.type ) {
    case SDL_EVENT_KEY_DOWN:
		switch (sdlEvent.key.scancode) {
			case SDL_SCANCODE_W:
				drawInWireMode = !drawInWireMode;
				break;
		}
		break;

	case SDL_EVENT_MOUSE_MOTION:
		break;

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		break; 

	case SDL_EVENT_MOUSE_BUTTON_UP:
	break;

	default:
		break;
    }
}
void Scene5::Update(const float deltaTime) {
}

void Scene5::Render() const {

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

	camera->RenderSkyBox();

    for (auto& pair : actors) {
        Ref<Actor> actor = pair.second;

        Ref<ShaderComponent>    shader    = actor->GetComponent<ShaderComponent>();
        Ref<TransformComponent> transform = actor->GetComponent<TransformComponent>();
        Ref<MaterialComponent>  material  = actor->GetComponent<MaterialComponent>();
        Ref<MeshComponent>      mesh      = actor->GetComponent<MeshComponent>();

        if (!shader || !transform || !material || !mesh) continue;

        glUseProgram(shader->GetProgram());
        glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
        glUniformMatrix4fv(shader->GetUniformID("viewMatrix"),       1, GL_FALSE, camera->GetViewMatrix());
        glUniformMatrix4fv(shader->GetUniformID("modelMatrix"),      1, GL_FALSE, transform->GetModelMatrix());
        glUniform3fv(shader->GetUniformID("lightPos1"), 1, lightPos1);
        glUniform3fv(shader->GetUniformID("lightPos2"), 1, lightPos2);

        glBindTexture(GL_TEXTURE_2D, material->getTextureID());
        mesh->Render();
    }
}