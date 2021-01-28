#pragma once
#include "Core/Core.h"

#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/Vulkan/VulkanDescriptor.h"
#include "Renderer/Vulkan/VulkanPipelineSpecification.h"
#include "Renderer/Vulkan/VulkanDescriptor.h"
#include "Renderer/BufferElement.h"

namespace SmolEngine
{
	class VulkanShader;
	class VulkanTexture;
	enum class DrawMode : uint16_t;

	class VulkanPipeline
	{
	public:

		VulkanPipeline();

		~VulkanPipeline();

		/// Main
		
		bool Invalidate(VulkanPipelineSpecification& pipelineSpec);

		bool CreatePipeline(DrawMode mode);

		bool ReCreate();

		void Destroy();

		bool UpdateSamplers2D(const std::vector<VulkanTexture*>& textures, uint32_t bindingPoint, uint32_t setIndex = 0);

		bool UpdateCubeMap(const VulkanTexture* cubeMap, uint32_t bindingPoint, uint32_t setIndex = 0);

		/// Save / Load

		bool SaveCache(const std::string& fileName, DrawMode mode);

		bool CreateOrLoadCached(const std::string& fileName, DrawMode mode);

		/// Getters

		const VkPipeline& GetVkPipeline(DrawMode mode);

		const VkPipelineLayout& GetVkPipelineLayot() const;

		const VkDescriptorSet GetVkDescriptorSets(uint32_t setIndex = 0) const;

	private:

		void BuildDescriptors(VulkanShader* shader, uint32_t DescriptorSets);

		VkFormat GetVkInputFormat(ShaderDataType type);

		VkPrimitiveTopology GetVkTopology(DrawMode mode);

		VkPolygonMode GetVkPolygonMode(DrawMode mode);

	private:

		std::vector<VulkanDescriptor> m_Descriptors;
		std::vector<VkDescriptorSetLayout> m_SetLayout;

		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		VkRenderPass m_TargetRenderPass = nullptr;
		VkDevice m_Device = nullptr;

		std::unordered_map<DrawMode, VkPipelineCache> m_PipelineCaches;
		std::unordered_map<DrawMode, VkPipeline> m_Pipelines;
		std::string m_FilePath = "";

		VkDescriptorPool m_DescriptorPool = nullptr;
		VulkanPipelineSpecification m_VulkanPipelineSpecification = {};
		
	private:

		friend class GraphicsPipeline;
	};
}