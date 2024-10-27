#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
// uniform sampler2D texture_diffuse3;
// uniform sampler2D texture_specular1;
// uniform sampler2D texture_specular2;
// uniform sampler2D texture_normal1;
// uniform sampler2D texture_normal2;
// uniform sampler2D texture_normal3;
// uniform sampler2D texture_height1;
// uniform sampler2D texture_height2;
// uniform sampler2D texture_height3;

void main(){
    vec4 diffuse1 = texture(texture_diffuse1, TexCoords);
    vec4 diffuse2 = texture(texture_diffuse2, TexCoords);
    // vec4 diffuse3 = texture(texture_diffuse3, TexCoords);
    // vec4 specular1 = texture(texture_specular1, TexCoords);
    // vec4 specular2 = texture(texture_specular2, TexCoords);
    // vec4 normal1 = texture(texture_normal1, TexCoords);
    // vec4 normal2 = texture(texture_normal2, TexCoords);
    // vec4 normal3 = texture(texture_normal3, TexCoords);
    // vec4 height1 = texture(texture_height1, TexCoords);
    // vec4 height2 = texture(texture_height2, TexCoords);
    // vec4 height3 = texture(texture_height3, TexCoords);
    FragColor = diffuse1 * 0.5 + diffuse2 * 0.5;
}
