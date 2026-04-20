#version 450
#extension GL_ARB_separate_shader_objects : enable
in vec2 uvCoordFromEval;
in vec2 textureCoords;
in vec3 normalFromEval;
in vec3 lightDirFromEval;
in vec3 eyeDirFromEval;
in float fogValFromEval;
out vec4 fragColor;
uniform sampler2D textureData;

layout (binding = 0) uniform sampler2D heightMap;
layout (binding = 1) uniform sampler2D normalMap;
layout (binding = 2) uniform sampler2D diffuseMap;

void main() {
       vec4 deepColour = vec4(0.01, 0.2, 0.4, 0.0);
       vec4 kd = vec4(0.5, 0.45, 0.5, 0.0);
       vec4 ks = 0.7 * kd;
       vec4 ka = 0.01 * kd;
       vec4 kt = texture(diffuseMap, uvCoordFromEval);

       vec3 normalFromMap = texture(normalMap, uvCoordFromEval).xzy * 2.0 - 1.0;

       float diff  = max(dot (lightDirFromEval, normalFromMap), 0.0);
       vec3 reflection = normalize(reflect(-lightDirFromEval, normalFromMap));
       float spec = max(dot(lightDirFromEval, reflection), 0.0);
       spec = pow(spec,14.0);
       vec4 outputColour = ka + (kd * diff) + (ks * spec) ; 

    fragColor = mix(deepColour, outputColour, fogValFromEval) * kt;
}
