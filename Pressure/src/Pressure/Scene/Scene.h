#pragma once

#include "Pressure/Containers/QuadTree.h"
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
		Entity FindEntityByName(std::string_view name);
		Entity DuplicateEntity(Entity entity);

		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnSimulationStart();
		void OnSimulationStop();

		void Step(int frames = 1);

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateSimulation(Timestep ts, EditorCamera& camera);
        void OnUpdateEditor(Timestep ts, EditorCamera& camera);
        void OnViewportResize(uint32_t width, uint32_t height);

		void OnCreateEntityRuntime(Entity entity) const;

		void InstantiatePhysicsBody(Entity entity) const;

		Entity GetPrimaryCameraEntity();

    	glm::vec2 GetViewportSize() const { return { m_ViewportWidth, m_ViewportHeight }; }
		glm::vec3 ScreenToWorldPosition(const glm::vec3& screenPosition);

		QuadTree<Entity>& GetEntityQuadTree() const { return *m_EntityQuadTree; }

		bool IsRunning() const { return m_IsRunning; }
    	bool IsPaused() const { return m_IsPaused; }
		void SetPaused(bool paused) { m_IsPaused = paused; }

		bool* IsBox2DPhysicsSimulationEnabled() const { return const_cast<bool*>(&m_Box2DPhysicsSimulationEnabled); }
		void SetBox2DPhysicsSimulationEnabled(const bool value) { m_Box2DPhysicsSimulationEnabled = value; }

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
		void OnPhysics2DUpdate(Timestep ts);
		void OnPhysics2DStop();

		static bool CheckCollision(Entity entity, Entity otherEntity);

		void OnScriptEngineUpdate(Timestep ts);

		void RenderScene(EditorCamera& camera);

		void DestroyEntityImmediate(Entity entity);

    private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		bool m_IsRunning = false;
		bool m_IsPaused = false;
		int m_StepFrames = 0;

		// Box2D Physics
		PhysicsWorldImpl* m_PhysicsImpl;
		bool m_Box2DPhysicsSimulationEnabled = false;

		std::unordered_map<UUID, entt::entity> m_EntityMap;

		// Runtime
		std::vector<std::pair<UUID, UUID>> m_PendingCollisions;
		std::vector<entt::entity> m_PendingDestroyEntities;

		// Spatial partitioning for collision detection
		Scope<QuadTree<Entity>> m_EntityQuadTree;

        friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
    };

}
