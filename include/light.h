#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_s.h"

class Light {
public:
    glm::vec3 pos;
    glm::vec3 color;
    float strength;
    float N;
    Light(glm::vec3, glm::vec3, float, float);
    void render(Shader& shader);
};

Light::Light(glm::vec3 pos, glm::vec3 color, float strength, float N)
    : pos(pos), color(color), strength(strength), N(N) {}

void Light::render(Shader& shader) {
    shader.setVec3("uLightPos", pos);
    shader.setVec3("uLightColour", color);
    shader.setFloat1("uLightStr", strength);
    shader.setFloat1("uN", N);
}

#endif // LIGHT_H
