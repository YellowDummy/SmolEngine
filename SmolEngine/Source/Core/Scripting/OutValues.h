#pragma once

#include <string>
#include <cereal/cereal.hpp>
#include <cereal/types/variant.hpp>

namespace SmolEngine
{
	enum class OutValueType: uint16_t
	{
		Float, Int, String
	};

	struct OutValue
	{
		OutValue() = default;

		OutValue(const std::string& key, std::variant<float, int, std::string> val, OutValueType type)
			: Key(key), Value(val), Type(type) {}

		std::variant<float, int, std::string> Value;

		std::string Key;
		OutValueType Type;

	private:
		friend class cereal::access;
		friend class EditorLayer;
		friend class Scene;

		char stringBuffer[128] = "";

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Key, Value, Type);
		}
	};

}