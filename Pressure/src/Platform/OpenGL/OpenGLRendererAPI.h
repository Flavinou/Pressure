#pragma once

#include "Pressure/Renderer/RendererAPI.h"

namespace Pressure
{

	class OpenGLRendererAPI : public RendererAPI
	{
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const Pressure::Ref<VertexArray>& vertexArray) override;
	};

}

