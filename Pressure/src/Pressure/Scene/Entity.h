#pragma once

#include "Pressure/Core/UUID.h"
#include "Pressure/Scene/Scene.h"

#include <entt.hpp>

namespace Pressure
{
	struct AABB;

    class Entity
    {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene);
        Entity(const Entity& other) = default;
        
        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            PRS_CORE_ASSERT(!HasComponent<T>(), "Entity already has component !");
			m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			if constexpr (std::is_empty_v<T>)
			{
				static T s_EmptyComponent{};
				m_Scene->OnComponentAdded<T>(*this, s_EmptyComponent);
				return s_EmptyComponent;
			}
			else
			{
				T& component = m_Scene->m_Registry.get<T>(m_EntityHandle);
				m_Scene->OnComponentAdded<T>(*this, component);
				return component;
			}
        }

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&& ...args)
        {
	        T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
        	m_Scene->OnComponentAdded<T>(*this, component);
			return component;
        }

		template<typename T>
    	void OnComponentAddedOrReplaced(Entity src, T& srcComponent);

        template<typename T>
        T& GetComponent()
        {
            PRS_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }

		template<typename T>
		const T& GetComponent() const
		{
			PRS_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

        template<typename... T>
        bool HasComponent() const
        {
            return (m_Scene->m_Registry.any_of<T...>(m_EntityHandle));
        }

        template<typename T>
        void RemoveComponent()
        {
            PRS_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            m_Scene->m_Registry.remove<T>(m_EntityHandle);
        }

		void SetEnabled(const bool enabled) const;

        bool IsEnabled() const;

        operator bool() const { 
        	return m_EntityHandle != entt::null
        		&& m_Scene != nullptr
        		&& m_Scene->m_Registry.valid(m_EntityHandle); 
        }
		operator uint32_t() const { return static_cast<uint32_t>(m_EntityHandle); }
		operator entt::entity() const { return m_EntityHandle; }

		UUID GetUUID();
        const std::string& GetName();

		glm::vec3 GetCenter() const;
		float GetHalfExtent() const;

        bool operator ==(const Entity& other) const 
		{ 
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator !=(const Entity& other) const
		{
			return !(*this == other);
		}
    private:
        entt::entity m_EntityHandle{ entt::null };
        Scene* m_Scene{ nullptr };
    };
}
