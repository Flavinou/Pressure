#include <Pressure.h>

#include "imgui/imgui.h"
#include "glm/gtc/matrix_transform.hpp"

class ExampleLayer : public Pressure::Layer
{
public:
	ExampleLayer()
		:Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
	{
        m_VertexArray.reset(Pressure::VertexArray::Create());

        float vertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
        };

        std::shared_ptr<Pressure::VertexBuffer> vertexBuffer;
        vertexBuffer.reset(Pressure::VertexBuffer::Create(vertices, sizeof(vertices)));
		Pressure::BufferLayout layout = {
            { Pressure::ShaderDataType::Float3, "a_Position" },
            { Pressure::ShaderDataType::Float4, "a_Color" }
        };
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        unsigned int indices[3] = { 0, 1, 2 };
        std::shared_ptr<Pressure::IndexBuffer> indexBuffer;
        indexBuffer.reset(Pressure::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(indexBuffer);

        m_SquareVA.reset(Pressure::VertexArray::Create());

        float squareVertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f,
        };
        std::shared_ptr<Pressure::VertexBuffer> squareVB;
        squareVB.reset(Pressure::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

        squareVB->SetLayout({
            { Pressure::ShaderDataType::Float3, "a_Position" },
            });
        m_SquareVA->AddVertexBuffer(squareVB);

        unsigned int squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
        std::shared_ptr<Pressure::IndexBuffer> squareIB;
        squareIB.reset(Pressure::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
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

        m_Shader.reset(new Pressure::Shader(vertexSrc, fragmentSrc));

        std::string blueShaderVertexSrc = R"(
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

        std::string blueShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

        m_BlueShader.reset(new Pressure::Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
	}

	void OnUpdate(Pressure::Timestep ts) override
	{
        PRS_TRACE("Delta time : {0}s ({1})", ts, ts.GetMilliseconds());

        // Camera movement with arrows
        if (Pressure::Input::IsKeyPressed(PRS_KEY_LEFT))
            m_CameraPosition.x -= m_CameraMoveSpeed * ts;
        else if (Pressure::Input::IsKeyPressed(PRS_KEY_RIGHT))
            m_CameraPosition.x += m_CameraMoveSpeed * ts;
        if (Pressure::Input::IsKeyPressed(PRS_KEY_UP))
            m_CameraPosition.y -= m_CameraMoveSpeed * ts;
        else if (Pressure::Input::IsKeyPressed(PRS_KEY_DOWN))
            m_CameraPosition.y += m_CameraMoveSpeed * ts;

        // Camera rotation
        if (Pressure::Input::IsKeyPressed(PRS_KEY_A))
            m_CameraRotation += m_CameraRotationSpeed * ts;
        if (Pressure::Input::IsKeyPressed(PRS_KEY_D))
            m_CameraRotation -= m_CameraRotationSpeed * ts;

        Pressure::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        Pressure::RenderCommand::Clear();

        m_Camera.SetPosition(m_CameraPosition);
        m_Camera.SetRotation(m_CameraRotation);

        Pressure::Renderer::BeginScene(m_Camera);

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

        for (int x = 0; x < 20; x++)
        {
			for (int y = 0; y < 20; y++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Pressure::Renderer::Submit(m_BlueShader, m_SquareVA, transform);
			}
        }

        Pressure::Renderer::Submit(m_Shader, m_VertexArray);

        Pressure::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{

	}

	void OnEvent(Pressure::Event& event) override
	{

	}

private:
    std::shared_ptr<Pressure::Shader> m_Shader;
    std::shared_ptr<Pressure::VertexArray> m_VertexArray;

    std::shared_ptr<Pressure::Shader> m_BlueShader;
    std::shared_ptr<Pressure::VertexArray> m_SquareVA;

    Pressure::OrthographicCamera m_Camera;

    glm::vec3 m_CameraPosition;
    float m_CameraRotation = 0.0f;

    float m_CameraRotationSpeed = 180.0f;
    float m_CameraMoveSpeed = 10.0f;
};

class Sandbox : public Pressure::Application 
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}
};

Pressure::Application* Pressure::CreateApplication()
{
	return new Sandbox();
}