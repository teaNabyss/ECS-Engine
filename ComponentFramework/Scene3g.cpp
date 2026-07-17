#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Scene3g.h"
#include <MMath.h>
#include "Debug.h"

#include "MaterialComponent.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "CollisionComponent.h"
#include "PhysicsComponent.h"
#include "CollisionSystem.h"

Scene3g::Scene3g() :
	drawInWireMode{ false }, camera{nullptr} {
	Debug::Info("Created Scene3: ", __FILE__, __LINE__);
}

Scene3g::~Scene3g() {
	Debug::Info("Deleted Scene3: ", __FILE__, __LINE__);
}

bool Scene3g::OnCreate() {
	camera = std::make_shared<CameraActor3d>(nullptr, 45.0f, 16.0f / 9.0f, 0.5f, 100.0f);
	camera->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.0f, -5.0f), Quaternion());
	camera->OnCreate();

	lightPos = Vec3(3.0f, 0.0f, 0.0f);


	Ref<Actor> ground = std::make_shared<Actor>(nullptr);

	//I made types of objects instead of creating inner vector. It's called tags I thiink, but I prefer word "types" here
	ground->SetType(ActorType::PLANE);
	actors["ground"] = ground;

	ground->AddComponent<MeshComponent>(nullptr, "meshes/Plane.obj");
	ground->AddComponent<ShaderComponent>(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	ground->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, -1.0f, -0.5f), QMath::angleAxisRotation(-90.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.4f, 0.4f, 0.4f));
	ground->AddComponent<MaterialComponent>(nullptr, "textures/background.jpg");

	Ref<TransformComponent> groundTC = ground->GetComponent<TransformComponent>();
	Vec3 groundNormal = QMath::rotate(Vec3(0.0f, 0.0f, 1.0f), groundTC->GetQuaternion());
	float d = -VMath::dot(groundNormal, groundTC->GetPosition());
	ground->AddComponent<CollisionComponent>(ground.get(), MATHEX::Plane(groundNormal, d));	ground->OnCreate();

	ground->ListComponents();
	collisionSystem.AddGround(actors["ground"]);

	Ref<Actor> sphere = std::make_shared<Actor>(nullptr);
	sphere->SetType(ActorType::SPHERE);
	actors["sphere"] = sphere;
	sphere->AddComponent<MeshComponent>(nullptr, "meshes/Sphere.obj");
	sphere->AddComponent<ShaderComponent>(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	sphere->AddComponent<TransformComponent>(nullptr, Vec3( -1.0f, -0.7f, -0.2f), QMath::angleAxisRotation(0.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.3f, 0.3f, 0.3f));
	sphere->AddComponent<MaterialComponent>(nullptr, "textures/carpet.jpg");
	sphere->AddComponent<CollisionComponent>(sphere.get(), 0.3f);
	sphere->AddComponent<PhysicsComponent>(sphere.get(), 1.0f);
	sphere->OnCreate();
	sphere->ListComponents();
	collisionSystem.AddActor(actors["sphere"]);


	Ref<Actor> sphere2 = std::make_shared<Actor>(nullptr);
	sphere2->SetType(ActorType::SPHERE);
	actors["sphere2"] = sphere2;
	sphere2->AddComponent<MeshComponent>(nullptr, "meshes/Sphere.obj");
	sphere2->AddComponent<ShaderComponent>(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	sphere2->AddComponent<TransformComponent>(nullptr, Vec3(1.0f, -0.7f, -0.2f), QMath::angleAxisRotation(0.0f, Vec3(1.0f, 0.0f, 0.0f)), Vec3(0.3f, 0.3f, 0.3f));
	sphere2->AddComponent<MaterialComponent>(nullptr, "textures/evilEye.jpg");
	sphere2->AddComponent<CollisionComponent>(sphere2.get(), 0.3f);
	sphere2->AddComponent<PhysicsComponent>(sphere2.get(), 1.0f);
	sphere2->OnCreate();
	sphere2->ListComponents();
	collisionSystem.AddActor(actors["sphere2"]);

	return true;
}

void Scene3g::OnDestroy() {
	actors.clear();
}
void Scene3g::HandleEvents(const SDL_Event &sdlEvent) {
	switch( sdlEvent.type ) {
    case SDL_EVENT_KEY_DOWN:
		switch (sdlEvent.key.scancode) {
			case SDL_SCANCODE_W:
				drawInWireMode = !drawInWireMode;
				break;
			case SDL_SCANCODE_SPACE:
				spining = true;
				actors["sphere"]->GetComponent<PhysicsComponent>()->SetVelocity(Vec3(1.0f, 0.0f, 0.0f));
				actors["sphere2"]->GetComponent<PhysicsComponent>()->SetVelocity(Vec3(-1.0f, 0.0f, 0.0f));

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

void Scene3g::Update(const float deltaTime) {
	/*for (auto& pair : actors) {
		Ref<Actor> actor = pair.second;
		if (actor->GetType() == ActorType::BOX) {
			if (spining) {
				Ref<TransformComponent> spin = actor->GetComponent<TransformComponent>();
				spin->Rotate(QMath::angleAxisRotation(deltaTime * 7.0f, Vec3(0, 0, 1)));
			}
		}
	}*/

	collisionSystem.Update(deltaTime);


}

void Scene3g::Render() const {
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



