#ifndef SCENE5_H
#define SCENE5_H

#include "Scene.h"
#include "Vector.h"
#include <vector>
#include <Matrix.h>
#include "Actor.h"
#include <unordered_map>
#include "CameraActor.h"
#include "AssetManager.h"
/// Forward declarations
union SDL_Event;

class Scene5 : public Scene {
private:

    //std::vector<Ref<Actor>> actors;   //put all future actors in one vec
    std::unordered_map<std::string, Ref<Actor>> actors;
    Ref <CameraActor> camera;
    Ref <AssetManager> assetManager;
    bool drawInWireMode;

    Vec3 lightPos1;
    Vec3 lightPos2;

public:
    explicit Scene5();
    virtual ~Scene5();

    virtual bool OnCreate() override;
    virtual void OnDestroy() override;
    virtual void Update(const float deltaTime) override;
    virtual void Render() const override;
    virtual void HandleEvents(const SDL_Event& sdlEvent) override;
};

#endif