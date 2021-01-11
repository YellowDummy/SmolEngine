#include "stdafx.h"
#include "FBXImporter.h"

#include <ofbx.h>

namespace SmolEngine
{
	bool FBXImporter::Load(const std::string& filePath, ImportedData* out_data)
	{
        ofbx::IScene* scene = nullptr;
        FILE* fp = fopen(filePath.c_str(), "rb");
        assert(fp);
        if (!fp)
            return false;

        fseek(fp, 0, SEEK_END);
        long file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        auto* content = new ofbx::u8[file_size];
        fread(content, 1, file_size, fp);
        scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);
        assert(scene);
        if (!scene)
            return false;

        uint32_t vertextCount = 0;
        uint32_t indexCount = 0;
        for (uint32_t m = 0; m < scene->getMeshCount(); m++)
        {
            const ofbx::Mesh* mesh = scene->getMesh(m);
            const ofbx::Geometry* g = mesh->getGeometry();
            vertextCount += g->getVertexCount();
            indexCount += g->getIndexCount();
        }

        out_data->vertices.reserve(vertextCount);
        out_data->normals.reserve(vertextCount);
        out_data->uvs.reserve(vertextCount);
        out_data->colors.reserve(vertextCount);

        uint32_t startIndex = 0;
        for (uint32_t m = 0; m < scene->getMeshCount(); m++)
        {
            const ofbx::Mesh* mesh = scene->getMesh(m);
            const ofbx::Geometry* g = mesh->getGeometry();

            auto v_ptr = g->getVertices();
            auto n_ptr = g->getNormals();
            auto u_ptr = g->getUVs();
            auto c_ptr = g->getColors();

            for (uint32_t i = 0; i < g->getVertexCount(); ++i)
            {
                out_data->vertices.push_back({ v_ptr->x, v_ptr->y, v_ptr->z });
                v_ptr++;

                out_data->normals.push_back({ n_ptr->x, n_ptr->y, n_ptr->z });
                n_ptr++;

                out_data->uvs.push_back({ u_ptr->x, u_ptr->y});
                u_ptr++;

                c_ptr != nullptr ? out_data->colors.push_back({ c_ptr->w, c_ptr->x, c_ptr->y, c_ptr->z })
                    : out_data->colors.push_back({ 1.0f, 1.0f, 1.0f, 1.0f });
            }

            out_data->indices.reserve(indexCount);
            {
                auto ptr = g->getFaceIndices();
                for (auto i = 0; i < g->getIndexCount(); ++i)
                {
                    out_data->indices.push_back(*ptr);
                    ptr++;
                }
            }
        }

        return true;
	}
}