#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Shader.h"
#include <vector>
#include <iostream>

class Model {
public:
    std::vector<Mesh> meshes;

    Model(std::string path) {
        // std::cout << "start loading" << std::endl;
        load(path);
    }

    void Draw(Shader& s) {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(s);
    }

private:
    void load(std::string path) {
        Assimp::Importer import;
        const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ASSIMP ERROR " << import.GetErrorString() << std::endl;
            return;
        }
        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode* node, const aiScene* scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vs;
        std::vector<unsigned int> is;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex v;
            glm::vec3 tmp;

            tmp.x = mesh->mVertices[i].x;
            tmp.y = mesh->mVertices[i].y;
            tmp.z = mesh->mVertices[i].z;
            v.Position = tmp;

            if (mesh->HasNormals()) {
                tmp.x = mesh->mNormals[i].x;
                tmp.y = mesh->mNormals[i].y;
                tmp.z = mesh->mNormals[i].z;
                v.Normal = tmp;
            }
            else {
                v.Normal = glm::vec3(0.0f);
            }
            // текстуры не использую пока
            if (mesh->mTextureCoords[0]) {
                v.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }
            else {
                v.TexCoords = glm::vec2(0.0f, 0.0f);
            }
            vs.push_back(v);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                is.push_back(face.mIndices[j]);
        }

        return Mesh(vs, is);
    }
};