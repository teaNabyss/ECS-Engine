#pragma once
#include "Actor.h"
#include "ShaderComponent.h"
#include "MeshComponent.h"
#include <SDL_image.h>
#include <MMath.h>
#include <QMath.h>
class SkyBox :  public Actor
{
private:
    GLuint textureID;

    const char* posX; const char* negX;
    const char* posY; const char* negY;
    const char* posZ; const char* negZ;

    Matrix4 projectionMatrix;
    Quaternion orientation;
public:
    SkyBox(Actor* parent_,
        const char* posX_, const char* negX_,
        const char* posY_, const char* negY_,
        const char* posZ_, const char* negZ_,
        const char* vert_, const char* frag_);
    ~SkyBox();

    bool OnCreate() override;
    void OnDestroy() override;
    void Render() const override;

    void SetOrientation(const Quaternion& q) { orientation = q; }
    void SetProjectionMatrix(const Matrix4& pm) { projectionMatrix = pm; }
};

