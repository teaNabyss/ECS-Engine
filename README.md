## 🔍 About
A personal game engine project developed as part of [course/self-study].
Built without a third-party engine, focusing on understanding core rendering and architecture from the ground up.

## Features
ECS-based architecture — actors composed of mesh, shader, material, transform, physics, and collision components
XML-driven asset loading — scenes and assets declared in XML, parsed at runtime via AssetManager
Parent-child transform hierarchy — children inherit parent world transforms via matrix multiplication
2 camera types - 2D and 3D cameras for different types of games and simulations
Skybox rendering — cubemap skybox with dedicated SkyBox actor class
Custom math library — vectors, matrices, quaternions. (Provided by college proffesors)
OpenGL rendering — phong shading, texture mapping, depth testing
SDL3 — window management and event handling

## Architecture
Engine
- AssetManager — XML manifest reader, builds component & actor catalogs
- Actor — base game object, holds components
- CameraActor3d — projection/view matrices, skybox rendering
--SkyBox — cubemap texture, skybox draw
- CameraActor2d - position-locking on desired character
- Components
-- TransformComponent   — position, rotation, scale + parent inheritance
-- MeshComponent        — geometry loading and rendering
-- ShaderComponent      — GLSL shader program
-- MaterialComponent    — texture binding
-- PhysicsComponent     — mass and physics data
--- CollisionComponent   — sphere / AABB / plane collision shapes
- Scene3g — currently active scene, update and render 

What is being worked on at the moment:
Collision and Physics are separated classes. 
Right now I work on expansion of collision types and aspects of physics
