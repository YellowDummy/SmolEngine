#include "stdafx.h"
#include "FBXImporter.h"

#include <ofbx.h>

namespace SmolEngine
{
	bool FBXImporter::Load(const std::string& filePath, ImportedData& out_data)
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

        const ofbx::Mesh* mesh = scene->getMesh(0);
        const ofbx::Geometry* g = mesh->getGeometry();

        uint32_t vertextCount = g->getVertexCount();
        {
            out_data.vertices.resize(vertextCount);
            out_data.normals.resize(vertextCount);
            out_data.uvs.resize(vertextCount);
            out_data.colors.resize(vertextCount);

            auto v_ptr = g->getVertices();
            auto n_ptr = g->getNormals();
            auto u_ptr = g->getUVs();
            auto c_ptr = g->getColors();

            for (uint32_t i = 0; i < g->getVertexCount(); ++i)
            {
                out_data.vertices[i].x = v_ptr->x;
                out_data.vertices[i].y = v_ptr->y;
                out_data.vertices[i].z = v_ptr->z;
                v_ptr++;

                out_data.normals[i].x = n_ptr->x;
                out_data.normals[i].y = n_ptr->y;
                out_data.normals[i].z = n_ptr->z;
                n_ptr++;

                out_data.uvs[i].x = u_ptr->x;
                out_data.uvs[i].y = u_ptr->y;
                u_ptr++;

                if (c_ptr != nullptr)
                {
                    out_data.colors[i].w = c_ptr->w;
                    out_data.colors[i].x = c_ptr->x;
                    out_data.colors[i].y = c_ptr->y;
                    out_data.colors[i].z = c_ptr->z;
                    c_ptr++;
                }
                else
                {
                    out_data.colors[i].w = 0.2f;
                    out_data.colors[i].x = 0.2f;
                    out_data.colors[i].y = 0.2f;
                    out_data.colors[i].z = 1.0f;
                }
            }
        }

        out_data.indices.resize(g->getIndexCount());
        {
            auto ptr = g->getFaceIndices();
            for (auto i = 0; i < g->getIndexCount(); ++i)
            {
                out_data.indices[i] = *ptr;
                ptr++;
            }
        }

        return true;
	}
}