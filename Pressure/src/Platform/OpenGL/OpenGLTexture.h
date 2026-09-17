#pragma once

#include "Pressure/Renderer/Texture.h"

#include <glad/glad.h>

namespace Pressure
{
	
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecification& specification, Buffer data = {});
		~OpenGLTexture2D() override;

		const TextureSpecification& GetSpecification() const override { return m_Specification; }

		uint32_t GetWidth() const override { return m_Specification.Width; }
		uint32_t GetHeight() const override { return m_Specification.Height; }
		uint32_t GetRendererID() const override { return m_RendererID; }

		void SetData(Buffer data) override;

		void Bind(uint32_t slot = 0) const override;

		bool operator==(const Texture& other) const override 
		{
			return m_RendererID == dynamic_cast<const OpenGLTexture2D&>(other).m_RendererID;
		}

		bool IsLoaded() const override { return m_IsLoaded; }

	private:
		TextureSpecification m_Specification;

		bool m_IsLoaded = false;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};
	
}
