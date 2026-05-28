#include "prspch.h"
#include "ScriptGlue.h"
#include "ScriptEngine.h"

#include "Pressure/Core/Input.h"
#include "Pressure/Core/KeyCodes.h"
#include "Pressure/Core/UUID.h"
#include "Pressure/Physics/Physics2D.h"
#include "Pressure/Scene/Scene.h"

#include <glm/glm.hpp>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

#include <box2d/box2d.h>

namespace Pressure
{

#define PRS_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Pressure.InternalCalls::" #Name, Name)

	namespace
	{

		std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

		MonoObject* GetScriptInstance(UUID entityId)
		{
			return ScriptEngine::GetManagedInstance(entityId);
		}

		bool Entity_HasComponent(const UUID entityId, MonoReflectionType* componentType)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(entity);

			MonoType* managedType = mono_reflection_type_get_type(componentType);
			PRS_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());

			return s_EntityHasComponentFuncs.at(managedType)(entity);
		}

		uint64_t Entity_FindEntityByName(MonoString* name)
		{
			char* nameCStr = mono_string_to_utf8(name);

			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->FindEntityByName(nameCStr);
			mono_free(nameCStr);

			if (!entity)
				return 0;

			return entity.GetUUID();
		}

		void TransformComponent_GetTranslation(const UUID entityId, glm::vec3* outTranslation)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(entity);

			*outTranslation = entity.GetComponent<TransformComponent>().Translation;
		}

		void TransformComponent_SetTranslation(const UUID entityId, const glm::vec3* translation)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(entity);

			entity.GetComponent<TransformComponent>().Translation = *translation;
		}

		void RigidBody2DComponent_ApplyLinearImpulse(const UUID entityId, const glm::vec2* impulse, const glm::vec2* point, bool wake)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(entity);

			b2Vec2 imp = { impulse->x, impulse->y };
			b2Vec2 pt = { point->x, point->y };
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2Body_ApplyLinearImpulse(rb2d.RuntimeBody->BodyId, imp, pt, wake);
		}

		void RigidBody2DComponent_ApplyLinearImpulseToCenter(const UUID entityId, const glm::vec2* impulse, const bool wake)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(entity);

			b2Vec2 imp = { impulse->x, impulse->y };
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2Body_ApplyLinearImpulseToCenter(rb2d.RuntimeBody->BodyId, imp, wake);
		}

		bool Input_IsKeyDown(const KeyCode keyCode)
		{
			return Input::IsKeyPressed(keyCode);
		}

	}

	namespace
	{

		template<typename... Component>
		void RegisterComponent()
		{
			([]
			{
				const std::string_view typeName = typeid(Component).name();
				const size_t pos = typeName.find_last_of(':');
				std::string_view structName = typeName.substr(pos + 1);
				std::string managedTypeName = fmt::format("Pressure.{}", structName);

				MonoType* managedType = mono_reflection_type_from_name(managedTypeName.data(), ScriptEngine::GetCoreAssemblyImage());
				if (!managedType)
				{
					PRS_CORE_ERROR("Failed to get managed type for component: {}", typeName);
					return;
				}

				s_EntityHasComponentFuncs[managedType] = [](const Entity entity) { return entity.HasComponent<Component>(); };
			}(), ...);
		}

		template<typename... Component>
		void RegisterComponent(ComponentGroup<Component...>)
		{
			RegisterComponent<Component...>();
		}

	}

	void ScriptGlue::RegisterComponents()
	{
		s_EntityHasComponentFuncs.clear();
		RegisterComponent(AllComponents{});
	}

	void ScriptGlue::RegisterFunctions()
	{
		PRS_ADD_INTERNAL_CALL(GetScriptInstance);

		PRS_ADD_INTERNAL_CALL(Entity_HasComponent);
		PRS_ADD_INTERNAL_CALL(Entity_FindEntityByName);

		PRS_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		PRS_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

		PRS_ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulse);
		PRS_ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulseToCenter);

		PRS_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}
}
