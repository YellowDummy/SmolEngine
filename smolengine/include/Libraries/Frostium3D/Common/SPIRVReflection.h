#pragma once
#include "Common/Core.h"
#include "Common/Common.h"

#include <string>
#include <vector>
#include <unordered_map>

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	enum class BufferType : uint16_t
	{
		Uniform,
		Storage
	};

	enum class ElementType : uint16_t
	{

	};

	struct Uniform
	{
		//ElementType                      Type;
		size_t                           Size = 0;
		size_t                           Offset = 0;
		std::string                      Name = "";
	};

	struct PushContantData
	{
		uint32_t                         Offset = 0;
		uint32_t                         Size = 0;
		ShaderType                       Stage = ShaderType::Vertex;
	};

	struct ShaderBuffer
	{
		ShaderType                       Stage = ShaderType::Vertex;
		BufferType                       Type = BufferType::Uniform;
		uint32_t                         BindingPoint = 0;
		std::string                      Name = "";
		std::string                      ObjectName = "";
		size_t                           Size = 0;
		std::vector<Uniform>             Uniforms;

	};

	struct UniformResource
	{
		ShaderType                       Stage;
		uint32_t                         Location = 0;
		uint32_t                         BindingPoint = 0;
		uint32_t                         Dimension = 0;
		uint32_t                         ArraySize = 0;
	};

	struct ReflectionData
	{
		PushContantData                                PushConstant{};
		std::unordered_map<uint32_t, UniformResource>  Resources;
		std::unordered_map<uint32_t, ShaderBuffer>     Buffers;

		void Clean()
		{
			Resources.clear();
			Buffers.clear();
			PushConstant = {};
		}
	};

}