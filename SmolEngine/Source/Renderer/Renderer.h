#pragma once
#include "Core/Core.h"
#include "Core/AssetManager.h"
#include "ECS/Components/MaterialComponent.h"

#include <glm/glm.hpp>
#include <memory>

namespace SmolEngine
{
	class Mesh;

	class Renderer
	{
	public:

		/// Init

		static void Init();

		static void Shutdown();

		/// Main

		static void BeginScene(const glm::mat4& viewProj);

		static void EndScene();

		/// Submit

		static void SubmitMesh(const glm::vec3& pos, const glm::vec3& rotation,
			const glm::vec3& scale, const Ref<Mesh>& mesh, const MaterialComponent& material);

		static void SubmitCube(const glm::vec3& pos, const glm::vec3& rotation,
			const glm::vec3& scale, const MaterialComponent& material);

		static void SubmitSphere(const glm::vec3& pos, const glm::vec3& rotation,
			const glm::vec3& scale, const MaterialComponent& material);

		static void SubmitPlane(const glm::vec3& pos, const glm::vec3& rotation,
			const glm::vec3& scale, const MaterialComponent& material);

		static void SubmitCapsule(const glm::vec3& pos, const glm::vec3& rotation,
			const glm::vec3& scale, const MaterialComponent& material);

		/// Events

		static void OnNewLevelLoaded();

	private:

		/// Helpres

		static void InitPBR();

		static void InitMainPipeline();

		static void InitSkyBoxPipeline();
	};
}
