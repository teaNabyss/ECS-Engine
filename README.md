## 🔍 About
A personal game engine project developed as part of [course/self-study — fill in].
Built without a third-party engine, focusing on understanding core rendering and architecture from the ground up.

## Features
ECS-based architecture — actors composed of mesh, shader, material, transform, physics, and collision components
XML-driven asset loading — scenes and assets declared in XML, parsed at runtime via AssetManager
Parent-child transform hierarchy — children inherit parent world transforms via matrix multiplication
Skybox rendering — cubemap skybox with dedicated SkyBox actor class
Custom math library — vectors, matrices, quaternions
OpenGL rendering — phong shading, texture mapping, depth testing
SDL3 — window management and event handling

## Architecture
Engine
├── AssetManager      — XML manifest reader, builds component & actor catalogs
├── Actor             — base game object, holds components
│   ├── CameraActor   — projection/view matrices, skybox rendering
│   └── SkyBox        — cubemap texture, skybox draw
├── Components
│   ├── TransformComponent   — position, rotation, scale + parent inheritance
│   ├── MeshComponent        — geometry loading and rendering
│   ├── ShaderComponent      — GLSL shader program
│   ├── MaterialComponent    — texture binding
│   ├── PhysicsComponent     — mass and physics data
│   └── CollisionComponent   — sphere / AABB / plane collision shapes
└── Scene4g           — currently active scene, update and render loop
