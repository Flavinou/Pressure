#include "prspch.h"
#include "Entity.h"

#include "Pressure/Containers/QuadTree.h"
#include "Pressure/Scene/Components.h"
#include "Pressure/Scripting/ScriptEngine.h"

namespace Pressure
{
    Entity::Entity(entt::entity handle, Scene* scene)
        : m_EntityHandle(handle), m_Scene(scene)
    {
    }

    void Entity::SetEnabled(const bool enabled) const
    {
	    if (enabled && HasComponent<DisabledComponent>())
	    {
		    m_Scene->m_Registry.remove<DisabledComponent>(m_EntityHandle);
	    }
	    else if (!enabled && !HasComponent<DisabledComponent>())
	    {
		    m_Scene->m_Registry.emplace<DisabledComponent>(m_EntityHandle);
	    }
    }

    bool Entity::IsEnabled() const
    {
	    return !HasComponent<DisabledComponent>();
    }

    UUID Entity::GetUUID()
    {
	    return GetComponent<IDComponent>().ID;
    }

    const std::string& Entity::GetName()
    {
	    return GetComponent<TagComponent>().Tag;
    }

    glm::vec3 Entity::GetCenter() const
    {
		auto& tc = GetComponent<TransformComponent>();

		if (HasComponent<CircleCollider2DComponent>())
		{
			auto& circleCollider = GetComponent<CircleCollider2DComponent>();
			return tc.Translation + glm::vec3(circleCollider.Offset, 0.0f);
		}

		if (HasComponent<BoxCollider2DComponent>())
		{
			auto& boxCollider = GetComponent<BoxCollider2DComponent>();
			return tc.Translation + glm::vec3(boxCollider.Offset, 0.0f);
		}

		return tc.Translation;
    }

    float Entity::GetHalfExtent() const
    {
		auto& tc = GetComponent<TransformComponent>();

		if (HasComponent<CircleCollider2DComponent>())
		{
			auto& circleCollider = GetComponent<CircleCollider2DComponent>();
			return circleCollider.Radius * std::max(tc.Scale.x, tc.Scale.y);
		}

		if (HasComponent<BoxCollider2DComponent>())
		{
			auto& boxCollider = GetComponent<BoxCollider2DComponent>();
			return std::max(boxCollider.Size.x, boxCollider.Size.y) * 0.5f;
		}

		return std::max(tc.Scale.x, tc.Scale.y) * 0.5f;
    }

#pragma region OnComponentAddedOrReplaced overloads

	template<typename T>
	void Entity::OnComponentAddedOrReplaced(Entity src, T& component)
	{
		// static_assert(false, "Unknown component!");
	}

	template<>
	void Entity::OnComponentAddedOrReplaced<IDComponent>(Entity src, IDComponent& component)
	{
	}

	template<>
	void Entity::OnComponentAddedOrReplaced<TagComponent>(Entity src, TagComponent& component)
	{
	}

	template<>
	void Entity::OnComponentAddedOrReplaced<TransformComponent>(Entity src, TransformComponent& component)
	{
	}

	template<>
	void Entity::OnComponentAddedOrReplaced<CameraComponent>(Entity src, CameraComponent& component)
	{
	}

	template<>
	void Entity::OnComponentAddedOrReplaced<ScriptComponent>(Entity src, ScriptComponent& component)
	{
		// On entity duplication, field values must be copied from the source entity to the new entity, otherwise they will be reset to default values
		const auto& srcComponent = src.GetComponent<ScriptComponent>();

		Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(srcComponent.ClassName);
		const auto& fields = entityClass->GetFields();

		auto& srcEntityFields = ScriptEngine::GetScriptFieldMap(src);
		auto& dstEntityFields = ScriptEngine::GetScriptFieldMap(*this);
		for (const auto& [fieldName, field] : fields)
		{
			// Field has been set in editor
			if (srcEntityFields.find(fieldName) != srcEntityFields.end())
			{
				ScriptFieldInstance& srcFieldInstance = srcEntityFields[fieldName];
				ScriptFieldInstance& dstFieldInstance = dstEntityFields[fieldName];

				switch (field.Type)
				{
				case ScriptFieldType::Float:
					{
						float srcValue = srcFieldInstance.GetValue<float>();
						dstFieldInstance.SetValue<float>(srcValue);
						break;
					}
				case ScriptFieldType::Int:
					{
						int srcValue = srcFieldInstance.GetValue<int>();
						dstFieldInstance.SetValue<int>(srcValue);
						break;
					}
				default:
					break;
				}
			}
			else // Field has not been set in editor yet, force default value
			{
				switch (field.Type)
				{
				case ScriptFieldType::Float:
					{
						float data = 0.0f;
						ScriptFieldInstance& dstFieldInstance = dstEntityFields[fieldName];
						dstFieldInstance.SetField(field);
						dstFieldInstance.SetValue<float>(data);
						break;
					}
				case ScriptFieldType::Int:
					{
						int data = 0;
						ScriptFieldInstance& dstFieldInstance = dstEntityFields[fieldName];
						dstFieldInstance.SetField(field);
						dstFieldInstance.SetValue<int>(data);
						break;
					}
				default:
					break;
				}
			}
		}
	}

	template<>
	void Entity::OnComponentAddedOrReplaced<SpriteRendererComponent>(Entity src, SpriteRendererComponent& component)
	{
	}

	template<>
	void Entity::OnComponentAddedOrReplaced<CircleRendererComponent>(Entity src, CircleRendererComponent& component)
	{
	}

	template<>
	void Entity::OnComponentAddedOrReplaced<NativeScriptComponent>(Entity src, NativeScriptComponent& component)
	{
	}

	template<>
	void Entity::OnComponentAddedOrReplaced<RigidBody2DComponent>(Entity src, RigidBody2DComponent& component)
	{
	}

	template<>
	void Entity::OnComponentAddedOrReplaced<BoxCollider2DComponent>(Entity src, BoxCollider2DComponent& component)
	{
	}

	template<>
	void Entity::OnComponentAddedOrReplaced<CircleCollider2DComponent>(Entity src, CircleCollider2DComponent& component)
	{
	}

	template<>
	void Entity::OnComponentAddedOrReplaced<TextComponent>(Entity src, TextComponent& component)
	{
	}

	template<>
	void Entity::OnComponentAddedOrReplaced<DisabledComponent>(Entity src, DisabledComponent& component)
	{
	}

#pragma endregion

}
