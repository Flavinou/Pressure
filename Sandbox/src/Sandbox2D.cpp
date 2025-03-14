#include "Sandbox2D.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
{
}

void Sandbox2D::OnAttach()
{
	PRS_PROFILE_FUNCTION();

	m_VoronoiTexture = Pressure::Texture2D::Create("assets/textures/Voronoi2.png");
}

void Sandbox2D::OnDetach()
{
	PRS_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Pressure::Timestep ts)
{
	PRS_PROFILE_FUNCTION();

	// Update
	m_CameraController.OnUpdate(ts);

	// Render
	Pressure::Renderer2D::ResetStats();

	{
		PRS_PROFILE_SCOPE("Renderer preparation");
		Pressure::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Pressure::RenderCommand::Clear();
	}

	{
		static float rotation = 0.0f;
		rotation += ts * 50.0f;

		PRS_PROFILE_SCOPE("Renderer Draw");
		Pressure::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Pressure::Renderer2D::DrawRotatedQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, -45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });
        Pressure::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
        Pressure::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, m_SquareColor);
		Pressure::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_VoronoiTexture, 10.0f);
        Pressure::Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, rotation, m_VoronoiTexture, 20.0f);
        Pressure::Renderer2D::EndScene();

        Pressure::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Pressure::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}
		Pressure::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnEvent(Pressure::Event& e)
{
	m_CameraController.OnEvent(e);
}

void Sandbox2D::OnImGuiRender()
{
	PRS_PROFILE_FUNCTION();

	ImGui::Begin("Settings");

    auto engineStats = Pressure::Application::GetStats();
    ImGui::Text("Engine Stats:");
	ImGui::Text("Frame time : %.2f ms (%.2f FPS)", engineStats.FrameTime, engineStats.FramesPerSecond);
	ImGui::NewLine();

	auto stats = Pressure::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	ImGui::NewLine();

	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}