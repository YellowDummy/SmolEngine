#pragma once
#include "Common/Core.h"
#include "Common/Common.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace Frostium
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
									     
	struct Animation				     
	{								     
		std::string                      Name;
		std::vector<AnimationSampler>    Samplers;
		std::vector<AnimationChannel>    Channels;
		float                            Start = std::numeric_limits<float>::max();
		float                            End = std::numeric_limits<float>::min();
		float                            CurrentTime = 0.0f;
	};

	struct Primitive
	{
		uint32_t                         FirstIndex;
		uint32_t                         IndexCount;
		int32_t                          MaterialIndex;
	};

	struct ImportedDataGlTF
	{
		ImportedDataGlTF() = default;
		~ImportedDataGlTF();
		// TEMP
		void UpdateJoints(glTFNode* node);
		void UpdateAnimation(float deltaTime);
		glm::mat4 GetNodeMatrix(glTFNode* node);

		uint32_t                         ActiveAnimation = 0;

		std::vector<Skin>                Skins;
		std::vector<glTFNode*>           Nodes;
		std::vector<Animation>           Animations;
		std::vector<PBRVertex>           VertexBuffer;
		std::vector<uint32_t>            IndexBuffer;
		std::vector<Primitive>           Primitives;
	};

	class glTFImporter
	{
	public:

		static bool Import(const std::string& filePath, ImportedDataGlTF* out_data);

	};
}