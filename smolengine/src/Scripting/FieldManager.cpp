#include "stdafx.h"
#include "Scripting/FieldManager.h"

namespace SmolEngine
{
	bool FieldManager::AreEqual(FieldManager* another)
	{
		Finilize();

		size_t my_hash = GetHash();
		size_t another_hash = another->GetHash();

		return my_hash == another_hash;
	}

	void FieldManager::Finilize()
	{
		if (m_RefData.size() > 0)
			m_RefData.clear();

		m_RefData.resize(m_Ints.size() + m_Floats.size() + m_Strings.size());

		uint32_t index = 0;
		for (auto& i : m_Ints)
		{
			m_RefData[index].ptr = &i.second;
			m_RefData[index].type = FiledDataFlags::Int32;
			m_RefData[index].name = i.first;
			index++;
		}

		for (auto& i : m_Floats)
		{
			m_RefData[index].ptr = &i.second;
			m_RefData[index].type = FiledDataFlags::Float;
			m_RefData[index].name = i.first;
			index++;
		}

		for (auto& i : m_Strings)
		{
			m_RefData[index].ptr = &i.second;
			m_RefData[index].type = FiledDataFlags::String;
			m_RefData[index].name = i.first;
			index++;
		}
	}

	size_t FieldManager::GetHash() const
	{
		size_t id = 0;
		std::hash<std::string> hasher{};

		for (auto& filed : m_RefData)
		{
			id += hasher(filed.name);
		}


		return id;
	}

	uint32_t FieldManager::GetCount() const
	{
		return static_cast<uint32_t>(m_Ints.size() + m_Floats.size() + m_Strings.size());
	}

	const std::vector<FieldView>& FieldManager::GetFields() const
	{
		return m_RefData;
	}
}