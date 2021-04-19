#pragma once

#include "Common/Core.h"

#include <string>

namespace Frostium
{
	enum class DataTypes : uint16_t
	{
		None = 0,

		Float, Float2, Float3, Float4,

		Mat3, Mat4,

		Int, Int2, Int3, Int4,

		Bool
	};

	static uint32_t ShaderDataTypeSize(DataTypes type)
	{
		switch (type)
		{
		case DataTypes::None:     return 0;           break;
		case DataTypes::Float:    return 4;           break;
		case DataTypes::Float2:   return 4 * 2;       break;
		case DataTypes::Float3:   return 4 * 3;       break;
		case DataTypes::Float4:   return 4 * 4;       break;
		case DataTypes::Mat3:     return 4 * 3 * 3;   break;
		case DataTypes::Mat4:     return 4 * 4 * 4;   break;
		case DataTypes::Int:      return 4;           break;
		case DataTypes::Int2:     return 4 * 2;       break;
		case DataTypes::Int3:     return 4 * 3;       break;
		case DataTypes::Int4:     return 4 * 4;       break;
		case DataTypes::Bool:     return 1;           break;

		default:                  return 0;
		}
	}

	struct BufferElement
	{
		BufferElement(DataTypes _type, const std::string& _name, bool normalized = false)
			:name(_name), type(_type), size(ShaderDataTypeSize(type)), offset(0), Normalized(normalized) {}

		uint32_t GetComponentCount() const
		{
			switch (type)
			{
			case DataTypes::Float:    return 1;           break;
			case DataTypes::Float2:   return 2;           break;
			case DataTypes::Float3:   return 3;           break;
			case DataTypes::Float4:   return 4;           break;
			case DataTypes::Mat3:     return 3 * 3;       break;
			case DataTypes::Mat4:     return 4 * 4;       break;
			case DataTypes::Int:      return 1;           break;
			case DataTypes::Int2:     return 2;           break;
			case DataTypes::Int3:     return 3;           break;
			case DataTypes::Int4:     return 4;           break;
			case DataTypes::Bool:     return 1;           break;

			default:                     return 0;
			}
		}

		std::string          name;
		DataTypes            type;
		uint32_t             offset, size;
		bool                 Normalized;
	};

}