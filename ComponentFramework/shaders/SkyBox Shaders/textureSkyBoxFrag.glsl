#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 fragColor;

layout(location = 3) in vec3 textureCoords; 

uniform samplerCube myTexture; 

void main() {
	fragColor = texture(myTexture,textureCoords); 
//	fragColor = vec4(0.0, 0.0, 0.0, 0.0);

}