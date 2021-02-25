#include "stdafx.h"
#include "VulkanDescriptor.h"

#include "Renderer/Texture.h"
#include "Renderer/Vulkan/VulkanShader.h"
#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanBufferPool.h"
#include "Renderer/CubeTexture.h"
#include "Renderer/Vulkan/VulkanPBR.h"

namespace SmolEngine
{
	VulkanDescriptor::VulkanDescriptor()
	{
		m_Device = VulkanContext::GetDevice().GetLogicalDevice();
	}

	VulkanDescriptor::~VulkanDescriptor()
	{
		if (m_Device)
		{
			if (m_DescriptorSetLayout != VK_NULL_HANDLE)
			{
				vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, nullptr);
			}
		}
	}

	void VulkanDescriptor::GenDescriptorSet(VulkanShader* shader, VkDescriptorPool pool)
	{
		std::vector< VkDescriptorSetLayoutBinding> layouts;
		layouts.reserve(shader->m_Buffers.size() + shader->m_UniformResources.size());

		for (auto& [bindingPoint, buffer] : shader->m_Buffers)
		{
			VkDescriptorType type = buffer.Type == ShaderBufferType::Uniform ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

			VkDescriptorSetLayoutBinding layoutBinding = {};
			{
				layoutBinding.binding = buffer.BindingPoint;
				layoutBinding.descriptorType = type;
				layoutBinding.descriptorCount = static_cast<uint32_t>(buffer.Uniforms.size());
				layoutBinding.stageFlags = buffer.StageFlags;
			}

			layouts.push_back(layoutBinding);
		}

		// Samplers
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

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		{
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = static_cast<uint32_t>(layouts.size());
			layoutInfo.pBindings = layouts.data();

			VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device, &layoutInfo, nullptr, &m_DescriptorSetLayout));
		}

		VkDescriptorSetAllocateInfo allocateInfo = {};
		{
			allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocateInfo.descriptorPool = pool;
			allocateInfo.descriptorSetCount = 1;
			allocateInfo.pSetLayouts = &m_DescriptorSetLayout;

			VK_CHECK_RESULT(vkAllocateDescriptorSets(m_Device, &allocateInfo, &m_DescriptorSet));
		}
	}

	void VulkanDescriptor::GenBuffersDescriptors(VulkanShader* shader)
	{
		for (auto& [key, buffer] : shader->m_Buffers)
		{
			VkDescriptorBufferInfo descriptorBufferInfo = {};
			size_t dataSize = buffer.Size;
			VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			VkMemoryPropertyFlags mem = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT; // temp

			if (buffer.Type == ShaderBufferType::Storage)
			{
				type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

				auto& it = shader->m_Info.StorageBuffersSizes.find(buffer.BindingPoint);
				if (it != shader->m_Info.StorageBuffersSizes.end())
					dataSize = it->second;
				else
				{
					if (!VulkanBufferPool::GetSingleton()->IsBindingExist(buffer.BindingPoint))
					{
						NATIVE_ERROR("Storage buffer dataSize must be declared inside GraphicsPipelineShaderCreateInfo!");
						continue;
					}
				}

			}

			VulkanBufferPool::GetSingleton()->Add(dataSize, buffer.BindingPoint, mem,
				usage, descriptorBufferInfo);

			m_WriteSets.push_back(CreateWriteSet(m_DescriptorSet,
				buffer.BindingPoint, &descriptorBufferInfo, type));

			vkUpdateDescriptorSets(m_Device, 1, &m_WriteSets.back(), 0, nullptr);

			NATIVE_WARN("Created " + buffer.ObjectName + " {}: Members Count: {}, Binding Point: {}",
				buffer.Name, buffer.Uniforms.size(), buffer.BindingPoint);
		}
	}

	void VulkanDescriptor::GenSamplersDescriptors(VulkanShader* shader)
	{
#ifndef SMOLENGINE_OPENGL_IMPL
		m_ImageInfo = Texture::CreateWhiteTexture()->GetVulkanTexture()->m_DescriptorImageInfo;
#endif
		for (auto& [bindingPoint, res] : shader->m_UniformResources)
		{
			if (res.Dimension == 3) // cubeMap
			{
				auto& skyBox = VulkanPBR::GetSkyBox();
				assert(skyBox.IsActive());

				if (skyBox.IsActive())
				{
					VkWriteDescriptorSet writeSet = {};
					{
						writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						writeSet.dstSet = m_DescriptorSet;
						writeSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						writeSet.dstBinding = res.BindingPoint;
						writeSet.dstArrayElement = 0;
						writeSet.descriptorCount = 1;
						writeSet.pImageInfo = &skyBox.m_DescriptorImageInfo;
					}

					m_WriteSets.push_back(writeSet);

					auto& kek = m_WriteSets.back();
					vkUpdateDescriptorSets(m_Device, 1, &m_WriteSets.back(), 0, nullptr);
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
						infos[i] = m_ImageInfo;
					}
				}
				else
					infos.push_back(m_ImageInfo);

				m_WriteSets.push_back(CreateWriteSet(m_DescriptorSet,
					res.BindingPoint, infos, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER));
				vkUpdateDescriptorSets(m_Device, 1, &m_WriteSets.back(), 0, nullptr);
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

		std::vector<VkDescriptorImageInfo> infos(writeSet->descriptorCount);
		for (uint32_t i = 0; i < writeSet->descriptorCount; ++i)
		{
			if (textures.size() > i)
			{
				if (textures[i])
				{
					infos[i] = textures[i]->m_DescriptorImageInfo;
					continue;
				}
			}

			infos[i] = m_ImageInfo;
		}

		*writeSet = CreateWriteSet(m_DescriptorSet,
			bindingPoint, infos, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

		vkUpdateDescriptorSets(m_Device, 1, writeSet, 0, nullptr);
		return true;
	}

	bool VulkanDescriptor::UpdateImageResource(uint32_t bindingPoint, const VkDescriptorImageInfo& imageInfo)
	{
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

		writeSet->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSet->dstSet = m_DescriptorSet;
		writeSet->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeSet->dstBinding = bindingPoint;
		writeSet->dstArrayElement = 0;
		writeSet->descriptorCount = 1;
		writeSet->pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(m_Device, 1, writeSet, 0, nullptr);
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

		vkUpdateDescriptorSets(m_Device, 1, writeSet, 0, nullptr);
		return true;
	}

	void VulkanDescriptor::UpdateWriteSets()
	{
		vkUpdateDescriptorSets(m_Device, static_cast<uint32_t>(m_WriteSets.size()), m_WriteSets.data(), 0, nullptr);
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
			writeSet.dstArrayElement = 0;
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