#pragma once

#include "Pressure/Core/Base.h"
#include "Pressure/Core/Log.h"
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
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};

}
