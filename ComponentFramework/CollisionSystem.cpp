#include "CollisionSystem.h"

bool CollisionSystem::TwoSpheresDetection(Vec3 pos1, float r1, Vec3 pos2, float r2) {
    Vec3 L = pos1 - pos2;
    if (VMath::mag(L) < (r1 + r2)) {
        std::cout << "Spheres colided" << std::endl;
        return true;
    }
    return false;
}

void CollisionSystem::TwoSpheresResponse(Vec3 pos1, Ref<PhysicsComponent> pc1, Vec3 pos2, Ref<PhysicsComponent> pc2) {
    float e = 1.0f;
    Vec3 n = VMath::normalize(pos1 - pos2);

    float v1p = VMath::dot(pc1->vel, n);
    float v2p = VMath::dot(pc2->vel, n);

    if (v1p - v2p > 0.0f) return;

    float m1 = pc1->mass, m2 = pc2->mass;
    float v1p_new = (((m1 - e * m2) * v1p) + ((1.0f + e) * m2 * v2p)) / (m1 + m2);
    float v2p_new = (((m2 - e * m1) * v2p) + ((1.0f + e) * m1 * v1p)) / (m1 + m2);

    pc1->vel = pc1->vel + (v1p_new - v1p) * n;
    pc2->vel = pc2->vel + (v2p_new - v2p) * n;

    // torque from collision impulse
    Vec3 impulse = (v1p_new - v1p) * n;
    pc1->ApplyTorque(VMath::cross(n, impulse));
    pc2->ApplyTorque(VMath::cross(-n, impulse));
}


void CollisionSystem::Update(float deltaTime) {
    std::cout << "Update called, actor count: " << collidingActors.size() << std::endl;

    for (auto& actor : collidingActors) {
        Ref<PhysicsComponent> pc = actor->GetComponent<PhysicsComponent>();
        std::cout << "pc: " << (pc ? "found" : "NULL") << std::endl;
        pc->Update(deltaTime);
        pc->UpdateAngularVel(Vec3(0.0f, 1.0f, 0.0f));
    }

    //loop through pairs of actors 
    for (int i = 0; i < collidingActors.size(); i++) {
        for (int j = i + 1; j < collidingActors.size(); j++) {
            //store components in pc and cc accordingly
            std::cout << "checking pair " << i << " vs " << j << std::endl;
            Vec3 pos1 = collidingActors[i]->GetComponent<PhysicsComponent>()->tc->GetPosition();
            Vec3 pos2 = collidingActors[j]->GetComponent<PhysicsComponent>()->tc->GetPosition();
            float r1 = collidingActors[i]->GetComponent<CollisionComponent>()->radius;
            float r2 = collidingActors[j]->GetComponent<CollisionComponent>()->radius;
            float dist = VMath::mag(pos1 - pos2);

            std::cout << "dist: " << dist << " r1+r2: " << r1 + r2 << std::endl;

            Ref<PhysicsComponent> pc1 = collidingActors[i]->GetComponent<PhysicsComponent>();
            Ref<PhysicsComponent> pc2 = collidingActors[j]->GetComponent<PhysicsComponent>();

            //check if they colided
            if (TwoSpheresDetection(pos1, r1, pos2, r2)) {
                //do the responce
                TwoSpheresResponse(pos1, pc1, pos2, pc2);
            }
        }
    }
}
