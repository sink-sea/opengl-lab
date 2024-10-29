#version 330 core

out vec4 FragColor;

in vec2 oTexCoords;
in vec3 oNormal;
in vec3 oCamPos;
in vec3 oFragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform int useTexture;

uniform vec3  uDiffuse;
uniform vec3  uAmbient;
uniform vec3  uSpecular;
uniform float uLightStr;
uniform float uN;
uniform vec3  uLightPos;
uniform vec3  uLightColour;

void main(){
    vec3 normal = normalize(oNormal);
    vec3 lightDir = normalize(uLightPos - oFragPos);
    vec3 viewDir = normalize(oCamPos - oFragPos);

    vec3 ambient = uAmbient * uLightColour * uLightStr;

    float diff = max(dot(lightDir, normal), 0.0f); // Lambert diffuse model
    vec3 diffuse = uDiffuse * uLightColour * diff;

    vec3 halfVec = normalize(lightDir + viewDir);
    float spec = max(dot(halfVec, normal), 0.0f);
    vec3 specular = uSpecular * uLightColour * pow(spec, uN); // Phong's model

    if (useTexture == 0) {
        FragColor = vec4(specular + diffuse + ambient, 1.0f);
    } else {
        FragColor = mix(texture(texture_diffuse1, oTexCoords), vec4(specular + diffuse + ambient, 1.0f), 0.2); // mix together
    }
}
