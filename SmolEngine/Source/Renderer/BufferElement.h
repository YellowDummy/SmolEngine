#pragma once

#include "Core/Core.h"

#include <string>

namespace SmolEngine
{
	enum class ShaderDataType : uint16_t
	{
		None = 0,

		Float, Float2, Float3, Float4,

		Mat3, Mat4,

		Int, Int2, Int3, Int4,

		Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::None:
			return 0;           break;
		case ShaderDataType::Float:    return 4;           break;
		case ShaderDataType::Float2:   return 4 * 2;       break;
		case ShaderDataType::Float3:   return 4 * 3;       break;
		case ShaderDataType::Float4:   return 4 * 4;       break;
		case ShaderDataType::Mat3:     return 4 * 3 * 3;   break;
		case ShaderDataType::Mat4:     return 4 * 4 * 4;   break;
		case ShaderDataType::Int:      return 4;           break;
		case ShaderDataType::Int2:     return 4 * 2;       break;
		case ShaderDataType::Int3:     return 4 * 3;       break;
		case ShaderDataType::Int4:     return 4 * 4;       break;
		case ShaderDataType::Bool:     return 1;           break;

		default:                     return 0;
		}
	}

	struct BufferElement
	{
		BufferElement(ShaderDataType _type, const std::string& _name, bool normalized = false)
			:name(_name), type(_type), size(ShaderDataTypeSize(type)), offset(0), Normalized(normalized) {}

		uint32_t GetComponentCount() const
		{
			switch (type)
			{
			case ShaderDataType::Float:    return 1;           break;
			case ShaderDataType::Float2:   return 2;           break;
			case ShaderDataType::Float3:   return 3;           break;
			case ShaderDataType::Float4:   return 4;           break;
			case ShaderDataType::Mat3:     return 3 * 3;       break;
			case ShaderDataType::Mat4:     return 4 * 4;       break;
			case ShaderDataType::Int:      return 1;           break;
			case ShaderDataType::Int2:     return 2;           break;
			case ShaderDataType::Int3:     return 3;           break;
			case ShaderDataType::Int4:     return 4;           break;
			case ShaderDataType::Bool:     return 1;           break;

			default:                     return 0;
			}
		}

		std::string name;
		ShaderDataType type;
		uint32_t offset, size;
		bool Normalized;
	};

}