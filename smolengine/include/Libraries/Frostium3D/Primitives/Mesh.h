#pragma once
#include "Common/Core.h"
#include "Common/Common.h"
#include "Common/Animation.h"

#include "Primitives/VertexBuffer.h"
#include "Primitives/IndexBuffer.h"

#include <memory>

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
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

		// Setters
		void                  SetMaterialID(int32_t materialID, bool apply_to_children = false);
		void                  SetActiveAnimation(uint32_t index);
		// Getters			  
		std::vector<Mesh>&    GetChilds();
		BoundingBox&          GetAABB();
		uint32_t              GetVertexCount() const;
		uint32_t              GetAnimationsCount() const;
		uint32_t              GetChildCount() const;
		uint32_t              GetMaterialID() const;
		uint32_t              GetMeshID() const;
		std::string           GetName() const;
		VertexBuffer*         GetVertexBuffer();
		IndexBuffer*          GetIndexBuffer();
		Mesh*                 GetMeshByName(const std::string& name);
		Mesh*                 GetMeshByIndex(uint32_t index);
		AnimationProperties*  GetAnimationProperties(uint32_t index) const;
		// Helpers			  
		bool                  IsAnimated() const;
		bool                  IsRootNode() const;
		bool                  IsReady() const;
		void                  ResetAnimation(uint32_t index);
		// Factory			  
		static void           Create(const std::string& filePath, Mesh* mesh);
							  
	private:				  
		static bool           Init(Mesh* mesh, Mesh* parent, Primitive* primitive);
		void                  UpdateAnimations();

	private:
		Mesh*                 m_Root = nullptr;
		Ref<VertexBuffer>     m_VertexBuffer = nullptr;
		Ref<IndexBuffer>      m_IndexBuffer = nullptr;
		Ref<ImportedDataGlTF> m_ImportedData = nullptr;
		bool                  m_Initialized = false;
		uint32_t              m_MaterialID = 0;
		uint32_t              m_VertexCount = 0;
		uint32_t              m_ID = 0;
		std::string           m_Name = "";
		BoundingBox           m_AABB = {};
		std::vector<Mesh>     m_Childs;

	private:
		friend class DeferredRenderer;
		friend class Animator;
		friend class GraphicsPipeline;
	};
}