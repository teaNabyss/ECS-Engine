#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Scene4g.h"
#include <MMath.h>
#include "Debug.h"

#include "MaterialComponent.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"

Scene4g::Scene4g() :
	drawInWireMode{ false }, assetManager{nullptr}, camera{ nullptr } {
	Debug::Info("Created Scene4: ", __FILE__, __LINE__);
}

Scene4g::~Scene4g() {
	Debug::Info("Deleted Scene4: ", __FILE__, __LINE__);
}

bool Scene4g::OnCreate() {
	lightPos1 = Vec3(0.0f, 5.0f, 0.0f);
	lightPos2 = Vec3(0.0f, 5.0f, -15.0f);

	assetManager = std::make_shared<AssetManager>("XML/my xml.xml");
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
	camera->OnCreate();

	return true;
}

void Scene4g::OnDestroy() {
	actors.clear();
}
void Scene4g::HandleEvents(const SDL_Event &sdlEvent) {
	switch( sdlEvent.type ) {
    case SDL_EVENT_KEY_DOWN:
		switch (sdlEvent.key.scancode) {
			case SDL_SCANCODE_W:
				drawInWireMode = !drawInWireMode;
				break;
			case SDL_SCANCODE_SPACE:
				spining = true;
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
void Scene4g::Update(const float deltaTime) {
/*	for (auto& pair : actors) {
		Ref<Actor> actor = pair.second;
		if (actor->GetType() == ActorType::PLANE) {
			if (spining) {
				Ref<TransformComponent> spin = actor->GetComponent<TransformComponent>();
				spin->Rotate(QMath::angleAxisRotation(deltaTime * 7.0f, Vec3(0, 0, 1)));
			}
		}
	}*/
}

void Scene4g::Render() const {
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	for (auto& pair : actors) {
		Ref<Actor> actor = pair.second;
		Ref<ShaderComponent> shader = actor->GetComponent<ShaderComponent>();

		if (!shader) continue;  // skip actors without shader (like camera)

		glUseProgram(shader->GetProgram());
		glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
		glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE,
			actor->GetComponent<TransformComponent>()->GetModelMatrix());
		glUniform3fv(shader->GetUniformID("lightPos1"), 1, lightPos1);
		glUniform3fv(shader->GetUniformID("lightPos2"), 1, lightPos2);

		glBindTexture(GL_TEXTURE_2D, actor->GetComponent<MaterialComponent>()->getTextureID());
		actor->GetComponent<MeshComponent>()->Render();
	}
}