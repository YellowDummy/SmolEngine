#include "stdafx.h"
#include "VulkanRendererAPI.h"

namespace SmolEngine
{
	void VulkanRendererAPI::Init()
	{
		m_Instance.Init();

		m_Device.Init(&m_Instance);

		m_CommandPool.Init(&m_Device);
	}
}