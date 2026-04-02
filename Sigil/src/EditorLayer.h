#pragma once

#include "Pressure.h"
#include "Pressure/Renderer/EditorCamera.h"

#include "Panels/ContentBrowserPanel.h"
#include "Panels/SceneHierarchyPanel.h"

namespace Pressure
{

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		~EditorLayer() override = default;

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		void OnEvent(Event& e) override;
		void OnImGuiRender() override;

	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();

		void OnScenePlay();
		void OnSceneStop();

		void UI_Toolbar();

	private:
		OrthographicCameraController m_CameraController;

		// Temporary
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;
		Ref<Texture2D> m_VoronoiTexture;

		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity;
        Entity m_CameraEntity;		
		Entity m_SecondCameraEntity;

		Entity m_HoveredEntity;

		bool m_PrimaryCamera = true;

		EditorCamera m_EditorCamera;

		Ref<FrameBuffer> m_FrameBuffer;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
		glm::vec2 m_ViewportBounds[2];

		int m_GizmoType = -1;

		enum class SceneState
		{
			Edit = 0,
			Play = 1,
		};
		SceneState m_SceneState = SceneState::Edit;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel{};
		ContentBrowserPanel m_ContentBrowserPanel;
		
		// Editor resources
		Ref<Texture2D> m_IconPlay, m_IconStop;
	};

}
