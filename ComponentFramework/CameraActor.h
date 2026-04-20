#pragma once
#include "Actor.h"
#include <MMath.h>
#include <QMath.h>
using namespace MATH;
class CameraActor : public Actor
{
private:
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	Quaternion orientation;
	Matrix4 translate;

public:
	CameraActor(Actor* parent_, float fovy, float aspectRatio, float near, float far);
	~CameraActor();
	bool OnCreate();

	/// Some getters and setters
	Matrix4 GetViewMatrix() const {
		return viewMatrix;
	}

	Matrix4 GetProjectionMatrix() const {
		return projectionMatrix;
	}



};

