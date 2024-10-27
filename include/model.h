#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <cstring>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "camera.h"
#include "shader_s.h"

unsigned TextureFromFile(const char* path, const std::string& directory, GLint wrapMode, GLint MagFilterMode, GLint MinFilterMode);
unsigned TextureFromAssimp(const aiTexture* aiTex, GLint wrapMode, GLint MagFilterMode, GLint MinFilterMode);

class Model {
public:
    // model data
    // std::vector<Texture> texture_loader;
    std::map<std::string, Texture> texture_loader;
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;

    Model(std::string const& path, bool gamma = false): gammaCorrection(gamma) {
        loadModel(path);
    }
    
    void Draw(Shader& shader) {
        for (unsigned i{}; i < meshes.size(); i ++) {
            meshes[i].Draw(shader);
        }
    }
private:
    void loadModel(std::string const& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene);
};

// load a model with Assimp supported extension from file and save to mesh vector
void Model::loadModel(std::string const& path) {
    // read model with assimp extentions
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) { // null
        std::cout << "ERROR::ASSIMP: " << importer.GetErrorString() << '\n';
        return;
    }

    // retrieve the diretory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));
    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene); 
}

// process a node recursively
void Model::processNode(aiNode* node, const aiScene* scene) {
    // process each mesh located at the current node
    for (unsigned i{}; i < node->mNumMeshes; i ++) {
        // scene contains all the data; node only contains indices(index)
        auto mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // after that, process each of the children nodes
    for (unsigned i{}; i < node->mNumChildren; i ++) {
        processNode(node->mChildren[i], scene);
    }
}

// return a Mesh object created from extracted mesh data
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
    std::vector<Texture> textures;

    // walk through each of the mesh vertices
    for (unsigned i{}; i < mesh->mNumVertices; i ++) {
        Vertex vertex;
        glm::vec3 vector; // placeholder, temporary data container

        // position
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        // normals
        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }

        // texture coordinate
        if (mesh->mTextureCoords[0]) { // does mesh contain texture coordinate
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoord = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        } else {
            vertex.TexCoord = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // walk through each of the mesh faces(aka, triangle), and retrieve the indices
    for (unsigned i{}; i < mesh->mNumFaces; i ++) {
        auto face = mesh->mFaces[i];
        for (unsigned j{}; j < face.mNumIndices; j ++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // process material
    if (mesh->mMaterialIndex >= 0) {
        auto material = scene->mMaterials[mesh->mMaterialIndex];
        // Types with number N defined in mesh.h
        // 1. diffuse maps
        auto diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        auto specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        auto normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", scene);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        auto heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", scene);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }
    return Mesh(vertices, indices, textures);
}

// check all material textures and load if not
std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene) {
    std::vector<Texture> textures;
    // std::cout << mat->GetTextureCount(type) << '\n';
    if (mat->GetTextureCount(type) != 0)
        std::cout << typeName << '\n';
    
    if (mat->GetTextureCount(type) == 0 && type == aiTextureType_DIFFUSE) {
        Texture texture;
        texture.id = TextureFromFile("default.png", directory, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
        texture.type = typeName;
        texture.path = "default.png";
        textures.push_back(texture);
        return textures;
    }
    for (unsigned i{}; i < mat->GetTextureCount(type); i ++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        // // check if texture is loaded before
        // bool skip = false;
        // for (unsigned j{}; j < texture_loader.size(); j ++) {
        //     if (std::strcmp(texture_loader[j].path.data(), str.C_Str()) == 0) {
        //         textures.push_back(texture_loader[j]);
        //         skip = true; // already loaded, pass it
        //         break;
        //     }
        // }
        auto to_find = texture_loader.find(str.C_Str());
        if (to_find != texture_loader.end()) {
            textures.push_back(to_find->second);
        } else { // not loaded, load it
            Texture texture;
            auto path = str.C_Str();
            auto aitexture = scene->GetEmbeddedTexture(path);
            if (aitexture) {
                texture.id = TextureFromAssimp(aitexture, GL_CLAMP, GL_LINEAR, GL_LINEAR);
            } else {    
                texture.id = TextureFromFile(str.C_Str(), directory, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
            }
            // texture.id = TextureFromFile(str.C_Str(), directory, GL_REPEAT, GL_LINEAR, GL_LINEAR);
            texture.path = str.C_Str();
            texture.type = typeName;
            textures.push_back(texture);
            // texture_loader.push_back(texture);
            texture_loader[str.C_Str()] = texture;
        }
    }
    return textures;
}

unsigned TextureFromFile(const char* path, const std::string& directory, GLint wrapMode, GLint MagFilterMode, GLint MinFilterMode) {
    std::string fileName(path);
    fileName = directory + "/" + fileName;
    std::cout << fileName << '\n';

    // load texture
    unsigned textureID{};
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0);

    if (data) {
        GLenum format = GL_RGB;
        if (nrComponents == 1) {
            format = GL_RED;    
        } else if (nrComponents == 3) {
            format = GL_RGB;
        } else if (nrComponents == 4) {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilterMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilterMode);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Texture failed to load at path: " << path << '\n';
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}

unsigned TextureFromAssimp(const aiTexture* aiTex, GLint wrapMode, GLint MagFilterMode, GLint MinFilterMode) {
    if (!aiTex) {
        return 0;
    }
    GLuint textureID = 0;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilterMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilterMode);


	int width, height, nrChannels;
	unsigned char* image_data = nullptr;
	if (aiTex->mHeight == 0) {
		image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(aiTex->pcData), aiTex->mWidth, &width, &height, &nrChannels, 0);
	}
	else {
		image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(aiTex->pcData), aiTex->mWidth * aiTex->mHeight, &width, &height, &nrChannels, 0);
	}

	if (image_data != nullptr) {
		GLenum format = GL_RGB;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
	}

	glGenerateMipmap(GL_TEXTURE_2D);
	return textureID;
}

#endif // MODEL_H