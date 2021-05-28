#pragma once

#include <cfloat>
#include "Utils/GLM.h"

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	struct AnimationProperties
	{
		// Setters
		void SetSpeed(float speed) { Speed = speed; }
		void SetStart(float start) { Start = start; }
		void SetEnd(float end) { End = end; }
		void SetLooping(bool loop) { Loop = loop; }
		void SetActive(bool value) { Active = value; }

		// Getters
		bool IsLooping() const { return Loop; }
		bool IsActive() const { return Active; }

		float GetStart() const { return Start; }
		float GetEnd() const { return End; }
		float GetSpeed() const { return Speed; }

	private:

		bool                   Loop = true;
		bool                   Active = false;
			                   
		float                  Start = FLT_MAX;
		float                  End = FLT_MIN;
		float                  Speed = 1.0f;
		float                  CurrentTime = 0.0f;

		std::vector<glm::mat4> Joints;

		friend struct ImportedDataGlTF;
		friend class DeferredRenderer;
	};
}