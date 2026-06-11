#ifndef SKYBOXSCENE_H
#define SKYBOXSCENE_H

#include "Scene.h"
#include "Vector.h"
#include <vector>
#include <Matrix.h>
#include "Actor.h"
#include <unordered_map>
#include "CameraActor3d.h"
#include "AssetManager.h"
#include "Window.h"
union SDL_Event;

class SkyBoxScene : public Scene {
private:

    //std::vector<Ref<Actor>> actors;   //put all future actors in one vec
    std::unordered_map<std::string, Ref<Actor>> actors;
    Ref <CameraActor3d> camera;
    Ref <AssetManager> assetManager;
    Window& window;
    bool drawInWireMode;

    Vec3 lightPos1;
    Vec3 lightPos2;

public:
    explicit SkyBoxScene(Window& window_);
    virtual ~SkyBoxScene();

    virtual bool OnCreate() override;
    virtual void OnDestroy() override;
    virtual void Update(const float deltaTime) override;
    virtual void Render() const override;
    virtual void HandleEvents(const SDL_Event& sdlEvent) override;
};

#endif