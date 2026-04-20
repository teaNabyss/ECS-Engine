#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Scene2g.h"
#include <MMath.h>
#include "Debug.h"

#include "MaterialComponent.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"

Scene2g::Scene2g() :
	drawInWireMode{ false }, camera{nullptr} {
	Debug::Info("Created Scene2: ", __FILE__, __LINE__);
}

Scene2g::~Scene2g() {
	Debug::Info("Deleted Scene2: ", __FILE__, __LINE__);
}

bool Scene2g::OnCreate() {
	camera = std::make_shared<CameraActor>(nullptr, 45.0f, 16.0f / 9.0f, 0.5f, 100.0f);
	camera->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.0f, -5.0f), Quaternion());
	camera->OnCreate();

	lightPos = Vec3(3.0f, 0.0f, 0.0f);


	Ref<Actor> floor = std::make_shared<Actor>(nullptr);

	//I made types of objects insted of creating inner vector. It's called tags I thiink, but I prefer word "types" here
	floor->SetType(ActorType::PLANE);
	actors["floor"] = floor;

	floor->AddComponent<MeshComponent>(nullptr, "meshes/Plane.obj");
	floor->AddComponent<ShaderComponent>(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	floor->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, -1.5f, -3.5f), QMath::angleAxisRotation(-90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.6f, 0.6f, 0.6f));
	floor->AddComponent<MaterialComponent>(nullptr, "textures/8x8_checkered_board2.png");
	floor->OnCreate();
	floor->ListComponents();

	float spacing = 1.0f;

	for (int i = 0; i < 8; i++) {
		//count pos and spacing 
		float y = (i - 3.5f) * 1.3f;

		//to rows on each side
		for (int side = 0; side < 2; side++) {
			for (int row = 0; row < 2; row++) {

				float xOffset = (row == 0) ? 0.0f : -1.2f;  //spacing between rows
				float x = (side == 0) ? -4.35f - xOffset : 4.35f + xOffset;

				Ref<Actor> piece = std::make_shared<Actor>(nullptr);
				piece->SetType(ActorType::BOX);
				piece->AddComponent<MeshComponent>(nullptr, "meshes/ChessPieces/AllChessPieces.obj");
				piece->AddComponent<ShaderComponent>(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
				piece->AddComponent<TransformComponent>(floor.get(), Vec3(x, y, -0.0f), QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.08f, 0.07f, 0.07f));
				piece->AddComponent<MaterialComponent>(nullptr, "textures/BlackChessBaseColour.png");

				piece->OnCreate();
				actors["piece_" + std::to_string(i) + "_" + std::to_string(side) + "_" + std::to_string(row)] = piece;
			}
		}
	}	return true;
}

void Scene2g::OnDestroy() {
	actors.clear();
}
void Scene2g::HandleEvents(const SDL_Event &sdlEvent) {
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

void Scene2g::Update(const float deltaTime) {
	for (auto& pair : actors) {
		Ref<Actor> actor = pair.second;
		if (actor->GetType() == ActorType::PLANE) {
			if (spining) {
				Ref<TransformComponent> spin = actor->GetComponent<TransformComponent>();
				spin->Rotate(QMath::angleAxisRotation(deltaTime * 7.0f, Vec3(0, 0, 1)));
			}
		}
	}
}

void Scene2g::Render() const {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

	for (auto& pair : actors) {
		Ref<Actor> actor = pair.second;
		Ref<ShaderComponent> shader = actor->GetComponent<ShaderComponent>();

        glUseProgram(shader->GetProgram());
        glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera->GetProjectionMatrix());
        glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera->GetViewMatrix());
        glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE,
            actor->GetComponent<TransformComponent>()->GetModelMatrix());
        glUniform3fv(shader->GetUniformID("lightPos"), 1, lightPos);

        glBindTexture(GL_TEXTURE_2D, actor->GetComponent<MaterialComponent>()->getTextureID());
        actor->GetComponent<MeshComponent>()->Render();
    }
}