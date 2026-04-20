#ifndef SCENE2_H
#define SCENE2_H

#include "Scene.h"
#include "Vector.h"
#include <vector>
#include <Matrix.h>
#include "Actor.h"
#include <unordered_map>
#include "CameraActor.h"

/// Forward declarations
union SDL_Event;

class Scene2g : public Scene {
private:

    //std::vector<Ref<Actor>> actors;   //put all future actors in one vec
    std::unordered_map<std::string, Ref<Actor>> actors;
    Ref<CameraActor> camera;

    bool drawInWireMode;
    bool spining = false;

    Vec3 lightPos;

public:
    explicit Scene2g();
    virtual ~Scene2g();

    virtual bool OnCreate() override;
    virtual void OnDestroy() override;
    virtual void Update(const float deltaTime) override;
    virtual void Render() const override;
    virtual void HandleEvents(const SDL_Event& sdlEvent) override;
};

#endif