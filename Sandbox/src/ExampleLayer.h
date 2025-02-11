#pragma once

#include <Pressure.h>

class ExampleLayer : public Pressure::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer() = default;

	void OnUpdate(Pressure::Timestep ts) override;
	void OnEvent(Pressure::Event& e) override;
	virtual void OnImGuiRender() override;

private:
	Pressure::ShaderLibrary m_ShaderLibrary;
	Pressure::Ref<Pressure::Shader> m_Shader;
	Pressure::Ref<Pressure::VertexArray> m_VertexArray;

	Pressure::Ref<Pressure::Shader> m_FlatColorShader;
	Pressure::Ref<Pressure::VertexArray> m_SquareVA;

	Pressure::Ref<Pressure::Texture2D> m_VoronoiTexture, m_CloudyTexture;

	Pressure::OrthographicCameraController m_CameraController;
	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};