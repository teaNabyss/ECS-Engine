#include "CameraActor.h"
#include "TransformComponent.h"

CameraActor::CameraActor(Actor* parent_, float fovy, float aspectRatio, float near, float far)
	: Actor(parent_) {
	projectionMatrix = MMath::perspective(fovy, aspectRatio, near, far);
	viewMatrix.loadIdentity();

}
CameraActor::~CameraActor() {}

bool CameraActor::OnCreate() {
	Ref <TransformComponent> tc = GetComponent<TransformComponent>();
	if (tc != nullptr) {
		viewMatrix = tc->GetTransformMatrix();
		viewMatrix.print("viewMatrix");
	}
	return true;
}

void CameraActor::RenderSkyBox() const {
    if (!skybox) return;
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    Ref<ShaderComponent> shader = skybox->GetComponent<ShaderComponent>();
    glUseProgram(shader->GetProgram());
    glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
    glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, MMath::toMatrix4(orientation));

    skybox->Render();  // just binds texture and draws mesh

    glUseProgram(0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}