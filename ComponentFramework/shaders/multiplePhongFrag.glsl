#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec3 vertNormal;
layout(location = 1) in vec3 lightDir1;
layout(location = 2) in vec3 lightDir2;
layout(location = 3) in vec3 eyeDir; 
layout(location = 4) in vec2 textureCoords; 

uniform sampler2D myTexture; 

void main() {
    vec4 ks = vec4(0.5, 0.5, 0.5, 0.0);
	vec4 kd = vec4(0.4, 0.4, 0.4, 0.0);
	vec4 ka = 0.1 * kd;
	vec4 kt = texture(myTexture,textureCoords); 

	float diff1 = max(dot(vertNormal, lightDir1), 0.0);

	/// Reflection is based incedent which means a vector from the light source
	/// not the direction to the light source so flip the sign
	vec3 reflection1 = normalize(reflect(-lightDir1, vertNormal));

	float spec1 = max(dot(eyeDir, reflection1), 0.0);
	spec1 = pow(spec1,14.0);

		float diff2 = max(dot(vertNormal, lightDir2), 0.0);

	/// Reflection is based incedent which means a vector from the light source
	/// not the direction to the light source so flip the sign
	vec3 reflection2 = normalize(reflect(-lightDir2, vertNormal));

	float spec2 = max(dot(eyeDir, reflection2), 0.0);
	spec2 = pow(spec2,14.0);

	//fragColor =  (ka + ((diff2 * kd)*(diff1 * kd)) + ((spec2 * ks)*(spec1 * ks)) * kt;	

	fragColor =  ((ka + (diff1 * kd) + (spec1 * ks)) * kt) + (ka + (diff2 * kd) + (spec2 * ks)) * kt;

	//fragColor =  (ka + (diff1 * kd) + (spec1 * ks)) * kt;	


}