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
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateRuntime(Timestep ts);
        void OnUpdateEditor(Timestep ts, EditorCamera& camera);
        void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
    private:
        entt::registry m_Registry;

        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		PhysicsWorldImpl* m_PhysicsImpl;

        friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
    };

}
