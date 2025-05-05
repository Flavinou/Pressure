#include "SceneHierarchyPanel.h"
#include "Pressure/Scene/Components.h"

#include <imgui/imgui.h>
#include <entt.hpp>

namespace Pressure
{

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		m_Context->m_Registry.view<entt::entity>().each([&](auto entityID)
		{
			Entity entity{ entityID, m_Context.get() };

			DrawEntityNode(entity);
		});

		ImGui::End();

		ImGui::ShowDemoWindow();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags node_flags = 
			((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) 
			| ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, node_flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity; 
		}
		 
		if (opened)
		{
			{
				// TEST: Nested tree node
				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow;
				bool opened = ImGui::TreeNodeEx((void*)9817239, node_flags, tag.c_str());
				if (opened)
					ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}

}
