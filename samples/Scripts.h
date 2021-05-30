#include "Core/Core.h"
#include "SmolEngineCore.h"

using namespace SmolEngine;

class BasePlayerScript : public BehaviourPrimitive
{
public:

	BasePlayerScript();

	void OnProcess(float deltaTime);

	void OnBegin();
	void OnDestroy();
	void OnCollisionContact();
	void OnCollisionExit();

private:

	float* m_Speed = nullptr;
	float* m_CameraSpeed = nullptr;

	float      m_Yaw = 0;
	float      m_Pitch = 0.0f;
	glm::vec2  m_InitialMousePosition = glm::vec2(0.0f);
};