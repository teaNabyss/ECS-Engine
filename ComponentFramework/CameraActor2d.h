#pragma once
#include "Actor.h"
#include "SkyBox.h"
#include <MMath.h>
#include <QMath.h>
using namespace MATH;
class CameraActor2d : public Actor
{
private:
	int width = 1280;
	int height = 720;
	Matrix4 ndc;
	Matrix4 ortho;
	Matrix4 projectionMatrix;
	SDL_FRect cameraRect;
	Vec3 pos;

public:
	CameraActor2d(Actor* parent_, float fovy, float aspectRatio, float near, float far);
	~CameraActor2d();
	bool OnCreate();

	/// Some getters and setters
	void Follow(const Vec3& targetPos);
	Vec3 WorldToScreen(const Vec3& worldPos) const;

	Matrix4 GetProjectionMatrix() const {
		return projectionMatrix;
	}

};

