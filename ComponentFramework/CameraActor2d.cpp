#include "CameraActor2d.h"
#include "TransformComponent.h"

CameraActor2d::CameraActor2d(Actor* parent_, SDL_Window* window_, float worldWidth_, float worldHeight_)
    : Actor(parent_), sdlWindow(window_) {
    int w, h;
    SDL_GetWindowSize(sdlWindow, &w, &h);
    ndc = MMath::NDCtoViewport(w, h);
    ortho = MMath::orthographic(0.0f, worldWidth_, 0.0f, worldHeight_, -1.0f, 1.0f);
    projectionMatrix = ndc * ortho;
}

CameraActor2d::~CameraActor2d() {}

bool CameraActor2d::OnCreate() {
	Ref <TransformComponent> tc = GetComponent<TransformComponent>();
	if (tc != nullptr) {
		viewMatrix = tc->GetTransformMatrix();
		viewMatrix.print("viewMatrix");
	}
	return true;
}

void CameraActor2d::Follow(const Vec3& targetPos) {
    // Center camera on target
    cameraRect.x = targetPos.x - (cameraRect.w * 0.5f);
    cameraRect.y = targetPos.y - (cameraRect.h * 0.5f);

    // Clamp camera to world bounds
    if (cameraRect.x < 0.0f) cameraRect.x = 0.0f;
    if (cameraRect.y < 0.0f) cameraRect.y = 0.0f;

    // Update pos to match cameraRect (CRITICAL!)
    pos.x = cameraRect.x;
    pos.y = cameraRect.y;
    pos.z = 0.0f;
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
    Vec3 relativePos = worldPos - pos;
    Vec3 screenCoords = projectionMatrix * relativePos;
    return screenCoords;
}