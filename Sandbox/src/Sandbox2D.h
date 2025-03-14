#pragma once

#include <Pressure.h>

#include "ParticleSystem.h"

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
    Pressure::Ref<Pressure::Texture2D> m_SpriteSheet;
    Pressure::Ref<Pressure::SubTexture2D> m_TextureStairs, m_TextureBarrel, m_TextureTree;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

	ParticleSystem m_ParticleSystem;
	ParticleProps m_ParticleProps;

	uint32_t m_MapWidth, m_MapHeight;
	std::unordered_map<char, Pressure::Ref<Pressure::SubTexture2D>> s_TextureMap;
};