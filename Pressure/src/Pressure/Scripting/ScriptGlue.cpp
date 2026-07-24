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

		void Entity_GetName(const UUID entityId, MonoString** outName)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(entity);
			*outName = ScriptEngine::NewString(entity.GetName().c_str());
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

		uint64_t Entity_Create(MonoString* tag)
		{
			char* tagCStr = mono_string_to_utf8(tag);

			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->CreateEntity(tagCStr);
			mono_free(tagCStr);

			return entity.GetUUID();
		}

		void Entity_Destroy(uint64_t entityId)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(entity);

			scene->DestroyEntity(entity);
		}

		uint64_t Entity_Duplicate(MonoString* tag)
		{
			char* tagCStr = mono_string_to_utf8(tag);

			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity source = scene->FindEntityByName(tagCStr);
			mono_free(tagCStr);

			if (!source)
				return 0;

			static uint32_t copyCount = 0;
			Entity entity = scene->DuplicateEntity(source);
			const std::string copyName = source.GetName() + "_" + std::to_string(++copyCount);
			entity.GetComponent<TagComponent>().Tag = copyName;
			ScriptEngine::CopyEntityScriptFields(source, entity);
			scene->OnCreateEntityRuntime(entity);
			return entity.GetUUID();
		}

		uint64_t Entity_DuplicateById(uint64_t entityId)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity source = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(source);

			static uint32_t copyCount = 0;
			Entity entity = scene->DuplicateEntity(source);
			const std::string copyName = source.GetName() + "_" + std::to_string(++copyCount);
			entity.GetComponent<TagComponent>().Tag = copyName;
			ScriptEngine::CopyEntityScriptFields(source, entity);
			scene->OnCreateEntityRuntime(entity);
			return entity.GetUUID();
		}

		void Entity_SetEnabled(const UUID entityId, const bool enabled)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(entity);

			if (enabled)
			{
				entity.RemoveComponent<DisabledComponent>();
			}
			else if (!entity.HasComponent<DisabledComponent>())
			{
				entity.AddComponent<DisabledComponent>();
			}
		}

		bool Entity_IsEnabled(const UUID entityId)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(entity);

			return !entity.HasComponent<DisabledComponent>();
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

			// Recreate the physics body if the entity has a RigidBody2DComponent
			if (entity.HasComponent<RigidBody2DComponent>())
			{
				scene->InstantiatePhysicsBody(entity);
			}
		}

		void TransformComponent_GetScale(const UUID entityId, glm::vec3* outScale)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(entity);

			*outScale = entity.GetComponent<TransformComponent>().Scale;
		}

		void TransformComponent_SetScale(const UUID entityId, const glm::vec3* scale)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(entity);

			entity.GetComponent<TransformComponent>().Scale = *scale;

			// Recreate the physics body if the entity has a RigidBody2DComponent
			if (entity.HasComponent<RigidBody2DComponent>())
			{
				scene->InstantiatePhysicsBody(entity);
			}
		}

		void RigidBody2DComponent_GetPosition(const UUID entityId, glm::vec2* outPosition)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(entity);

			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2Transform transform = b2Body_GetTransform(rb2d.RuntimeBody->BodyId);
			*outPosition = { transform.p.x, transform.p.y };
		}

		void RigidBody2DComponent_SetPosition(const UUID entityId, const glm::vec2* position)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(entity);

			b2Vec2 pos = { position->x, position->y };
			entity.GetComponent<TransformComponent>().Translation = { pos.x, pos.y, 0.0f };

			// Recreate the physics body instead of expensive set transform
			scene->InstantiatePhysicsBody(entity);
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

		void RigidBody2DComponent_GetLinearVelocity(const UUID entityId, glm::vec2* outVelocity)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(entity);

			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2BodyId body = rb2d.RuntimeBody->BodyId;
			b2Vec2 velocity = b2Body_GetLinearVelocity(body);
			*outVelocity = { velocity.x, velocity.y };
		}

		float RigidBody2DComponent_GetGravityScale(const UUID entityId)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(entity);

			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2BodyId body = rb2d.RuntimeBody->BodyId;
			return b2Body_GetGravityScale(body);
		}

		void RigidBody2DComponent_SetGravityScale(const UUID entityId, float gravityScale)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(entity);

			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2BodyId body = rb2d.RuntimeBody->BodyId;
			b2Body_SetGravityScale(body, gravityScale);
		}

		RigidBody2DComponent::BodyType RigidBody2DComponent_GetBodyType(const UUID entityId)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(entity);

			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2BodyId body = rb2d.RuntimeBody->BodyId;
			b2BodyType type = b2Body_GetType(body);
			
			return Utils::RigidBody2DTypeFromBox2DBody(type);
		}

		void RigidBody2DComponent_SetBodyType(const UUID entityId, RigidBody2DComponent::BodyType type)
		{
			Scene* scene = ScriptEngine::GetSceneContext();
			PRS_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByUUID(entityId);
			PRS_CORE_ASSERT(entity);

			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2BodyId body = rb2d.RuntimeBody->BodyId;
			b2BodyType box2dType = Utils::RigidBody2DTypeToBox2DBody(type);
			b2Body_SetType(body, box2dType);
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
					PRS_CORE_WARN("Failed to get managed type for component: {}", typeName);
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

		PRS_ADD_INTERNAL_CALL(Entity_GetName);
		PRS_ADD_INTERNAL_CALL(Entity_HasComponent);
		PRS_ADD_INTERNAL_CALL(Entity_FindEntityByName);
		PRS_ADD_INTERNAL_CALL(Entity_Create);
		PRS_ADD_INTERNAL_CALL(Entity_Destroy);
		PRS_ADD_INTERNAL_CALL(Entity_Duplicate);
		PRS_ADD_INTERNAL_CALL(Entity_DuplicateById);
		PRS_ADD_INTERNAL_CALL(Entity_SetEnabled);
		PRS_ADD_INTERNAL_CALL(Entity_IsEnabled);

		PRS_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		PRS_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
		PRS_ADD_INTERNAL_CALL(TransformComponent_GetScale);
		PRS_ADD_INTERNAL_CALL(TransformComponent_SetScale);

		PRS_ADD_INTERNAL_CALL(RigidBody2DComponent_GetPosition);
		PRS_ADD_INTERNAL_CALL(RigidBody2DComponent_SetPosition);
		PRS_ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulse);
		PRS_ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulseToCenter);
		PRS_ADD_INTERNAL_CALL(RigidBody2DComponent_GetLinearVelocity);
		PRS_ADD_INTERNAL_CALL(RigidBody2DComponent_GetGravityScale);
		PRS_ADD_INTERNAL_CALL(RigidBody2DComponent_SetGravityScale);
		PRS_ADD_INTERNAL_CALL(RigidBody2DComponent_GetBodyType);
		PRS_ADD_INTERNAL_CALL(RigidBody2DComponent_SetBodyType);

		PRS_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}
}
