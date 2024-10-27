#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

void main(){
    vec4 diffuse1 = texture(texture_diffuse1, TexCoords);
    // vec4 diffuse2 = texture(texture_diffuse2, TexCoords);
    // vec4 diffuse3 = texture(texture_diffuse3, TexCoords);
    // vec4 specular1 = texture(texture_specular1, TexCoords);
    // vec4 specular2 = texture(texture_specular2, TexCoords);
    // FragColor = diffuse1 + diffuse2 + diffuse3 + specular1 + specular2;
    FragColor = diffuse1;
}
