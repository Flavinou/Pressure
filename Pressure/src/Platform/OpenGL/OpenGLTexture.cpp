#include "prspch.h"
#include "Platform/OpenGL/OpenGLTexture.h"

#include <stb_image.h>

namespace Pressure
{

	namespace
	{
		
		GLenum ImageFormatToGLDataFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB8: return GL_RGB;
				case ImageFormat::RGBA8: return GL_RGBA;
			}

			PRS_CORE_ASSERT(false);
			return 0;
		}

		GLenum ImageFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB8: return GL_RGB8;
				case ImageFormat::RGBA8: return GL_RGBA8;
			}

			PRS_CORE_ASSERT(false);
			return 0;
		}

	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification, Buffer data/* = {}*/)
		: m_Specification(specification)
	{
		PRS_PROFILE_FUNCTION();

		m_InternalFormat = ImageFormatToGLInternalFormat(m_Specification.Format);
		m_DataFormat = ImageFormatToGLDataFormat(m_Specification.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Specification.Width, m_Specification.Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (data)
		{
			OpenGLTexture2D::SetData(data);
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		PRS_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(Buffer data)
	{
		PRS_PROFILE_FUNCTION();

		uint32_t bytesPerPixel = m_DataFormat == GL_RGBA ? 4 : 3;
		PRS_CORE_ASSERT(data.Size == m_Specification.Width * m_Specification.Height * bytesPerPixel, "Data must be entire texture !");

		if (m_DataFormat == GL_RGB)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		}

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Specification.Width, m_Specification.Height, m_DataFormat, GL_UNSIGNED_BYTE, data.Data);

		if (m_DataFormat == GL_RGB)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		}
		
		if (m_Specification.GenerateMips)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		PRS_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}

}
