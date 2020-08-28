#pragma once

#include "Core/Core.h"
#include <vector>
#include <string>
#include <memory>

namespace SmolEngine
{
	enum class ShaderDataType
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
		std::string name;
		ShaderDataType type;
		uint32_t offset, size;
		bool Normalized;

		BufferElement(ShaderDataType _type, const std::string& _name, bool normalized = false)
			:name(_name), type(_type), size(ShaderDataTypeSize(type)), offset(0), Normalized(normalized)
		{

		}

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
	};

	class BufferLayout
	{
	public:
		BufferLayout(){}
		BufferLayout(const std::initializer_list<BufferElement>& elemets)
			:m_Elements(elemets)
		{
			CalculateOffsetAndPride();
		}

		inline uint32_t GetStride() const { return m_Stride; }
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }

		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:

		void CalculateOffsetAndPride()
		{
			uint32_t offset = 0;
			m_Stride = 0;

			for (auto& element : m_Elements)
			{
				element.offset = offset;
				offset += element.size;
				m_Stride += element.size;
			}
		}

	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {};
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		virtual void UploadData(const void* data, const uint32_t size, const uint32_t offset = 0) const = 0;
		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		static Ref<VertexBuffer> Create(uint32_t size);
		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {};
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};

	class VertexArray
	{
	public:
		~VertexArray() {}
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void SetVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;
		virtual Ref<IndexBuffer> GetIndexBuffer() const = 0;

		static Ref<VertexArray> Create();
	};
}