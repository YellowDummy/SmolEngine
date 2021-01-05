#include "stdafx.h"
#include "VulkanShader.h"

#include "Core/Renderer/Vulkan/VulkanContext.h"
#include "Core/AssetManager.h"

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
        std::unordered_map<ShaderType, std::vector<uint32_t>> binaryData;
        m_MinUboAlignment = VulkanContext::GetDevice().GetDeviceProperties()->limits.minUniformBufferOffsetAlignment;

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
        
        m_IsPrecompiled = usePrecompiledBinaries;
        m_Optimize = optimize;

        return true;
    }

    bool VulkanShader::Reload()
    {
        m_UniformBuffers.clear();
        m_UniformResources.clear();
        m_PipelineShaderStages.clear();
        m_VkPushConstantRanges.clear();

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
            size_t dynamicAlignment = size;
            if (size > 0)
            {
                dynamicAlignment = (dynamicAlignment + m_MinUboAlignment - 1) & ~(m_MinUboAlignment - 1);
            }

            auto& buffer = result->second;
            buffer.VkBuffer.SetData(data, size, offset);
            return;
        }

        NATIVE_ERROR("UBO not found, binding point: {}", bindingPoint);
        abort(); // temp
    }

    bool VulkanShader::SaveSPIRVBinaries(const std::string& filePath, const std::vector<uint32_t>& data)
    {
        FILE* f = fopen(AssetManager::GetCachedPath(filePath, CachedPathType::Shader).c_str(), "w");
        if (f)
        {
            fseek(f, 0, SEEK_END);
            uint64_t size = ftell(f);
            fseek(f, 0, SEEK_SET);

            fwrite(data.data(), sizeof(uint32_t), data.size(), f);
            fclose(f);

            return true;
        }

        NATIVE_ERROR(AssetManager::GetCachedPath(filePath, CachedPathType::Shader).c_str());
        return false;
    }

    VkShaderModule VulkanShader::LoadSPIRVBinaries(const std::string& filePath, ShaderType type)
    {
        VkShaderModule shaderModule = VK_NULL_HANDLE;
        FILE* f = fopen(AssetManager::GetCachedPath(filePath, CachedPathType::Shader).c_str(), "rb");
        if (f)
        {
            fseek(f, 0, SEEK_END);
            uint64_t size = ftell(f);
            fseek(f, 0, SEEK_SET);
            std::vector<uint32_t> data(size / sizeof(uint32_t));

            fread(data.data(), sizeof(uint32_t), data.size(), f);
            fclose(f);

            VkShaderModuleCreateInfo shaderModuleCI = {};
            {
                shaderModuleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                shaderModuleCI.codeSize = data.size() * sizeof(uint32_t);
                shaderModuleCI.pCode = data.data();

                VK_CHECK_RESULT(vkCreateShaderModule(VulkanContext::GetDevice().GetLogicalDevice(), &shaderModuleCI, nullptr, &shaderModule));
            }

            Reflect(data, type);
        }

        return shaderModule;
    }

    void VulkanShader::Reflect(const std::vector<uint32_t>& data, ShaderType shaderType)
    {
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

        for (const auto& res : resources.push_constant_buffers)
        {
            auto& type = compiler.get_type(res.base_type_id);
            VkPushConstantRange range = {};
            {
                range.offset = 0;
                range.size = compiler.get_declared_struct_size(type);
                range.stageFlags = GetVkShaderStage(shaderType);
            }

            m_VkPushConstantRanges.emplace_back(range);
        }

        int32_t sampler = 0;
        for (const auto& res : resources.sampled_images)
        {
            auto& type = compiler.get_type(res.base_type_id);

            UniformResource resBuffer = {};
            {
                resBuffer.BindingPoint = compiler.get_decoration(res.id, spv::DecorationBinding);
                resBuffer.Location = compiler.get_decoration(res.id, spv::DecorationLocation);
                resBuffer.Dimension = compiler.get_type(res.type_id).image.dim;
                resBuffer.StageFlags = GetVkShaderStage(shaderType);
                resBuffer.Sampler = sampler;
                resBuffer.ArraySize = compiler.get_type(res.type_id).array[0];

            }

            int32_t samplers[Renderer2D::MaxTextureSlot];
            for (uint32_t i = 0; i < Renderer2D::MaxTextureSlot; i++)
            {
                samplers[i] = i;
            }

            m_UniformResources[compiler.get_decoration(res.id, spv::DecorationBinding)] = std::move(resBuffer);
            sampler++;
        }
    }

    bool VulkanShader::LoadOrCompile(const shaderc::Compiler& compiler, const shaderc::CompileOptions& options, const std::string& filePath,
        shaderc_shader_kind shaderType, bool usePrecompiledBinaries, std::unordered_map<ShaderType, std::vector<uint32_t>>& out_binaryData)
    {
        if (filePath.empty())
            return false;

        ShaderType type = GetShaderType(shaderType);
        m_FilePaths[type] = filePath;

        if (usePrecompiledBinaries)
        {
            if (std::filesystem::exists(AssetManager::GetCachedPath(filePath, CachedPathType::Shader)))
            {
                VkShaderModule shaderModule = LoadSPIRVBinaries(filePath, type);
                if (shaderModule != nullptr)
                {

                    VkPipelineShaderStageCreateInfo pipelineShaderStageCI = {};
                    {
                        pipelineShaderStageCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                        pipelineShaderStageCI.stage = GetVkShaderStage(type);
                        pipelineShaderStageCI.pName = "main";
                        pipelineShaderStageCI.module = shaderModule;


                        assert(pipelineShaderStageCI.module != VK_NULL_HANDLE);
                    }

                    m_PipelineShaderStages.emplace_back(pipelineShaderStageCI);
                    m_ShaderModules[type] = shaderModule;

                    return true;
                }
            }

            goto COMPILE;

        }
        else
        {
            COMPILE:
            const std::string& load_result = LoadShaderSource(filePath);
            const shaderc::SpvCompilationResult result = CompileToSPIRV(compiler, options, load_result, shaderType, filePath);
            if (result.GetCompilationStatus() == shaderc_compilation_status_success)
            {
                std::vector<uint32_t> data(result.cbegin(), result.cend());

                VkShaderModule shaderModule = nullptr;
                VkShaderModuleCreateInfo shaderModuleCI = {};
                {
                    shaderModuleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                    shaderModuleCI.codeSize = data.size() * sizeof(uint32_t);
                    shaderModuleCI.pCode = data.data();

                    VK_CHECK_RESULT(vkCreateShaderModule(VulkanContext::GetDevice().GetLogicalDevice(), &shaderModuleCI, nullptr, &shaderModule));
                }

                VkPipelineShaderStageCreateInfo pipelineShaderStageCI = {};
                {
                    pipelineShaderStageCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                    pipelineShaderStageCI.stage = GetVkShaderStage(type);
                    pipelineShaderStageCI.pName = "main";
                    pipelineShaderStageCI.module = shaderModule;


                    assert(pipelineShaderStageCI.module != VK_NULL_HANDLE);
                }

                m_PipelineShaderStages.emplace_back(pipelineShaderStageCI);
                m_ShaderModules[type] = shaderModule;

                Reflect(data, type);
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

    void VulkanShader::DeleteShaderModules()
    {
        const auto& device = VulkanContext::GetDevice().GetLogicalDevice();
        for (auto& [key, module] : m_ShaderModules)
        {
            vkDestroyShaderModule(device, module, nullptr);
        }
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
}