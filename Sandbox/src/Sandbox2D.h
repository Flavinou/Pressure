#pragma once

#include "Pressure.h"

class Sandbox2D : public Pressure::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Pressure::Timestep ts) override;
	void OnEvent(Pressure::Event& e) override;
	virtual void OnImGuiRender() override;
private:
	Pressure::OrthographicCameraController m_CameraController;

	// Temporary
	Pressure::Ref<Pressure::VertexArray> m_SquareVA;
	Pressure::Ref<Pressure::Shader> m_FlatColorShader;
	Pressure::Ref<Pressure::Texture2D> m_VoronoiTexture;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};