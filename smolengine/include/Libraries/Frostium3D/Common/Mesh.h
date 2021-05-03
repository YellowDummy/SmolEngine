#pragma once
#include "Common/Core.h"
#include "Common/Common.h"

#include "Common/VertexBuffer.h"
#include "Common/IndexBuffer.h"
#include "Common/Animation.h"

#include <memory>

namespace Frostium
{
	struct ImportedDataGlTF;
	struct Primitive;
	class VertexBuffer;
	class IndexBuffer;
	class BufferLayout;
	class Texture;
	class Material;

	class Mesh
	{
	public:
		Mesh() = default;
		~Mesh();

		void SetMaterialID(int32_t materialID, bool apply_to_children = false);
		void SetActiveAnimation(uint32_t index);

		// Getters
		const std::vector<Mesh>& GetMeshes() const;
		uint32_t GetVertexCount() const;
		uint32_t GetAnimationsCount() const;
		VertexBuffer* GetVertexBuffer();
		IndexBuffer* GetIndexBuffer();
		Mesh* GetMeshByName(const std::string& name);
		Mesh* GetMeshByIndex(uint32_t index);
		AnimationProperties* GetAnimationProperties(uint32_t index) const;

		//Helpers
		bool IsAnimated() const;
		bool IsRootNode() const;
		void ResetAnimation(uint32_t index);
		static void Create(const std::string& filePath, Mesh* mesh);

	private:

		static bool Init(Mesh* mesh, Mesh* parent, Primitive* primitive);
		void UpdateAnimations();

	private:

		bool                                        m_Initialized = false;
		uint32_t                                    m_MaterialID = 0;
		uint32_t                                    m_VertexCount = 0;
		Mesh*                                       m_Root = nullptr;
		Ref<VertexBuffer>                           m_VertexBuffer = nullptr;
		Ref<IndexBuffer>                            m_IndexBuffer = nullptr;
		ImportedDataGlTF*                           m_ImportedData = nullptr;
		std::unordered_map<std::string, Mesh*>      m_MeshMap;
		std::vector<Mesh>                           m_Meshes;

	private:

		friend class Renderer;
		friend class Animator;
		friend class GraphicsPipeline;
	};
}