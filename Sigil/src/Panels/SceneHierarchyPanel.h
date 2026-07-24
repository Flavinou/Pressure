#pragma once

#include "Pressure/Core/Base.h"
#include "Pressure/Scene/Entity.h"
#include "Pressure/Scene/Scene.h"

namespace Pressure
{

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);
	private:
		void OnEntityDestroyed(Entity entity);

		template<typename Component>
		void DisplayAddableComponent(const std::string& label);

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};
}
