#include "stdafx.h"
#include "VulkanDescriptor.h"

#include "Renderer/Texture.h"
#include "Renderer/Vulkan/VulkanShader.h"
#include "Renderer/Vulkan/VulkanContext.h"

namespace SmolEngine
{
	VulkanDescriptor::~VulkanDescriptor()
	{
		const auto& device = VulkanContext::GetDevice().GetLogicalDevice();
		if (m_DescriptorSetLayout != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
		}
	}

	void VulkanDescriptor::GenDescriptorSet(VulkanShader* shader, VkDescriptorPool pool)
	{
		auto device = VulkanContext::GetDevice().GetLogicalDevice();
		std::vector< VkDescriptorSetLayoutBinding> layouts;
		layouts.reserve(shader->m_UniformResources.size() + shader->m_UniformResources.size());

		for (auto& uboInfo : shader->m_UniformBuffers)
		{
			auto& [bindingPoint, buffer] = uboInfo;

			VkDescriptorSetLayoutBinding layoutBinding = {};
			{
				layoutBinding.binding = buffer.BindingPoint;
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				layoutBinding.descriptorCount = static_cast<uint32_t>(buffer.Uniforms.size());
				layoutBinding.stageFlags = buffer.StageFlags;
			}

			buffer.VkBuffer.Create(buffer.Size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
			buffer.DesriptorBufferInfo.buffer = buffer.VkBuffer.GetBuffer();
			buffer.DesriptorBufferInfo.offset = 0;
			buffer.DesriptorBufferInfo.range = buffer.VkBuffer.GetSize();

			layouts.push_back(layoutBinding);
		}

		if (!shader->m_UniformResources.empty())
		{
			for (auto& info : shader->m_UniformResources)
			{
				auto& [bindingPoint, res] = info;
				VkDescriptorSetLayoutBinding layoutBinding = {};
				{
					layoutBinding.binding = res.BindingPoint;
					layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					res.ArraySize > 0 ? layoutBinding.descriptorCount = res.ArraySize : layoutBinding.descriptorCount = 1;
					layoutBinding.stageFlags = res.StageFlags;
				}

				layouts.push_back(layoutBinding);
			}
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		{
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = static_cast<uint32_t>(layouts.size());
			layoutInfo.pBindings = layouts.data();

			VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_DescriptorSetLayout));
		}

		VkDescriptorSetAllocateInfo allocateInfo = {};
		{
			allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocateInfo.descriptorPool = pool;
			allocateInfo.descriptorSetCount = 1;
			allocateInfo.pSetLayouts = &m_DescriptorSetLayout;

			VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocateInfo, &m_DescriptorSet));
		}
	}

	void VulkanDescriptor::GenUniformBuffersDescriptors(VulkanShader* shader)
	{
		auto device = VulkanContext::GetDevice().GetLogicalDevice();

		if (!shader->m_UniformBuffers.empty())
		{
			for (auto& uboInfo : shader->m_UniformBuffers)
			{
				auto& [bindingPoint, buffer] = uboInfo;

				m_WriteSets.push_back(CreateWriteSet(m_DescriptorSet,
					buffer.BindingPoint, &buffer.DesriptorBufferInfo));

				vkUpdateDescriptorSets(device, 1, &m_WriteSets.back(), 0, nullptr);

				NATIVE_WARN("Created UBO {}: Members Count: {}, Binding Point: {}", buffer.Name, buffer.Uniforms.size(), buffer.BindingPoint);
			}
		}
	}

	void VulkanDescriptor::GenSamplersDescriptors(VulkanShader* shader, VulkanTexture* cubeMap)
	{
		auto device = VulkanContext::GetDevice().GetLogicalDevice();

		if (!shader->m_UniformResources.empty())
		{
			VkDescriptorImageInfo whiteTexture = Texture::CreateWhiteTexture()->GetVulkanTexture()->m_DescriptorImageInfo;
			for (auto& [bindingPoint, res] : shader->m_UniformResources)
			{
				if (res.Dimension > 1) // cubeMap
				{
					if (cubeMap != nullptr)
					{
						VkWriteDescriptorSet writeSet = {};
						{
							writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
							writeSet.dstSet = m_DescriptorSet;
							writeSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
							writeSet.dstBinding = res.BindingPoint;
							writeSet.dstArrayElement = 0;
							writeSet.descriptorCount = 1;
							writeSet.pImageInfo = &cubeMap->m_DescriptorImageInfo;
						}

						m_WriteSets.push_back(writeSet);

						auto& kek = m_WriteSets.back();
						vkUpdateDescriptorSets(device, 1, &m_WriteSets.back(), 0, nullptr);
					}
					else
					{
						NATIVE_ERROR("GraphicsPipeline: Skybox is nullptr!");
						abort();
					}
				}
				else // sampler2d
				{
					std::vector<VkDescriptorImageInfo> infos;

					if (res.ArraySize > 0)
					{
						infos.resize(res.ArraySize);
						for (uint32_t i = 0; i < res.ArraySize; ++i)
						{
							infos[i] = whiteTexture;
						}
					}
					else
						infos.push_back(whiteTexture);

					m_WriteSets.push_back(CreateWriteSet(m_DescriptorSet,
						res.BindingPoint, infos, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER));
					vkUpdateDescriptorSets(device, 1, &m_WriteSets.back(), 0, nullptr);
				}
			}
		}
	}

	bool VulkanDescriptor::Update2DSamplers(const std::vector<VulkanTexture*>& textures, uint32_t bindingPoint)
	{
		VkWriteDescriptorSet* writeSet = nullptr;
		for (auto& set: m_WriteSets)
		{
			if (set.dstBinding == bindingPoint)
			{
				writeSet = &set;
				break;
			}
		}

		if (!writeSet)
			return false;

		std::vector<VkDescriptorImageInfo> infos(textures.size());
		for (uint32_t i = 0; i < textures.size(); ++i)
		{
			infos[i] = textures[i]->m_DescriptorImageInfo;
		}

		*writeSet = CreateWriteSet(m_DescriptorSet,
			bindingPoint, infos, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

		auto device = VulkanContext::GetDevice().GetLogicalDevice();
		vkUpdateDescriptorSets(device, 1, writeSet, 0, nullptr);
		return true;
	}

	bool VulkanDescriptor::UpdateCubeMap(const VulkanTexture* cubeMap, uint32_t bindingPoint)
	{
		if (!cubeMap)
			return false;

		VkWriteDescriptorSet* writeSet = nullptr;
		for (auto& set : m_WriteSets)
		{
			if (set.dstBinding == bindingPoint)
			{
				writeSet = &set;
				break;
			}
		}

		if (!writeSet)
			return false;

		*writeSet = CreateWriteSet(m_DescriptorSet,
			bindingPoint, { cubeMap->m_DescriptorImageInfo }, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

		auto device = VulkanContext::GetDevice().GetLogicalDevice();
		vkUpdateDescriptorSets(device, 1, writeSet, 0, nullptr);
		return true;
	}

	void VulkanDescriptor::UpdateWriteSets()
	{
		auto device = VulkanContext::GetDevice().GetLogicalDevice();
		vkUpdateDescriptorSets(device, static_cast<uint32_t>(m_WriteSets.size()), m_WriteSets.data(), 0, nullptr);
	}

	const VkDescriptorSet VulkanDescriptor::GetDescriptorSets() const
	{
		return m_DescriptorSet;
	}

	VkWriteDescriptorSet VulkanDescriptor::CreateWriteSet(VkDescriptorSet descriptorSet, uint32_t binding, 
		VkDescriptorBufferInfo* descriptorBufferInfo, VkDescriptorType descriptorType)
	{
		const auto& device = *VulkanContext::GetDevice().GetLogicalDevice();

		VkWriteDescriptorSet writeSet = {};
		{
			writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeSet.dstSet = descriptorSet;
			writeSet.dstBinding = binding;
			writeSet.dstArrayElement = 0; // temp
			writeSet.descriptorType = descriptorType;
			writeSet.descriptorCount = 1;
			writeSet.pBufferInfo = descriptorBufferInfo;
		}

		return writeSet;
	}

	VkWriteDescriptorSet VulkanDescriptor::CreateWriteSet(VkDescriptorSet descriptorSet, uint32_t binding,
		VkDescriptorImageInfo* imageInfo, VkDescriptorType descriptorType)
	{
		VkWriteDescriptorSet writeSet = {};
		{
			writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeSet.dstSet = descriptorSet;
			writeSet.descriptorType = descriptorType;
			writeSet.descriptorCount = 1;
			writeSet.dstArrayElement = 0;
			writeSet.dstBinding = binding;
			writeSet.pImageInfo = imageInfo;
		}

		return writeSet;
	}

	VkWriteDescriptorSet VulkanDescriptor::CreateWriteSet(VkDescriptorSet descriptorSet, uint32_t binding,
		const std::vector<VkDescriptorImageInfo>& descriptorimageInfos, VkDescriptorType descriptorType)
	{
		VkWriteDescriptorSet writeSet = {};
		{
			writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeSet.dstSet = descriptorSet;
			writeSet.descriptorType = descriptorType;
			writeSet.dstBinding = binding;
			writeSet.dstArrayElement = 0;
			writeSet.descriptorCount = static_cast<uint32_t>(descriptorimageInfos.size());
			writeSet.pImageInfo = descriptorimageInfos.data();
		}

		return writeSet;
	}
}