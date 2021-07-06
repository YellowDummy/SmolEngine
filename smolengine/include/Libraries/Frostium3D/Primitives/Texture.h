#pragma once

#ifdef FROSTIUM_OPENGL_IMPL
#include "OpenGL/OpenglTexture.h"
#else
#include "Vulkan/VulkanTexture.h"
#endif

#include "Common/Common.h"

#include <glm/glm.hpp>
#include <string>
#include <cereal/cereal.hpp>

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	enum class TextureFormat : int
	{
		R8_UNORM,
		R8G8B8A8_UNORM,
		B8G8R8A8_UNORM,
		// HDR
		R16G16B16A16_SFLOAT,
		R32G32B32A32_SFLOAT
	};

	enum class AddressMode: int
	{
		REPEAT,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER,
		MIRROR_CLAMP_TO_EDGE,
	};

	enum class BorderColor : int
	{
		FLOAT_TRANSPARENT_BLACK,
		INT_TRANSPARENT_BLACK,
		FLOAT_OPAQUE_BLACK,
		INT_OPAQUE_BLACK,
		FLOAT_OPAQUE_WHITE,
		INT_OPAQUE_WHITE
	};

	struct TextureCreateInfo
	{
		bool           bVerticalFlip = true;
		bool           bAnisotropyEnable = true;
		bool           bImGUIHandle = false;
		TextureFormat  eFormat = TextureFormat::R8G8B8A8_UNORM;
		AddressMode    eAddressMode = AddressMode::REPEAT;
		ImageFilter    eFilter = ImageFilter::LINEAR;
		BorderColor    eBorderColor = BorderColor::FLOAT_OPAQUE_WHITE;
		std::string    FilePath = "";

	public:
		bool           Save(const std::string& filePath);
		bool           Load(const std::string& filePath);

	private:
		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(bVerticalFlip, bAnisotropyEnable, bImGUIHandle, eFormat, eAddressMode, eFilter, eBorderColor, FilePath);
		}
	};

	struct TextureInfo
	{
		uint32_t ID = 0;
		uint32_t Width = 0;
		uint32_t Height = 0;
		void*    ImHandle = nullptr; // imgui texture id
	};

#ifdef  FROSTIUM_OPENGL_IMPL
	class Texture: OpenglTexture2D
#else
	class Texture: VulkanTexture
#endif
	{
	public:
		Texture();
		~Texture() = default;

		void                Bind(uint32_t slot = 0) const;
		void                UnBind() const;
		bool                IsReady() const; 
		const TextureInfo&  GetInfo() const;
		void*               GetImGuiTexture() const;
#ifndef  FROSTIUM_OPENGL_IMPL
		VulkanTexture*      GetVulkanTexture();
#endif
		// Factory
		static void         CreateWhiteTexture(Texture* out_texture);
		static void         Create(const TextureCreateInfo* ifno, Texture* out_texture);
		static void         Create(const void* data, uint32_t size, uint32_t width, uint32_t height, 
			                Texture* out_texture, TextureFormat format = TextureFormat::R8G8B8A8_UNORM);

	private:
		TextureInfo         m_Info = {};
	};
}
