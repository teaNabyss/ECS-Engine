#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Camera2dScene.h"
#include <MMath.h>
#include "Debug.h"

#include "MaterialComponent.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "PhysicsComponent.h"

Camera2dScene::Camera2dScene(Window& window_) :
	window{ window_ }, drawInWireMode{ false }, assetManager{ nullptr }, camera{ nullptr } {
	Debug::Info("Created Camera2dScene: ", __FILE__, __LINE__);
}

Camera2dScene::~Camera2dScene() {
	Debug::Info("Deleted Camera2dScene: ", __FILE__, __LINE__);
}

bool Camera2dScene::OnCreate() {
	assetManager = std::make_shared<AssetManager>("XML/2dCameraDEBUG.xml", window.getWindow());
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

	camera = std::dynamic_pointer_cast<CameraActor2d>(assetManager->GetActorCatalog().at("mainCamera"));
	camera->GetProjectionMatrix().print("projectionMatrix");

	Ref<TransformComponent> tc = actors["Miku"]->GetComponent<TransformComponent>();
	if (tc) tc->GetModelMatrix().print("Miku modelMatrix");
	camera->GetViewMatrix().print("viewMatrix");

	return true;



}

void Camera2dScene::OnDestroy() {
	actors.clear();
}
void Camera2dScene::HandleEvents(const SDL_Event &sdlEvent) {
	switch( sdlEvent.type ) {
	case SDL_EVENT_KEY_DOWN:
		switch (sdlEvent.key.scancode) {
		case SDL_SCANCODE_W:
			drawInWireMode = !drawInWireMode;
			break;
		case SDL_SCANCODE_A:
			movingLeft = true;
			break;
		case SDL_SCANCODE_D:
			movingRight = true;
			break;
		}
		break;

	case SDL_EVENT_KEY_UP:
		switch (sdlEvent.key.scancode) {
		case SDL_SCANCODE_A:
			movingLeft = false;
			break;
		case SDL_SCANCODE_D:
			movingRight = false;
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
void Camera2dScene::Update(const float deltaTime) {
	for (auto& pair : actors) {
		Ref<Actor> actor = pair.second;
		if (actor->GetType() == ActorType::PLANE) {
			Ref<PhysicsComponent> physics = actor->GetComponent<PhysicsComponent>();
			if (!physics) continue;

			if (movingLeft && !movingRight) {
				physics->SetVelocity(Vec3(-10.0f, 0.0f, 0.0f));
			}
			else if (movingRight && !movingLeft) {
				physics->SetVelocity(Vec3(10.0f, 0.0f, 0.0f));
			}
			else {
				physics->SetVelocity(Vec3(0.0f, 0.0f, 0.0f));
			}

			physics->Update(deltaTime);

			Ref<TransformComponent> tc = actor->GetComponent<TransformComponent>();
			camera->Follow(tc->GetPosition());
		}
	}
}void Camera2dScene::Render() const {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

    for (auto& pair : actors) {
        Ref<Actor> actor = pair.second;

        Ref<ShaderComponent>    shader    = actor->GetComponent<ShaderComponent>();
        Ref<TransformComponent> transform = actor->GetComponent<TransformComponent>();
        Ref<MaterialComponent>  material  = actor->GetComponent<MaterialComponent>();
		Ref<MeshComponent>		mesh = actor->GetComponent<MeshComponent>();

		if (!shader || !transform || !material || !mesh) {
			continue;
		}

		glUseProgram(shader->GetProgram());
        glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
        glUniformMatrix4fv(shader->GetUniformID("viewMatrix"),       1, GL_FALSE, camera->GetViewMatrix());
        glUniformMatrix4fv(shader->GetUniformID("modelMatrix"),      1, GL_FALSE, transform->GetModelMatrix());


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material->getTextureID());
		glUniform1i(shader->GetUniformID("myTexture"), 0);

		mesh->Render();
    }
}