#include "CollisionSystem.h"
#include <iomanip>

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

    float v1p = VMath::dot(pc1->GetVelocity(), n);
    float v2p = VMath::dot(pc2->GetVelocity(), n);

    if (v1p - v2p > 0.0f) return;

    float m1 = pc1->GetMass(), m2 = pc2->GetMass();
    float v1p_new = (((m1 - e * m2) * v1p) + ((1.0f + e) * m2 * v2p)) / (m1 + m2);
    float v2p_new = (((m2 - e * m1) * v2p) + ((1.0f + e) * m1 * v1p)) / (m1 + m2);

    pc1->SetVelocity(pc1->GetVelocity() + (v1p_new - v1p) * n);
    pc2->SetVelocity(pc2->GetVelocity() + (v2p_new - v2p) * n);

    // torque from collision impulse
    Vec3 impulse = (v1p_new - v1p) * n;
    pc1->ApplyTorque(VMath::cross(n, impulse));
    pc2->ApplyTorque(VMath::cross(-n, impulse));
}


void CollisionSystem::Update(float deltaTime) {

// Pass 1: integrate, branching on LAST frame's grounded state
    for (auto& actor : collidingActors) {
        Ref<PhysicsComponent> pc = actor->GetComponent<PhysicsComponent>();

        if (pc->IsGrounded()) {
            pc->Update(deltaTime);                          // translation only
            pc->UpdateAngularVel(pc->GetGroundNormal());     // kinematic overwrite
            pc->UpdateOrientation(deltaTime);
        }
        else {
            pc->UpdateRolling(deltaTime);                    // translation + torque-driven spin
        }
    }

    // Pass 2: detect against ground, refresh isGrounded + normal for NEXT frame
    for (auto& actor : collidingActors) {
        Ref<PhysicsComponent> pc = actor->GetComponent<PhysicsComponent>();
        Ref<CollisionComponent> cc = actor->GetComponent<CollisionComponent>();

        for (auto& groundActor : groundActors) {
            Ref<CollisionComponent> groundCC = groundActor->GetComponent<CollisionComponent>();

            if (SpherePlaneDetection(cc, groundCC)) {
                pc->SetGrounded(true);
                pc->SetGroundNormal(groundCC->plane.n);
                SpherePlaneResponse(cc, pc, groundCC);
            }
            else {
                pc->SetGrounded(false);
            }
        }
    }
    //loop through pairs of actors 
    for (int i = 0; i < collidingActors.size(); i++) {
        for (int j = i + 1; j < collidingActors.size(); j++) {
            //store components in pc and cc accordingly
            Ref<CollisionComponent> cc1 = collidingActors[i]->GetComponent<CollisionComponent>();
            Ref<CollisionComponent> cc2 = collidingActors[j]->GetComponent<CollisionComponent>();

            Vec3 pos1 = cc1->GetPosition();
            Vec3 pos2 = cc2->GetPosition();
            float r1 = cc1->GetRadius();
            float r2 = cc2->GetRadius();

            float dist = VMath::mag(pos1 - pos2);


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


bool CollisionSystem::SpherePlaneDetection(const Ref<CollisionComponent>& a, const Ref<CollisionComponent>& b, Contact& outContact) {
    Ref<CollisionComponent> sphere;
    Ref<CollisionComponent> plane;

    if (a->collidertype == ColliderType::SPHERE && b->collidertype == ColliderType::PLANE) {
        sphere = a;
        plane = b;
    }
    else if (b->collidertype == ColliderType::SPHERE && a->collidertype == ColliderType::PLANE) {
        sphere = b;
        plane = a;
    }
    else {
        return false;
    }
    // actual math
    float distance = VMath::dot(plane->plane.n, sphere->GetPosition()) + plane->plane.d;
    const float epsilon = 0.001f;

    if (fabs(distance) > sphere->GetRadius() + epsilon) {
        return false;
    }

    // Fill in the contact 
    outContact.typeA = ColliderType::SPHERE;
    outContact.typeB = ColliderType::PLANE;
    outContact.ccA = sphere;
    outContact.ccB = plane;

    outContact.pcA = sphere->GetPhysicsComponent();
    outContact.pcB = nullptr; // plane/ground has no PhysicsComponent

    outContact.normal = plane->plane.n;

    return true;
}void CollisionSystem::SpherePlaneResponse(Ref<CollisionComponent> sphereCC, Ref<PhysicsComponent> spherePC, Ref<CollisionComponent> planeCC) {
    float e = 1.0f;
    Vec3 n = planeCC->plane.n;
    Vec3 v = spherePC->GetVelocity();

    float vDotN = VMath::dot(v, n);

    // only respond if the sphere is moving into the plane
    if (vDotN >= VERY_SMALL) return;

    Vec3 v_new = v - (1.0f + e) * vDotN * n;
    spherePC->SetVelocity(v_new);
}

bool CollisionSystem::SphereAABBDetection(const Ref<CollisionComponent>& a, const Ref<CollisionComponent>& b, Contact& outContact) {
    Ref<CollisionComponent> sphere;
    Ref<CollisionComponent> box;

    if (a->collidertype == ColliderType::SPHERE && b->collidertype == ColliderType::AABB) {
        sphere = a;
        box = b;
    }
    else if (b->collidertype == ColliderType::SPHERE && a->collidertype == ColliderType::AABB) {
        sphere = b;
        box = a;
    }
    else {
        return false;
    }

    Vec3 sphereCenter = sphere->GetPosition();
    Vec3 boxCenter = box->aabb.center;
    Vec3 boxHalf = box->aabb.half;

    Vec3 boxMin = boxCenter - boxHalf;
    Vec3 boxMax = boxCenter + boxHalf;

    Vec3 closest;
    closest.x = std::max(boxMin.x, std::min(sphereCenter.x, boxMax.x));
    closest.y = std::max(boxMin.y, std::min(sphereCenter.y, boxMax.y));
    closest.z = std::max(boxMin.z, std::min(sphereCenter.z, boxMax.z));

    Vec3 diff = sphereCenter - closest;
    float distSq = VMath::dot(diff, diff);

    const float epsilon = 0.001f;
    float r = sphere->GetRadius() + epsilon;

    if (distSq > (r * r)) {
        return false;
    }

    float dist = VMath::mag(diff);
    Vec3 n;
    if (dist < VERY_SMALL) {
        n = Vec3(0.0f, 1.0f, 0.0f);
    }
    else {
        n = diff / dist;
    }

    FillContactComponents(outContact, ColliderType::SPHERE, ColliderType::AABB, sphere, box);
    outContact.normal = n;

    return true;
}

bool CollisionSystem::SpherePlaneDetection(const Ref<CollisionComponent>& a, const Ref<CollisionComponent>& b, Contact& outContact) {
    Ref<CollisionComponent> sphere;
    Ref<CollisionComponent> plane;

    if (a->collidertype == ColliderType::SPHERE && b->collidertype == ColliderType::PLANE) {
        sphere = a;
        plane = b;
    }
    else if (b->collidertype == ColliderType::SPHERE && a->collidertype == ColliderType::PLANE) {
        sphere = b;
        plane = a;
    }
    else {
        return false;
    }
    // actual math
    float distance = VMath::dot(plane->plane.n, sphere->GetPosition()) + plane->plane.d;
    const float epsilon = 0.001f;

    if (fabs(distance) > sphere->GetRadius() + epsilon) {
        return false;
    }

    // Fill in the contact 
    FillContactComponents(outContact, ColliderType::SPHERE, ColliderType::PLANE, sphere, plane);
    outContact.normal = plane->plane.n;

    return true;
}

void CollisionSystem::FillContactComponents(Contact& outContact, ColliderType typeA, ColliderType typeB,
    Ref<CollisionComponent> ccA, Ref<CollisionComponent> ccB) {
    outContact.typeA = typeA;
    outContact.typeB = typeB;
    outContact.ccA = ccA;
    outContact.ccB = ccB;
    outContact.pcA = ccA->GetPhysicsComponent();
    outContact.pcB = ccB->GetPhysicsComponent(); // naturally nullptr for ground/box, since they have no PhysicsComponent
}