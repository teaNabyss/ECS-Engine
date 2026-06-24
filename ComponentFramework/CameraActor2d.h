#pragma once
#include "Actor.h"
#include <MMath.h>
#include <QMath.h>
#include <SDL.h>
using namespace MATH;
class CameraActor2d : public Actor
{
private:

	Matrix4 ortho;
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	Vec3 pos;

	float worldSizeX = 120.0f;
	float worldSizeY = 60.0f;

	float worldWidth;    
	float worldHeight;

	SDL_Window* sdlWindow;

public:
	CameraActor2d(Actor* parent_, SDL_Window* window_, 
					float worldWidth_, float worldHeight_);	
	~CameraActor2d();
	bool OnCreate();

	/// Some getters and setters
	void Follow(const Vec3& targetPos);
	Vec3 WorldToScreen(const Vec3& worldPos) const;

	Matrix4 GetProjectionMatrix() const { return projectionMatrix; }

	Matrix4 GetViewMatrix() const;
};

