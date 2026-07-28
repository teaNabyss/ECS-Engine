#pragma once
#include <vector>
#include "CollisionComponent.h"
#include "PhysicsComponent.h"
#include "Actor.h"
#include "Debug.h"
#include <Sphere.h>
using namespace MATH;
using namespace MATHEX;

//The first step of breaking Update() by stages. 
//the idea is to make the function less chunky and annoying
//to be smth easy to read and manage in future.
// this is the moment where I start to make things pretty
//.
// 
//.
// I feel like the detection stage should do the paper work, not the response
// so I put actual Ref<CollisionComponent>/Ref<PhysicsComponent> pairs directly
// making the struct to be more specific
//.
//
//.
// A Contact is the answer to "did these two things touch, and how?"
// this struct stores info about collision event that is currently happening to actors
struct Contact {
	ColliderType typeA;
	ColliderType typeB;
	Ref<CollisionComponent> ccA;
	Ref<CollisionComponent> ccB;
	Ref<PhysicsComponent> pcA;
	Ref<PhysicsComponent> pcB; // may be null, e.g. ground has no PhysicsComponent
	Vec3 normal;               // contact normal, instead of recomputing it again is response
};

class CollisionSystem{


private:
	std::vector<Ref<Actor>> collidingActors;
	std::vector<Ref<Actor>> groundActors;

	void FillContactComponents(Contact& outContact, ColliderType typeA, ColliderType typeB,
		Ref<CollisionComponent> ccA, Ref<CollisionComponent> ccB);
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

	void Update(const float deltaTime);

	bool TwoSpheresDetection(const Ref<CollisionComponent>& a, const Ref<CollisionComponent>& b, Contact& outContact);
	void TwoSpheresResponse(const Contact& contact);
	bool SphereAABBDetection(const Ref<CollisionComponent>& a, const Ref<CollisionComponent>& b, Contact& outContact); // unchanged
	void SphereAABBResponse(const Contact& contact);
	bool SpherePlaneDetection(const Ref<CollisionComponent>& a, const Ref<CollisionComponent>& b, Contact& outContact); // unchanged
	void SpherePlaneResponse(const Contact& contact);

};

