#pragma once
#include "Core/Core.h"

#include <unordered_map>

#include <string>
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{
	enum class FiledDataFlags
	{
		Int32,
		Float,
		String,

		MaxEnum
	};

	struct FieldView
	{
		void*          ptr = nullptr;
		std::string    name = "";
		FiledDataFlags type = FiledDataFlags::MaxEnum;
	};

	class FieldManager
	{
	public:

		template<typename T>
		bool PushVariable(void* ptr, const std::string& name)
		{
			if (std::is_same<int32_t, T>::value)
			{
				if (m_Ints.find(name) == m_Ints.end())
				{
					m_Ints[name] = *(int32_t*)ptr;
					return true;
				}
			}

			if (std::is_same<float, T>::value)
			{
				if (m_Floats.find(name) == m_Floats.end())
				{
					m_Floats[name] = *(float*)ptr;
					return true;
				}
			}

			if (std::is_same<std::string, T>::value)
			{
				if (m_Strings.find(name) == m_Strings.end())
				{
					m_Strings[name] = *(std::string*)ptr;
					return true;
				}
			}

			return false;
		}

		template<typename T>
		Ref<FieldView> GetVarriable(const std::string& name)
		{
			if (std::is_same<int32_t, T>::value)
			{
				auto& it = m_Ints.find(name);
				if (it != m_Ints.end())
				{
					Ref<FieldView> view = std::make_shared<FieldView>();
					view->ptr = &it->second;
					view->type = FiledDataFlags::Int32;
					view->name = name;
					return view;
				}
			}

			if (std::is_same<float, T>::value)
			{
				auto& it = m_Floats.find(name);
				if (it != m_Floats.end())
				{
					Ref<FieldView> view = std::make_shared<FieldView>();
					view->ptr = &it->second;
					view->type = FiledDataFlags::Float;
					view->name = name;
					return view;
				}
			}

			if (std::is_same<std::string, T>::value)
			{
				auto& it = m_Strings.find(name);
				if (it != m_Strings.end())
				{
					Ref<FieldView> view = std::make_shared<FieldView>();
					view->ptr = &it->second;
					view->type = FiledDataFlags::String;
					view->name = name;
					return view;
				}
			}

			return nullptr;
		}

		bool                                 AreEqual(FieldManager* another);
		void                                 Finilize();
		size_t                               GetHash() const;
		uint32_t                             GetCount() const;
		const std::vector<FieldView>&        GetFields() const;

	private:

		std::unordered_map<std::string, int32_t>        m_Ints;
		std::unordered_map<std::string, float>          m_Floats;
		std::unordered_map<std::string, std::string>    m_Strings;
		std::vector<FieldView>                          m_RefData;

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_Ints, m_Floats, m_Strings);
		}
	};
}