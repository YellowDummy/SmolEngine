#include "stdafx.h"
#include "ModelImporter.h"
#include "Core/SLog.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace SmolEngine
{
    // TODO: Add materials and animations

    bool ModelImporter::Load(const std::string& filePath, ImportedData* out_data)
    {
        Assimp::Importer importer;
        const aiScene* g_scene = importer.ReadFile(filePath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
        auto meshes = g_scene->mMeshes;

        uint32_t childs = 0;
        for (uint32_t i = 0; i < g_scene->mRootNode->mNumChildren; ++i)
        {
            const aiNode* node = g_scene->mRootNode->mChildren[i];
            if (node->mNumMeshes > 0)
                childs++;
        }

        out_data->Components.reserve(childs);
        for (uint32_t i = 0; i < g_scene->mRootNode->mNumChildren; ++i)
        {
            const aiNode* node = g_scene->mRootNode->mChildren[i];

            if (node->mNumMeshes > 0)
            {
                ImportedComponent component;
                uint32_t vertexCount = 0;
                uint32_t indexCount = 0;

                for (uint32_t y = 0; y < node->mNumMeshes; ++y)
                {
                    uint32_t id = node->mMeshes[y];
                    const aiMesh* mesh = meshes[id];

                    vertexCount += mesh->mNumVertices;
                    indexCount += mesh->mNumFaces;
                }

                glm::mat4 Transformation;
                {
                    Transformation[0].r = node->mTransformation.a1;
                    Transformation[0].g = node->mTransformation.a2;
                    Transformation[0].b = node->mTransformation.a3;
                    Transformation[0].a = node->mTransformation.a4;

                    Transformation[1].r = node->mTransformation.b1;
                    Transformation[1].g = node->mTransformation.b2;
                    Transformation[1].b = node->mTransformation.b3;
                    Transformation[1].a = node->mTransformation.b4;

                    Transformation[2].r = node->mTransformation.c1;
                    Transformation[2].g = node->mTransformation.c2;
                    Transformation[2].b = node->mTransformation.c3;
                    Transformation[2].a = node->mTransformation.c4;

                    Transformation[3].r = node->mTransformation.d1;
                    Transformation[3].g = node->mTransformation.d2;
                    Transformation[3].b = node->mTransformation.d3;
                    Transformation[3].a = node->mTransformation.d4;
                }


                component.VertexData.reserve(vertexCount);
                component.Indices.reserve(indexCount * 3);
                for (uint32_t y = 0; y < node->mNumMeshes; ++y)
                {
                    uint32_t id = node->mMeshes[y];
                    const aiMesh* mesh = meshes[id];

                    for (uint32_t x = 0; x < mesh->mNumVertices; ++x)
                    {
                        // Position
                        glm::vec3 pos = { mesh->mVertices[x].x, mesh->mVertices[x].y, mesh->mVertices[x].z };

                        // Normals
                        glm::vec3 normal = glm::vec3(0.0f);
                        if (mesh->HasNormals())
                            normal = { mesh->mNormals[x].x, mesh->mNormals[x].y, mesh->mNormals[x].z };

                        // UVs
                        glm::vec2 uv = glm::vec2(0.0f);
                        if (mesh->HasTextureCoords(0))
                            uv = { mesh->mTextureCoords[0][x].x, mesh->mTextureCoords[0][x].y };

                        //Tangents
                        glm::vec3 tangents = glm::vec3(0.0f);
                        if (mesh->HasTangentsAndBitangents())
                            tangents = { mesh->mTangents[x].x, mesh->mTangents[x].y, mesh->mTangents[x].z };

                        //Colors
                        glm::vec4 color = glm::vec4(0.4f, 0.2f, 0.2, 1.0f);
                        if (mesh->HasVertexColors(0))
                        {
                            color = { mesh->mColors[0][x].r, mesh->mColors[0][x].g,
                                mesh->mColors[0][x].b, mesh->mColors[0][x].a };
                        }

                        PBRVertex data;
                        {
                            data.Pos = glm::vec4(pos, 1) * Transformation;
                            data.Normals = normal;
                            data.Tangent = glm::vec4(tangents, 1.0);
                            data.UVs = uv;
                            data.Color = color;
                        }

                        component.VertexData.emplace_back(data);
                    }

                    for (uint32_t x = 0; x < mesh->mNumFaces; ++x)
                    {
                        component.Indices.emplace_back(mesh->mFaces[x].mIndices[0]);
                        component.Indices.emplace_back(mesh->mFaces[x].mIndices[1]);
                        component.Indices.emplace_back(mesh->mFaces[x].mIndices[2]);
                    }
                }

                component.Name = node->mName.C_Str();
                out_data->Components.emplace_back(component);
            }
        }

        return true;
	}
}