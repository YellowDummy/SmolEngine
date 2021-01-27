#include "stdafx.h"
#include "ModelImporter.h"
#include "Core/SLog.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace SmolEngine
{
    bool ModelImporter::Load(const std::string& filePath, ImportedData* out_data)
    {
        Assimp::Importer importer;
        const aiScene* g_scene = importer.ReadFile(filePath.c_str(), aiProcessPreset_TargetRealtime_Quality);

        uint32_t vertextCount = 0;
        uint32_t indexCount = 0;

        auto meshes = g_scene->mMeshes;

        for (uint32_t i = 0; i < g_scene->mNumMeshes; ++i)
        {
            vertextCount += meshes[i]->mNumVertices;
            indexCount += meshes[i]->mNumFaces;
        }

        out_data->vertices.reserve(vertextCount);
        out_data->normals.reserve(vertextCount);
        out_data->uvs.reserve(vertextCount);
        out_data->colors.reserve(vertextCount);
        out_data->tangents.reserve(vertextCount);
        out_data->indices.reserve(indexCount * 3);

        for (uint32_t i = 0; i < g_scene->mNumMeshes; ++i)
        {
            const aiMesh* mesh = meshes[i];
            for (uint32_t x = 0; x < mesh->mNumVertices; ++x)
            {
                // Position
                glm::vec3 pos = { mesh->mVertices[x].x, mesh->mVertices[x].y, mesh->mVertices[x].z };
                out_data->vertices.push_back(pos);

                // Normals
                glm::vec3 normal = glm::vec3(0.0f);
                if (mesh->HasNormals())
                    normal = { mesh->mNormals[x].x, mesh->mNormals[x].y, mesh->mNormals[x].z};

                out_data->normals.push_back(normal);

                // UVs
                glm::vec2 uv = glm::vec2(0.0f);
                if (mesh->HasTextureCoords(0))
                    uv = { mesh->mTextureCoords[0][x].x, mesh->mTextureCoords[0][x].y };

                out_data->uvs.push_back(uv);

                //Tangents
                glm::vec3 tangents = glm::vec3(0.0f);
                if (mesh->HasTangentsAndBitangents())
                    tangents = { mesh->mTangents[x].x, mesh->mTangents[x].y, mesh->mTangents[x].z };

                out_data->tangents.push_back(tangents);

                //Colors
                glm::vec4 color = glm::vec4(0.4f, 0.2f, 0.2, 1.0f);
                if (mesh->HasVertexColors(0))
                {
                    color = { mesh->mColors[0][x].r, mesh->mColors[0][x].g,
                        mesh->mColors[0][x].b, mesh->mColors[0][x].a };
                }

                out_data->colors.push_back(color);
            }

            for (uint32_t x = 0; x < mesh->mNumFaces; ++x)
            {
                out_data->indices.push_back(mesh->mFaces[x].mIndices[0]);
                out_data->indices.push_back(mesh->mFaces[x].mIndices[1]);
                out_data->indices.push_back(mesh->mFaces[x].mIndices[2]);
            }

        }


        return true;
	}
}