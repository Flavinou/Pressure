#pragma once

#include "Pressure/Core/Timestep.h"
#include "Pressure/Core/UUID.h"
#include "Pressure/Renderer/EditorCamera.h"

#include <entt.hpp>

namespace Pressure
{

    class Entity;

	// Forward declaration of Box2D world id struct
	struct PhysicsWorldImpl;

    class Scene
    {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string& name = std::string());
        Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		Entity GetEntityByUUID(UUID uuid);
		void DuplicateEntity(Entity entity);
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnSimulationStart();
		void OnSimulationStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateSimulation(Timestep ts, EditorCamera& camera);
        void OnUpdateEditor(Timestep ts, EditorCamera& camera);
        void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();

		bool IsRunning() const { return m_IsRunning; }

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

        static Ref<Scene> Copy(Ref<Scene> other);

    private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		void OnPhysics2DStart();
		void OnPhysics2DStop();

		void RenderScene(EditorCamera& camera);
    private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		bool m_IsRunning = false;
		PhysicsWorldImpl* m_PhysicsImpl;

		std::unordered_map<UUID, entt::entity> m_EntityMap;

        friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
    };

}
