#pragma once
#include "Common/Core.h"

#include "Utils/GLM.h"

namespace Frostium
{
	enum class CachedPathType
	{
		Shader,
		Pipeline
	};

	class Utils
	{
	public:

		// Transform
		static bool DecomposeTransform(const glm::mat4& transform, glm::vec3& out_translation, 
			glm::vec3& out_rotation, glm::vec3& out_scale);
		static bool ComposeTransform(const glm::vec3& translation, const glm::vec3& rotation
			, const glm::vec3& scale, bool is3D, glm::mat4& out_transform);

		// Raycating
		static glm::vec3 ScreenToWorld(const glm::vec2& mousePos, float width,
			float height, const glm::mat4& viewProj);
		static glm::vec3 CastRay(const glm::vec3& startPos, const glm::vec2& mousePos, float width,
			float height, float distance, const glm::mat4& viewProj);
		static glm::vec3 CastRay(const glm::vec3& startPos, float distance, const glm::mat4& viewProj);

		// Helpers
		static bool IsPathValid(const std::string& path);
		static std::string GetCachedPath(const std::string& filePath, CachedPathType type);
	};
}