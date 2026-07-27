#pragma once
#include <vector>
#include "CollisionComponent.h"
#include "PhysicsComponent.h"
#include "Actor.h"
#include "Debug.h"
#include <Sphere.h>
using namespace MATH;
using namespace MATHEX;

class CollisionSystem{


private:
	std::vector<Ref<Actor>> collidingActors;
	std::vector<Ref<Actor>> groundActors;
public:
	/// This function will check the the actor being added is new and has the all proper components 
	void AddActor(Ref<Actor> actor_) {
		if (actor_->GetComponent<CollisionComponent>().get() == nullptr) {
			Debug::Error("The Actor must have a CollisionComponent - ignored ", __FILE__, __LINE__);
			return;
		}

		if (actor_->GetComponent<PhysicsComponent>().get() == nullptr) {
			Debug::Error("The Actor must have a PhysicsComponent - ignored ", __FILE__, __LINE__);
			return;
		}
		collidingActors.push_back(actor_);
	}

	// this is a temorary function to add plane without physics component

	void AddGround(Ref<Actor> actor_) {
		if (actor_->GetComponent<CollisionComponent>().get() == nullptr) {
			Debug::Error("The Actor must have a CollisionComponent - ignored ", __FILE__, __LINE__);
			return;
		}
		groundActors.push_back(actor_);
	}


	bool TwoSpheresDetection(Vec3 pos1, float r1, Vec3 pos2, float r2);
	void TwoSpheresResponse(Vec3 pos1, Ref<PhysicsComponent> pc1, Vec3 pos2, Ref<PhysicsComponent> pc2);
	bool SphereAABBDetection(const Ref<CollisionComponent>& a, const Ref<CollisionComponent>& b);
	void SphereAABBResponse(Ref<CollisionComponent> sphereCC, Ref<PhysicsComponent> spherePC, Ref<CollisionComponent> boxCC);
	bool SpherePlaneDetection(const Ref<CollisionComponent>& a, const Ref<CollisionComponent>& b);
	void SpherePlaneResponse(Ref<CollisionComponent> sphereCC, Ref<PhysicsComponent> spherePC, Ref<CollisionComponent> planeCC);
	void Update(const float deltaTime);
};

