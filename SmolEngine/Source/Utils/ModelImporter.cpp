#include "stdafx.h"
#include "ModelImporter.h"
#include "Core/SLog.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace SmolEngine
{
    // TODO: Add materials

    aiMatrix4x4 GLMMat4ToAi(const glm::mat4& mat)
    {
        return aiMatrix4x4(mat[0][0], mat[0][1], mat[0][2], mat[0][3],
            mat[1][0], mat[1][1], mat[1][2], mat[1][3],
            mat[2][0], mat[2][1], mat[2][2], mat[2][3],
            mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
    }

    glm::mat4 AiToGLMMat4(const aiMatrix4x4& in_mat)
    {
        glm::mat4 tmp;
        tmp[0][0] = in_mat.a1;
        tmp[1][0] = in_mat.b1;
        tmp[2][0] = in_mat.c1;
        tmp[3][0] = in_mat.d1;

        tmp[0][1] = in_mat.a2;
        tmp[1][1] = in_mat.b2;
        tmp[2][1] = in_mat.c2;
        tmp[3][1] = in_mat.d2;

        tmp[0][2] = in_mat.a3;
        tmp[1][2] = in_mat.b3;
        tmp[2][2] = in_mat.c3;
        tmp[3][2] = in_mat.d3;

        tmp[0][3] = in_mat.a4;
        tmp[1][3] = in_mat.b4;
        tmp[2][3] = in_mat.c4;
        tmp[3][3] = in_mat.d4;
        return tmp;
    }

    void LoadBones( std::map<std::string, uint32_t>& BoneMapping, const aiMesh* pMesh, ImportedComponent& comp)
    {
        for (uint32_t i = 0; i < pMesh->mNumBones; i++) {

            uint32_t boneIndex = 0;
            std::string BoneName(pMesh->mBones[i]->mName.data);

            if (BoneMapping.find(BoneName) == BoneMapping.end())
            {
                boneIndex = comp.NumBones;
                comp.NumBones++;

                BoneInfo bi;
                bi.BoneOffset = AiToGLMMat4(pMesh->mBones[i]->mOffsetMatrix);
                comp.BoneInfo.push_back(bi);
                BoneMapping[BoneName] = boneIndex;
            }
            else
            {
                boneIndex = BoneMapping[BoneName];
            }

            BoneMapping[BoneName] = boneIndex;

            for (uint32_t j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {

                uint32_t id = pMesh->mBones[i]->mWeights[j].mVertexId;
                float weight = pMesh->mBones[i]->mWeights[j].mWeight;

                for (uint32_t k = 0; k < 4; ++k)
                {
                    comp.VertexData[id].BoneIDs[k] = boneIndex;
                    comp.VertexData[id].Weight[k] = weight;
                }
            }
        }
    }

    void ProcessMesh(aiMesh** meshes, const aiNode* node, ImportedData* out_data)
    {
        ImportedComponent component = {};
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;

        for (uint32_t y = 0; y < node->mNumMeshes; ++y)
        {
            uint32_t id = node->mMeshes[y];
            const aiMesh* mesh = meshes[id];

            vertexCount += mesh->mNumVertices;
            indexCount += mesh->mNumFaces;
        }

        glm::mat4 Transformation = AiToGLMMat4(node->mTransformation);

        component.VertexData.reserve(vertexCount);
        component.Indices.reserve(indexCount * 3);

        for (uint32_t y = 0; y < node->mNumMeshes; ++y)
        {
            uint32_t id = node->mMeshes[y];
            const aiMesh* mesh = meshes[id];

            for (uint32_t x = 0; x < mesh->mNumVertices; ++x)
            {
                PBRVertex data = {};

                // Position
                data.Pos = glm::vec4(mesh->mVertices[x].x, mesh->mVertices[x].y, mesh->mVertices[x].z, 1) * Transformation;

                // Normals
                if (mesh->HasNormals())
                    data.Normals = { mesh->mNormals[x].x, mesh->mNormals[x].y, mesh->mNormals[x].z };

                // UVs
                if (mesh->HasTextureCoords(0))
                    data.UVs = { mesh->mTextureCoords[0][x].x, mesh->mTextureCoords[0][x].y };

                //Tangents
                if (mesh->HasTangentsAndBitangents())
                    data.Tangent = { mesh->mTangents[x].x, mesh->mTangents[x].y, mesh->mTangents[x].z, 1.0f};

                component.VertexData.emplace_back(data);
            }

            //Bones
            if (mesh->HasBones())
            {
                out_data->BoneMapping.clear();
                LoadBones(out_data->BoneMapping, mesh, component);
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

    bool ModelImporter::Load(const std::string& filePath, ImportedData* out_data)
    {
        Assimp::Importer importer;
        const aiScene* g_scene = importer.ReadFile(filePath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
        auto meshes = g_scene->mMeshes;

        out_data->Animations.resize(g_scene->mNumAnimations);
        for (uint32_t i = 0; i < g_scene->mNumAnimations; ++i)
        {
            auto animation = g_scene->mAnimations[i];
        }

        uint32_t childsCount = 0;
        for (uint32_t i = 0; i < g_scene->mRootNode->mNumChildren; ++i)
        {
            const aiNode* node = g_scene->mRootNode->mChildren[i];
            if (node->mNumMeshes > 0)
                childsCount++;

            for (uint32_t j = 0; j < node->mNumChildren; j++)
            {
                auto child = node->mChildren[j];
                if (child->mNumMeshes > 0)
                    childsCount++;
            }
        }
        out_data->Components.reserve(childsCount);

        for (uint32_t i = 0; i < g_scene->mRootNode->mNumChildren; ++i)
        {
            const aiNode* node = g_scene->mRootNode->mChildren[i];

            // Main Node
            if (node->mNumMeshes > 0)
                ProcessMesh(meshes, node, out_data);

            // Child Nodes
            for (uint32_t i = 0; i < node->mNumChildren; i++)
            {
                auto child = node->mChildren[i];
                if (child->mNumMeshes > 0)
                    ProcessMesh(meshes, child, out_data);
            }
        }

        return true;
	}
}