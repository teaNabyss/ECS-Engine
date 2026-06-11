#pragma once
#include "Actor.h"
#include "SkyBox.h"
#include <MMath.h>
#include <QMath.h>
using namespace MATH;
class CameraActor3d : public Actor
{
private:
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	Quaternion orientation;
	Matrix4 translate;

	Ref<SkyBox> skybox;

public:
	CameraActor3d(Actor* parent_, float fovy, float aspectRatio, float near, float far);
	~CameraActor3d();
	bool OnCreate();

	/// Some getters and setters
	Matrix4 GetViewMatrix() const {
		return viewMatrix;
	}

	Matrix4 GetProjectionMatrix() const {
		return projectionMatrix;
	}

	void SetSkyBox(Ref<SkyBox> skybox_) { skybox = skybox_; }
	void RenderSkyBox() const;

};

