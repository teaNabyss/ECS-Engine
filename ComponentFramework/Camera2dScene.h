#ifndef CAMERA2DSCENE_H
#define CAMERA2DSCENE_H

#include "Scene.h"
#include "Vector.h"
#include <vector>
#include <Matrix.h>
#include "Actor.h"
#include <unordered_map>
#include "CameraActor2d.h"
#include "AssetManager.h"
#include "Window.h"

union SDL_Event;

class Camera2dScene : public Scene {
private:

    //std::vector<Ref<Actor>> actors;   //put all future actors in one vec
    std::unordered_map<std::string, Ref<Actor>> actors;
    Ref <CameraActor2d> camera;
    Ref <AssetManager> assetManager;
    bool drawInWireMode;
    bool movingLeft;
    bool movingRight;
    Window& window;

public:
    explicit Camera2dScene(Window& window_);
    virtual ~Camera2dScene();

    virtual bool OnCreate() override;
    virtual void OnDestroy() override;
    virtual void Update(const float deltaTime) override;
    virtual void Render() const override;
    virtual void HandleEvents(const SDL_Event& sdlEvent) override;
};

#endif