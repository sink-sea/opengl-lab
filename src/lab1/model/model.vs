#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 oTexCoords;
out vec3 oNormal;
out vec3 oCamPos;
out vec3 oFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 NormalMatrix;
uniform vec3 camPos;

void main(){
    oTexCoords = aTexCoords;
    oNormal = aNormal;
    oFragPos = (model * vec4(aPos, 1.0f)).xyz;
    oNormal = (NormalMatrix * vec4(aNormal, 1.0f)).xyz;
    oCamPos = camPos;

    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}