#pragma once

#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"

#include <string>
#include <vector>

#include <meta/meta.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{
	class Actor;
	struct BehaviourComponent: public BaseComponent
	{
		BehaviourComponent() = default;
		BehaviourComponent(uint32_t id) 
			:BaseComponent(id) {}

		struct OutData
		{
			struct IntBuffer
			{
				int32_t Value;
				std::string Name;

				template<typename Archive>
				void serialize(Archive& archive)
				{
					archive(Value, Name);
				}
			};

			struct FloatBuffer
			{
				float Value;
				std::string Name;

				template<typename Archive>
				void serialize(Archive& archive)
				{
					archive(Value, Name);
				}
			};

			struct StringBuffer
			{
				std::string Value;
				std::string Name;

				template<typename Archive>
				void serialize(Archive& archive)
				{
					archive(Value, Name);
				}
			};

			int32_t                   ScriptID = 0;
			std::vector<IntBuffer>    Ints;
			std::vector<FloatBuffer>  Floats;
			std::vector<StringBuffer> Strings;

			template<typename Archive>
			void serialize(Archive& archive)
			{
				archive(ScriptID, Ints, Floats, Strings);
			}
		};

		struct ScriptInstance
		{										   
			std::string                            KeyName = "";
			meta::any                              Script;

			template<typename Archive>
			void serialize(Archive& archive)
			{
				archive(KeyName);
			}
		};		

		uint32_t                                   ActorID = 0;
		Actor*                                     Actor = nullptr;
		std::vector<ScriptInstance>                Scripts;
		std::unordered_map<std::string, OutData>   OutValues;			   

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class WorldAdmin;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(ActorID, Scripts, OutValues, ComponentID);
		}
	};
}