#include "CameraActor2d.h"
#include "TransformComponent.h"

CameraActor2d::CameraActor2d(Actor* parent_, SDL_Window* window_, float worldWidth_, float worldHeight_)
    : Actor(parent_), sdlWindow(window_) {
    projectionMatrix = MMath::orthographic(0.0f, worldWidth_, 0.0f, worldHeight_, -1.0f, 1.0f);
}

CameraActor2d::~CameraActor2d() {}

bool CameraActor2d::OnCreate() {
	return true;
}

Matrix4 CameraActor2d::GetViewMatrix() const {
    Ref<TransformComponent> tc = GetComponent<TransformComponent>();
    if (!tc) return MMath::scale(1.0f, 1.0f, 1.0f);
    Vec3 p = tc->GetPosition();
    return MMath::translate(Vec3(-p.x, -p.y, -p.z));
}

void CameraActor2d::Follow(const Vec3& targetPos) {
    Ref<TransformComponent> tc = GetComponent<TransformComponent>();
    if (!tc) return;

    cameraRect.x = targetPos.x - (cameraRect.w * 0.5f);
    cameraRect.y = targetPos.y - (cameraRect.h * 0.5f);

    if (cameraRect.x < 0.0f) cameraRect.x = 0.0f;
    if (cameraRect.y < 0.0f) cameraRect.y = 0.0f;

    tc->SetPosition(Vec3(cameraRect.x, cameraRect.y, 0.0f));
}

Vec3 CameraActor2d::WorldToScreen(const Vec3& worldPos) const {

    /// Relative position is the position of an object relative to the camera, 
    //  rather than its absolute position in the world.
    // 
    //  World pos is an "absolute" position of an object in the game
    //  Relative position shows the position of an object related to camera
    //  
    //  When we subtract the position of the camera we get exactly the image of where the objects are
    //  then when we multiply this number by projection matrix it just converts stuff to pixels as usual
    //  further in render we draw image with accurate screen coordinates of our objects
    Ref<TransformComponent> tc = GetComponent<TransformComponent>();
    Vec3 camPos = tc ? tc->GetPosition() : Vec3(0, 0, 0);
    Vec3 relativePos = worldPos - camPos;
    Vec3 screenCoords = projectionMatrix * relativePos;
    return screenCoords;
}