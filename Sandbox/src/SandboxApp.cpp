#include <Pressure.h>

#include <Platform/OpenGL/OpenGLShader.h>

#include "imgui/imgui.h"

#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Pressure::Layer
{
public:
	ExampleLayer()
		:Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_SquareColor(1.0f)
	{
        m_VertexArray.reset(Pressure::VertexArray::Create());

        float vertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
        };

        Pressure::Ref<Pressure::VertexBuffer> vertexBuffer;
        vertexBuffer.reset(Pressure::VertexBuffer::Create(vertices, sizeof(vertices)));
		Pressure::BufferLayout layout = {
            { Pressure::ShaderDataType::Float3, "a_Position" },
            { Pressure::ShaderDataType::Float4, "a_Color" }
        };
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        unsigned int indices[3] = { 0, 1, 2 };
        Pressure::Ref<Pressure::IndexBuffer> indexBuffer;
        indexBuffer.reset(Pressure::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(indexBuffer);

        m_SquareVA.reset(Pressure::VertexArray::Create());

        float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        };
        Pressure::Ref<Pressure::VertexBuffer> squareVB;
        squareVB.reset(Pressure::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

        squareVB->SetLayout({
            { Pressure::ShaderDataType::Float3, "a_Position" },
            { Pressure::ShaderDataType::Float2, "a_TexCoord"}
        });
        m_SquareVA->AddVertexBuffer(squareVB);

        unsigned int squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
        Pressure::Ref<Pressure::IndexBuffer> squareIB;
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

        m_Shader.reset(Pressure::Shader::Create(vertexSrc, fragmentSrc));

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

        m_FlatColorShader.reset(Pressure::Shader::Create(flatColorShaderVertexSrc, flatColorShaderFragmentSrc));

		std::string textureShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec2 a_TexCoord;

			uniform mat4 u_ViewProjectionMatrix;
            uniform mat4 u_Transform;

            out vec2 v_TexCoord;

			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string textureShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

            in vec2 v_TexCoord;

            uniform sampler2D u_Texture;

			void main()
			{
				color = texture(u_Texture, v_TexCoord);
			}
		)";

		m_TextureShader.reset(Pressure::Shader::Create(textureShaderVertexSrc, textureShaderFragmentSrc));

        m_WaterTexture = Pressure::Texture2D::Create("assets/textures/Water.png");
        m_VoronoiTexture = Pressure::Texture2D::Create("assets/textures/Voronoi2.png");
        m_CloudyTexture = Pressure::Texture2D::Create("assets/textures/cloudy.png");

		std::dynamic_pointer_cast<Pressure::OpenGLShader>(m_TextureShader)->Bind();
		std::dynamic_pointer_cast<Pressure::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);
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

        glm::vec4 redColor(0.8f, 0.2f, 0.3f, 1.0f);
        glm::vec4 blueColor(0.2f, 0.3f, 0.8f, 1.0f);

        // Goal API for Material system
        //  - Create a Material object that will take a Shader Object as parameter
        //  - This way, when doing this through a potential editor, we can settle "standard" 
        //    data/ information for future objects that would use a particular material
        // 
        // Pressure::MaterialRef material = new Pressure::Material(m_FlatColorShader);
        // Pressure::MaterialInstanceRef mi = new Pressure::MaterialInstance(material);
        // 
        // material->Set("u_Color", m_PickableColor);
        // squareMesh->SetMaterial(material);

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

        /*m_WaterTexture->Bind();
        Pressure::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));*/

		m_VoronoiTexture->Bind();
		Pressure::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		m_CloudyTexture->Bind();
		Pressure::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

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

	void OnEvent(Pressure::Event& event) override
	{

	}

private:
    Pressure::Ref<Pressure::Shader> m_Shader;
    Pressure::Ref<Pressure::VertexArray> m_VertexArray;

    Pressure::Ref<Pressure::Shader> m_FlatColorShader, m_TextureShader;
    Pressure::Ref<Pressure::VertexArray> m_SquareVA;

    Pressure::Ref<Pressure::Texture2D> m_WaterTexture, m_VoronoiTexture, m_CloudyTexture;

    Pressure::OrthographicCamera m_Camera;

    glm::vec3 m_CameraPosition;
    float m_CameraRotation = 0.0f;

    float m_CameraRotationSpeed = 180.0f;
    float m_CameraMoveSpeed = 10.0f;
    glm::vec3 m_SquareColor;
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