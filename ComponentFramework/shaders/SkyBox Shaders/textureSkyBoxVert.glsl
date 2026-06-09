#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vVertex;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout(location = 3) out vec3 textureCoords; 


void main() {
    textureCoords = vec3(vVertex);
    textureCoords.x *= -1;
    gl_Position = projectionMatrix * viewMatrix * vVertex;
    
}
