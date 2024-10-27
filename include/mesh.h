#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include <shader_s.h>

constexpr int MAX_BONE_INFLUNCE = 4;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;

    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    int m_BoneIDs[MAX_BONE_INFLUNCE];
    float m_Weights[MAX_BONE_INFLUNCE];

    // int useDiffuseTexture;
    // glm::vec4 diffuseColor;
};

struct Texture {
    unsigned id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex>, std::vector<unsigned>, std::vector<Texture>);
    void Draw(Shader&);
private:
    unsigned VAO, VBO, EBO;
    void setupMesh();
};

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned> indices, std::vector<Texture> textures)
    : vertices(vertices), indices(indices), textures(textures) {
    setupMesh();
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_DRAW);

    // vertex positionn
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);

    // vertex normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Vertex::Normal));

    // vertex texture
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Vertex::TexCoord));

    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Vertex::Tangent));

    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Vertex::Bitangent));

    // vertex ids and weights
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*) offsetof(Vertex, Vertex::m_BoneIDs));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Vertex::m_Weights));

    // // vertex diffuse texture
    // glEnableVertexAttribArray(7);
    // glVertexAttribIPointer(7, 1, GL_INT, sizeof(Vertex), (void*) offsetof(Vertex, Vertex::useDiffuseTexture));
    // // vertex diffuse color
    // glEnableVertexAttribArray(8);
    // glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Vertex::diffuseColor));

    glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader) {
    // bind appropriate textures    
    unsigned diffuseNr = 1;
    unsigned specularNr = 1;
    unsigned normalNr = 1;
    unsigned heightNr = 1;
    for (unsigned i{}; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number, name = textures[i].type;
        if (name == "texture_diffuse") {
            number = std::to_string(diffuseNr);
            diffuseNr ++;
        } else if (name == "texture_specular") {
            number = std::to_string(specularNr);
            specularNr ++;
        } else if (name == "texture_normal") {
            number = std::to_string(normalNr);
            normalNr ++;
        } else if (name == "texture_height") {
            number = std::to_string(heightNr);
            heightNr ++;
        }

        shader.setInt1(name + number, i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    // Bind texture
    glActiveTexture(GL_TEXTURE0);

    // Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}
#endif // MESH_H