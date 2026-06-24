#include "CameraActor2d.h"
#include "TransformComponent.h"

CameraActor2d::CameraActor2d(Actor* parent_, SDL_Window* window_, float worldWidth_, float worldHeight_)
    : Actor(parent_), sdlWindow(window_), worldWidth(worldWidth_), worldHeight(worldHeight_) {
    projectionMatrix = MMath::orthographic(0.0f, worldWidth_, 0.0f, worldHeight_, -1.0f, 1.0f);
}

CameraActor2d::~CameraActor2d() {}

bool CameraActor2d::OnCreate() {
	return true;
}


//when the camera moves along with player
//the view matrix will translate in opposite direction
//so if the camera moves from (0, 0) to (5, 0)
//the view matrix will change the world to (-5, 0)
//this way we create scrolling effect
Matrix4 CameraActor2d::GetViewMatrix() const {
    Ref<TransformComponent> tc = GetComponent<TransformComponent>();
   // if (!tc) return MMath::scale(1.0f, 1.0f, 1.0f);
    Vec3 p = tc->GetPosition();
    return MMath::translate(Vec3(-p.x, -p.y, -p.z));
}

//sticks to moving object (player), keeping it in center. 
void CameraActor2d::Follow(const Vec3& targetPos) {
    Ref<TransformComponent> tc = GetComponent<TransformComponent>();
    if (!tc) return;


    float halfWidth = worldWidth * 0.5f;
    float halfHeight = worldHeight * 0.5f;


    Vec3 p;
    p.x = targetPos.x - halfWidth;
    p.y = targetPos.y - halfHeight;

    std::cout << "Follow: target=(" << targetPos.x << "," << targetPos.y
        << ") half=(" << halfWidth << "," << halfHeight
        << ") calc p=(" << p.x << "," << p.y << ")\n";

    std::cout << "worldWidth=" << worldWidth << " worldSizeX=" << worldSizeX
        << " p.x before clamp=" << p.x << "\n"; 

    // Clamp to world bounds
    if (p.x < 0.0f) p.x = 0.0f;
    if (p.y < 0.0f) p.y = 0.0f;

    if (p.x + worldWidth > worldSizeX) p.x = worldSizeX - worldWidth;
    if (p.y + worldHeight > worldSizeY) p.y = worldSizeY - worldHeight;


    std::cout << "p.x after clamp=" << p.x << "\n";

    tc->SetPosition(Vec3(p.x, p.y, 0.0f));
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