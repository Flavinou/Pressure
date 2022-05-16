#include "Sandbox2D.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
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
	{
		PRS_PROFILE_SCOPE("Renderer preparation");
		Pressure::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Pressure::RenderCommand::Clear();
	}

	{
		PRS_PROFILE_SCOPE("Renderer Draw");
		Pressure::Renderer2D::BeginScene(m_CameraController.GetCamera());
		//Pressure::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, glm::radians(-60.0f), { 0.8f, 0.2f, 0.3f, 1.0f });
		Pressure::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Pressure::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		//Pressure::Renderer2D::DrawRotatedQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, glm::radians(60.0f), m_VoronoiTexture, 10.0f, { 1.0f, 0.9f, 0.9f, 1.0f });
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
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}