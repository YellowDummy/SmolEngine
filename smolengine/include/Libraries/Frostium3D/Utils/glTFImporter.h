#pragma once
#include "Common/Core.h"
#include "Common/Common.h"
#include "Common/Animation.h"
#include "Utils/GLM.h"

#include <vector>
#include <string>
#include <limits>

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	struct glTFNode
	{								     
		glTFNode*                        Parent;
		uint32_t                         Index;
		std::vector<glTFNode*>           Children;
		glm::vec3                        Translation{};
		glm::vec3                        Scale{ 1.0f };
		glm::quat                        Rotation{};
		int32_t                          Skin = -1;
		glm::mat4                        Matrix;
									    					     
									     
		glm::mat4 GetLocalMatrix();
	};

	struct Skin
	{
		std::string                      Name;
		glTFNode*                        SkeletonRoot = nullptr;
		std::vector<glm::mat4>           InverseBindMatrices;
		std::vector<glTFNode*>           Joints;
	};								     
									     
	struct AnimationSampler			     
	{								     
		std::string                      Interpolation;
		std::vector<float>               Inputs;
		std::vector<glm::vec4>           OutputsVec4;
	};								     
									     
	struct AnimationChannel			     
	{								     
		std::string                      Path;
		glTFNode*                        Node;
		uint32_t                         SamplerIndex;
	};								     
									     
	struct glTFAnimation
	{								     
		std::string                      Name;
		AnimationProperties              Properties{};
		std::vector<AnimationSampler>    Samplers;
		std::vector<AnimationChannel>    Channels;
	};

	struct Primitive
	{
		std::string                      MeshName = "";
		std::vector<PBRVertex>           VertexBuffer;
		std::vector<uint32_t>            IndexBuffer;
		BoundingBox                      AABB;
	};

	struct ImportedDataGlTF
	{
		ImportedDataGlTF() = default;
		~ImportedDataGlTF();

		void UpdateJoints(glTFNode* node, AnimationProperties* data);
		void UpdateAnimation();
		void ResetAnimation(uint32_t index);

		glm::mat4 GetNodeMatrix(glTFNode* node);

	public:
		uint32_t                         ActiveAnimation = 0;
		std::vector<Skin>                Skins;
		std::vector<glTFNode*>           Nodes;
		std::vector<glTFAnimation>       Animations;
		std::vector<Primitive>           Primitives;
	};

	class glTFImporter
	{
	public:

		static bool Import(const std::string& filePath, ImportedDataGlTF* out_data);

	};
}