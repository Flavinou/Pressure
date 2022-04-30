#include <Pressure.h>
#include <Pressure/Core/EntryPoint.h>

#include <Platform/OpenGL/OpenGLShader.h>

#include "imgui/imgui.h"

#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"

class ExampleLayer : public Pressure::Layer
{
public:
	ExampleLayer()
		:Layer("Example"), m_CameraController(1280.0f / 720.0f)
	{
        m_VertexArray = Pressure::VertexArray::Create();

        float vertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
        };

        Pressure::Ref<Pressure::VertexBuffer> vertexBuffer;
        vertexBuffer = Pressure::VertexBuffer::Create(vertices, sizeof(vertices));
		Pressure::BufferLayout layout = {
            { Pressure::ShaderDataType::Float3, "a_Position" },
            { Pressure::ShaderDataType::Float4, "a_Color" }
        };
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        unsigned int indices[3] = { 0, 1, 2 };
        Pressure::Ref<Pressure::IndexBuffer> indexBuffer;
        indexBuffer = Pressure::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
        m_VertexArray->SetIndexBuffer(indexBuffer);

        m_SquareVA = Pressure::VertexArray::Create();

        float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        };
        Pressure::Ref<Pressure::VertexBuffer> squareVB;
        squareVB = Pressure::VertexBuffer::Create(squareVertices, sizeof(squareVertices));

        squareVB->SetLayout({
            { Pressure::ShaderDataType::Float3, "a_Position" },
            { Pressure::ShaderDataType::Float2, "a_TexCoord"}
        });
        m_SquareVA->AddVertexBuffer(squareVB);

        unsigned int squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
        Pressure::Ref<Pressure::IndexBuffer> squareIB;
        squareIB = Pressure::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
        m_SquareVA->SetIndexBuffer(squareIB);

        // Shader "magic"
        std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjectionMatrix;
            uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0);
			}
		)";

        std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

        m_Shader = Pressure::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

        std::string flatColorShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjectionMatrix;
            uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0);
			}
		)";

        std::string flatColorShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

            uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

        m_FlatColorShader = Pressure::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

        m_VoronoiTexture = Pressure::Texture2D::Create("assets/textures/Voronoi2.png");
        m_CloudyTexture = Pressure::Texture2D::Create("assets/textures/cloudy.png");

		std::dynamic_pointer_cast<Pressure::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Pressure::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(Pressure::Timestep ts) override
	{
		// Update
		m_CameraController.OnUpdate(ts);

		// Render
        Pressure::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        Pressure::RenderCommand::Clear();

        Pressure::Renderer::BeginScene(m_CameraController.GetCamera());

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

        glm::vec4 redColor(0.8f, 0.2f, 0.3f, 1.0f);
        glm::vec4 blueColor(0.2f, 0.3f, 0.8f, 1.0f);

        std::dynamic_pointer_cast<Pressure::OpenGLShader>(m_FlatColorShader)->Bind();
        std::dynamic_pointer_cast<Pressure::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

        for (int x = 0; x < 20; x++)
        {
			for (int y = 0; y < 20; y++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Pressure::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
        }

        auto textureShader = m_ShaderLibrary.Get("Texture");

		m_VoronoiTexture->Bind();
		Pressure::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		m_CloudyTexture->Bind();
		Pressure::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

        // Triangle draw call
        // Pressure::Renderer::Submit(m_Shader, m_VertexArray);

        Pressure::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Squares color", glm::value_ptr(m_SquareColor));
        ImGui::End();
	}

	void OnEvent(Pressure::Event& e) override
	{
		m_CameraController.OnEvent(e);

		if (e.GetEventType() == Pressure::EventType::WindowResize)
		{
			auto& re = (Pressure::WindowResizeEvent&)e;

			//float zoom = (float)re.GetWidth() / 1280.0f;
			//m_CameraController.SetZoomLevel(zoom);
		}
	}

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

class Sandbox : public Pressure::Application 
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{
	}
};

Pressure::Application* Pressure::CreateApplication()
{
	return new Sandbox();
}