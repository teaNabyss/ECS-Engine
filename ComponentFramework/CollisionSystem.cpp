#include "CollisionSystem.h"
#include <iomanip>

void CollisionSystem::Update(float deltaTime) {

    // Stage 0: branching on LAST frame's grounded state
    for (auto& actor : collidingActors) {
        Ref<PhysicsComponent> pc = actor->GetComponent<PhysicsComponent>();

        if (pc->IsGrounded()) {
            pc->Update(deltaTime);
            pc->UpdateAngularVel(pc->GetGroundNormal());
            pc->UpdateOrientation(deltaTime);
        }
        else {
            pc->UpdateRolling(deltaTime);
        }
    }

    // Stage 1: detection phase — collect all contacts, no state changes yet
    std::vector<Contact> contacts;

    // reset grounded state before re-detecting this frame
    for (auto& actor : collidingActors) {
        actor->GetComponent<PhysicsComponent>()->SetGrounded(false);
    }

    // sphere vs ground
    for (auto& actor : collidingActors) {
        Ref<CollisionComponent> cc = actor->GetComponent<CollisionComponent>();

        for (auto& groundActor : groundActors) {
            Ref<CollisionComponent> groundCC = groundActor->GetComponent<CollisionComponent>();

            Contact contact;
            if (SpherePlaneDetection(cc, groundCC, contact)) {
                contacts.push_back(contact);
            }
        }
    }

    // sphere vs sphere, sphere vs box (pairs within collidingActors)
    for (int i = 0; i < collidingActors.size(); i++) {
        for (int j = i + 1; j < collidingActors.size(); j++) {
            Ref<CollisionComponent> cc1 = collidingActors[i]->GetComponent<CollisionComponent>();
            Ref<CollisionComponent> cc2 = collidingActors[j]->GetComponent<CollisionComponent>();

            Contact contact;
            if (cc1->collidertype == ColliderType::SPHERE && cc2->collidertype == ColliderType::SPHERE) {
                if (TwoSpheresDetection(cc1, cc2, contact)) {
                    contacts.push_back(contact);
                }
            }
            else if (SphereAABBDetection(cc1, cc2, contact)) {
                contacts.push_back(contact);
            }
        }
    }

    // Stage 2: response phase — dispatch each contact, apply state changes
    for (auto& contact : contacts) {
        if (contact.typeA == ColliderType::SPHERE && contact.typeB == ColliderType::PLANE) {
            contact.pcA->SetGrounded(true);
            contact.pcA->SetGroundNormal(contact.normal);
            SpherePlaneResponse(contact);
        }
        else if (contact.typeA == ColliderType::SPHERE && contact.typeB == ColliderType::AABB) {
            SphereAABBResponse(contact);
        }
        else if (contact.typeA == ColliderType::SPHERE && contact.typeB == ColliderType::SPHERE) {
            TwoSpheresResponse(contact);
        }
    }
}

bool CollisionSystem::TwoSpheresDetection(const Ref<CollisionComponent>& a, const Ref<CollisionComponent>& b, Contact& outContact) {
    if (a->collidertype != ColliderType::SPHERE || b->collidertype != ColliderType::SPHERE) {
        return false;
    }

    Vec3 pos1 = a->GetPosition();
    Vec3 pos2 = b->GetPosition();
    float r1 = a->GetRadius();
    float r2 = b->GetRadius();

    Vec3 L = pos1 - pos2;
    float dist = VMath::mag(L);

    if (dist >= (r1 + r2)) {
        return false;
    }

    if (dist < VERY_SMALL) {
        // centers are (almost) exactly overlapping — no clear direction.
        // Same edge case as the original normalize() crash earlier in this project.
        return false;
    }

    Vec3 n = L / dist; // normalized direction from b's center to a's center

    FillContactComponents(outContact, ColliderType::SPHERE, ColliderType::SPHERE, a, b);
    outContact.normal = n;

    return true;
}
void CollisionSystem::TwoSpheresResponse(const Contact& contact) {
    float e = 1.0f;
    Vec3 n = contact.normal;

    Ref<PhysicsComponent> pc1 = contact.pcA;
    Ref<PhysicsComponent> pc2 = contact.pcB;

    float v1p = VMath::dot(pc1->GetVelocity(), n);
    float v2p = VMath::dot(pc2->GetVelocity(), n);

    if (v1p - v2p > 0.0f) return;

    float m1 = pc1->GetMass(), m2 = pc2->GetMass();
    float v1p_new = (((m1 - e * m2) * v1p) + ((1.0f + e) * m2 * v2p)) / (m1 + m2);
    float v2p_new = (((m2 - e * m1) * v2p) + ((1.0f + e) * m1 * v1p)) / (m1 + m2);

    pc1->SetVelocity(pc1->GetVelocity() + (v1p_new - v1p) * n);
    pc2->SetVelocity(pc2->GetVelocity() + (v2p_new - v2p) * n);

    Vec3 impulse = (v1p_new - v1p) * n;
    pc1->ApplyTorque(VMath::cross(n, impulse));
    pc2->ApplyTorque(VMath::cross(-n, impulse));
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
void CollisionSystem::SphereAABBResponse(const Contact& contact) {
    float e = 1.0f;
    Vec3 n = contact.normal;
    Ref<PhysicsComponent> spherePC = contact.pcA;

    Vec3 v = spherePC->GetVelocity();
    float vDotN = VMath::dot(v, n);

    if (vDotN >= 0.0f) return; // already moving away

    Vec3 v_new = v - (1.0f + e) * vDotN * n;
    spherePC->SetVelocity(v_new);
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
void CollisionSystem::SpherePlaneResponse(const Contact& contact) {
    float e = 1.0f;
    Vec3 n = contact.normal;
    Ref<PhysicsComponent> spherePC = contact.pcA;

    Vec3 v = spherePC->GetVelocity();
    float vDotN = VMath::dot(v, n);

    // only respond if the sphere is moving into the plane
    if (vDotN >= VERY_SMALL) return;

    Vec3 v_new = v - (1.0f + e) * vDotN * n;
    spherePC->SetVelocity(v_new);
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