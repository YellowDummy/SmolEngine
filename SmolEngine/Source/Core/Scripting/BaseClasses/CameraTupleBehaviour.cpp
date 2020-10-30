#include "stdafx.h"
#include "CameraTupleBehaviour.h"

namespace SmolEngine
{
	CameraBaseTuple& CameraTupleBehaviour::GetCameraBaseTuple()
	{
		return *WorldAdmin::GetScene()->GetTuple<CameraBaseTuple>(*m_Actor);
	}
}