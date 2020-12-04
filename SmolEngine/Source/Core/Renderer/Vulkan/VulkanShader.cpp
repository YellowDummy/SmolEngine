#include "stdafx.h"
#include "VulkanShader.h"

#include "Core/Renderer/Vulkan/VulkanContext.h"

namespace SmolEngine
{
    VulkanShader::VulkanShader()
    {

    }

    VulkanShader::~VulkanShader()
    {

    }

    bool VulkanShader::Init(const std::string& vertexPath, const std::string& fragmentPath, bool usePrecompiledBinaries, bool optimize, const std::string& computePath)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        std::unordered_map<ShaderType, VkShaderModule> shaderModules;
        std::unordered_map<ShaderType, std::vector<uint32_t>> binaryData;

        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);

        if (optimize)
            options.SetOptimizationLevel(shaderc_optimization_level_performance);

        // Vertex
        assert(LoadOrCompile(compiler, options, vertexPath, shaderc_shader_kind::shaderc_vertex_shader, usePrecompiledBinaries, binaryData) == true);

        // Fragment
        assert(LoadOrCompile(compiler, options, fragmentPath, shaderc_shader_kind::shaderc_fragment_shader, usePrecompiledBinaries, binaryData) == true);

        // Compute 
        if (!computePath.empty())
        {
            assert(LoadOrCompile(compiler, options, computePath, shaderc_shader_kind::shaderc_compute_shader, usePrecompiledBinaries, binaryData) == true);
        }

        // Vulkan Shader Creation

        if (!binaryData.empty())
        {
            const auto& device = *VulkanContext::GetDevice().GetLogicalDevice();
            for (const auto& info : binaryData)
            {
                const auto& [type, data] = info;
                auto& shaderModule = shaderModules[type];

                VkShaderModuleCreateInfo shaderModuleCI = {};
                {
                    shaderModuleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                    shaderModuleCI.codeSize = data.size() * sizeof(uint32_t);
                    shaderModuleCI.pCode = data.data();

                    VK_CHECK_RESULT(vkCreateShaderModule(device, &shaderModuleCI, nullptr, &shaderModule));
                }


                VkPipelineShaderStageCreateInfo pipelineShaderStageCI = {};
                {
                    pipelineShaderStageCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                    pipelineShaderStageCI.stage = GetVkShaderStage(type);
                    pipelineShaderStageCI.pName = "main";
                    pipelineShaderStageCI.module = shaderModule;


                    assert(pipelineShaderStageCI.module != VK_NULL_HANDLE);
                }

                m_PipelineShaderStages.emplace_back(std::move(pipelineShaderStageCI));

                // Reflection

                Reflect(data, type);
            }
        }
        
        BuildDescriptors();
        m_IsPrecompiled = usePrecompiledBinaries;
        m_Optimize = optimize;

        return true;
    }

    bool VulkanShader::Reload()
    {
        if (!m_FilePaths.empty())
        {
            return Init(m_FilePaths[ShaderType::Vertex], m_FilePaths[ShaderType::Fragment], false, m_Optimize, m_FilePaths[ShaderType::Compute]);
        }

        return false;
    }

    void VulkanShader::SetUniformBuffer(size_t bindingPoint, const void* data, size_t size, uint32_t offset)
    {
        const auto& result = m_UniformBuffers.find(bindingPoint);
        if (result != m_UniformBuffers.end())
        {
            auto& buffer = result->second;
            buffer.VkBuffer.SetData(data, size, offset);
            return;
        }

        NATIVE_ERROR("UBO not found, binding point: {}", bindingPoint);
        abort(); // temp
    }

    void VulkanShader::BuildDescriptors()
    {
        m_Descriptors.resize(m_UniformBuffers.size());
        m_VkDescriptors.resize(m_UniformBuffers.size());
        m_VkDescriptorSetLayout.reserve(m_UniformBuffers.size());
        const auto& device = *VulkanContext::GetDevice().GetLogicalDevice();

        // UBO's
        uint32_t index = 0;
        for (auto& uboInfo : m_UniformBuffers)
        {
            auto& [bindingPoint, buffer] = uboInfo;

            VkDescriptorSetLayoutBinding layoutBinding = {};
            {
                layoutBinding.binding = buffer.BindingPoint;
                layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                layoutBinding.descriptorCount = 1;
                layoutBinding.stageFlags = buffer.StageFlags;
            }

            buffer.VkBuffer.Create(buffer.Size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
            buffer.DesriptorBufferInfo.buffer = buffer.VkBuffer.GetBuffer();
            buffer.DesriptorBufferInfo.offset = 0;
            buffer.DesriptorBufferInfo.range = buffer.VkBuffer.GetSize();

            VkDescriptorSetLayoutCreateInfo layoutInfo = {};
            {
                layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                layoutInfo.bindingCount = 1;
                layoutInfo.pBindings = &layoutBinding;

                VkDescriptorSetLayout temp = nullptr;
                VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &temp));
                m_VkDescriptorSetLayout.push_back(temp);

                auto& vkDescriptor = m_VkDescriptors[index];
                auto& descriptor = m_Descriptors[index];

                vkDescriptor = descriptor.Init(temp, buffer.BindingPoint, &buffer.DesriptorBufferInfo);
            }

            NATIVE_WARN("Created UBO {}: Members Count: {}, Binding Point: {}", buffer.Name, buffer.Uniforms.size(), buffer.BindingPoint);
            index++;
        }

        // Samplers
        for (const auto& info : m_UniformResources)
        {
            const auto& [bindingPoint, res] = info;
            NATIVE_WARN("UniformResource\nSampler: {}, Location: {}, Dim: {}, Name: {}, Binding: {}", res.Sampler, res.Location, res.Dimension, bindingPoint);
        }
    }

    bool VulkanShader::SaveSPIRVBinaries(const std::string& filePath, const std::vector<uint32_t>& data)
    {
        FILE* f = fopen(GetCachedPath(filePath).c_str(), "w");
        if (f)
        {
            fseek(f, 0, SEEK_END);
            uint64_t size = ftell(f);
            fseek(f, 0, SEEK_SET);

            fwrite(data.data(), sizeof(uint32_t), data.size(), f);
            fclose(f);

            return true;
        }

        NATIVE_ERROR(GetCachedPath(filePath).c_str());
        return false;
    }

    const std::vector<uint32_t> VulkanShader::LoadSPIRVBinaries(const std::string& filePath)
    {
       FILE* f = fopen(GetCachedPath(filePath).c_str(), "rb");
       if (f)
       {
           fseek(f, 0, SEEK_END);
           uint64_t size = ftell(f);
           fseek(f, 0, SEEK_SET);
           std::vector<uint32_t> data(size / sizeof(uint32_t));

           fread(data.data(), sizeof(uint32_t), data.size(), f);
           fclose(f);
           return data;
       }

       return std::vector<uint32_t>();
    }

    void VulkanShader::Reflect(const std::vector<uint32_t>& data, ShaderType shaderType)
    {
        const auto& device = *VulkanContext::GetDevice().GetLogicalDevice();
        spirv_cross::Compiler compiler(data);
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        uint32_t bufferIndex = 0;
        for (const auto& res : resources.uniform_buffers)
        {
            auto& type = compiler.get_type(res.base_type_id);
            int bufferElements = type.member_types.size();

            UniformBuffer buffer = {};
            {
                buffer.Name = res.name;
                buffer.BindingPoint = compiler.get_decoration(res.id, spv::DecorationBinding);
                buffer.Size = compiler.get_declared_struct_size(type);
                buffer.Index = bufferIndex;
                buffer.StageFlags = GetVkShaderStage(shaderType);

                buffer.Uniforms.reserve(bufferElements);
            }

            for (uint32_t i = 0; i < bufferElements; ++i)
            {
                Uniform uniform = {};
                {
                    uniform.Name = compiler.get_member_name(type.self, i);
                    uniform.Type = compiler.get_type(type.member_types[i]);
                    uniform.Size = compiler.get_declared_struct_member_size(type, i);
                    uniform.Offset = compiler.type_struct_member_offset(type, i);
                }

                buffer.Uniforms.push_back(uniform);
            }

            m_UniformBuffers[buffer.BindingPoint] = std::move(buffer);

            bufferIndex++;
        }

        int32_t sampler = 0;
        for (const auto& res : resources.sampled_images)
        {
            UniformResource resBuffer = {};
            {
                resBuffer.Type = compiler.get_type(res.type_id);
                resBuffer.Location = compiler.get_decoration(res.id, spv::DecorationLocation);
                resBuffer.Dimension = resBuffer.Type.image.dim;
                resBuffer.Sampler = sampler;
                resBuffer.ArraySize = resBuffer.Type.array[0];
            }

            int32_t samplers[LayerDataBuffer::MaxTextureSlot];
            for (uint32_t i = 0; i < LayerDataBuffer::MaxTextureSlot; i++)
            {
                samplers[i] = i;
            }

            /// Upload vulkan samples

            m_UniformResources[compiler.get_decoration(res.id, spv::DecorationBinding)] = std::move(resBuffer);
            sampler++;

        }
    }

    bool VulkanShader::LoadOrCompile(const shaderc::Compiler& compiler, const shaderc::CompileOptions& options, const std::string& filePath,
        shaderc_shader_kind shaderType, bool usePrecompiledBinaries, std::unordered_map<ShaderType, std::vector<uint32_t>>& out_binaryData)
    {
        ShaderType type = GetShaderType(shaderType);
        m_FilePaths[type] = filePath;

        if (usePrecompiledBinaries)
        {
            if (std::filesystem::exists(GetCachedPath(filePath)))
            {
                out_binaryData.emplace(type, LoadSPIRVBinaries(filePath));
                return true;
            }
            else
            {
                goto COMPILE;
            }

        }
        else
        {
            COMPILE:
            const std::string& load_result = LoadShaderSource(filePath);
            const shaderc::SpvCompilationResult result = CompileToSPIRV(compiler, options, load_result, shaderType, filePath);
            if (result.GetCompilationStatus() == shaderc_compilation_status_success)
            {
                std::vector<uint32_t> data(result.cbegin(), result.cend());
                out_binaryData.emplace(type, data);
                SaveSPIRVBinaries(filePath, data);
            }

            return result.GetCompilationStatus() == shaderc_compilation_status_success;
        }

        return false;
    }

    const shaderc::SpvCompilationResult VulkanShader::CompileToSPIRV(const shaderc::Compiler& comp, const shaderc::CompileOptions& options, const std::string& source, shaderc_shader_kind type, const std::string& shaderName) const
    {
        auto result = comp.CompileGlslToSpv(source, type, shaderName.c_str(), options);
        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            NATIVE_ERROR(result.GetErrorMessage());
            assert(result.GetCompilationStatus() == shaderc_compilation_status_success);
        }

        return result;
    }

    const std::string VulkanShader::LoadShaderSource(const std::string& filePath)
    {
        if (filePath.empty())
        {
            return filePath;
        }

        std::ifstream file(filePath);
        std::stringstream buffer;

        if (!file)
        {
            NATIVE_ERROR("Could not open the file: {}", filePath);
            assert(file);
        }

        buffer << file.rdbuf();
        file.close();

        return buffer.str();
    }

    const std::string VulkanShader::GetCachedPath(const std::string& filePath)
    {
        std::filesystem::path p = filePath;
        auto path = p.parent_path() / "Cached" / (p.filename().string() + ".cached");
        return path.string();
    }

    ShaderType VulkanShader::GetShaderType(shaderc_shader_kind shadercType)
    {
        switch (shadercType)
        {
        case shaderc_shader_kind::shaderc_vertex_shader:
        {
            return ShaderType::Vertex;
        }
        case shaderc_shader_kind::shaderc_fragment_shader:
        {
            return ShaderType::Fragment;
        }
        case shaderc_shader_kind::shaderc_compute_shader:
        {
            return ShaderType::Compute;
        }
        default:
            return ShaderType::Invalid;
        }
    }

    VkShaderStageFlagBits VulkanShader::GetVkShaderStage(ShaderType type)
    {
        switch (type)
        {
        case SmolEngine::ShaderType::Vertex:
        {
            return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
        }
        case SmolEngine::ShaderType::Fragment:
        {
            return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
        }
        case SmolEngine::ShaderType::Compute:
        {
            return VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
        }
        case SmolEngine::ShaderType::Geometry:
        {
            return VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
        }
        default:
        {
            return VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
        }

        }
    }

    const std::vector<VkPipelineShaderStageCreateInfo>& VulkanShader::GetVkPipelineShaderStages() const
    {
        return m_PipelineShaderStages;
    }

    const std::vector<VkDescriptorSetLayout>& VulkanShader::GetVkDescriptorSetLayout() const
    {
        return m_VkDescriptorSetLayout;
    }

    const std::vector<VulkanDescriptor>& VulkanShader::GetDescriptors() const
    {
        return m_Descriptors;
    }

    const std::vector<VkDescriptorSet>& VulkanShader::GetVkDescriptors() const
    {
        return m_VkDescriptors;
    }
}