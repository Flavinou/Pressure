#include <prspch.h>
#include "Platform/OpenGL/OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Pressure
{

	namespace Utils
	{

		static GLenum TextureTarget(bool multiSampled)
		{
			return multiSampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static GLenum TextureFormat(FrameBufferTextureFormat format)
		{
			switch (format)
			{
				case FrameBufferTextureFormat::RGBA8:
					return GL_RGBA8;
				case FrameBufferTextureFormat::RED_INTEGER:
					return GL_R32I;
				case FrameBufferTextureFormat::DEPTH24STENCIL8:
					return GL_DEPTH24_STENCIL8;
			}

			PRS_CORE_ASSERT(false, "Unsupported FrameBufferTextureFormat for texture format conversion!");
			return GL_NONE;
		}

		static GLenum PixelDataFormat(FrameBufferTextureFormat format)
		{
			switch (format)
			{
				case FrameBufferTextureFormat::RGBA8:
					return GL_RGBA;
				case FrameBufferTextureFormat::RED_INTEGER:
					return GL_RED_INTEGER;
				case FrameBufferTextureFormat::DEPTH24STENCIL8:
					return GL_DEPTH_STENCIL; // Depth textures don't have a pixel data format, but we use this for glTexStorage2D
			}

			PRS_CORE_ASSERT(false, "Unsupported FrameBufferTextureFormat for pixel data format conversion!");
			return GL_NONE;
		}

		static bool IsDepthFormat(FrameBufferTextureFormat format)
		{
			return format == FrameBufferTextureFormat::DEPTH24STENCIL8;
		}

		static void CreateTextures(bool multiSampled, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multiSampled), count, outID);
		}

		static void BindTexture(bool multiSampled, uint32_t id)
		{
			glBindTexture(TextureTarget(multiSampled), id);
		}

		static void AttachTexture(uint32_t id, int samples, FrameBufferTextureFormat format, GLenum attachmentType, uint32_t width, uint32_t height, int index = 0)
		{
			bool multiSampled = samples > 1;
			GLenum target = TextureTarget(multiSampled);
			GLenum internalFormat = TextureFormat(format);
			if (multiSampled)
			{
				glTexImage2DMultisample(target, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				// Create depth texture attachment
				if (IsDepthFormat(format))
				{
					glTexStorage2D(target, 1, internalFormat, width, height);
				}
				else // Create color texture attachment
				{
					glTexImage2D(target, 0, internalFormat, width, height, 0, PixelDataFormat(format), GL_UNSIGNED_BYTE, nullptr);
				}

				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType + index, target, id, 0);
		}

	}

    static const uint32_t s_MaxFrameBufferSize = 8192;

    OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
        : m_Specification(spec)
    {
		for (auto textureSpec : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(textureSpec.TextureFormat))
			{
				m_ColorAttachmentSpecifications.emplace_back(textureSpec);
			}
			else
			{
				m_DepthAttachmentSpecification = textureSpec;
			}
		}

        Invalidate();
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
        glDeleteTextures(1, &m_DepthAttachment);
    }

    void OpenGLFrameBuffer::Invalidate()
    {
        if (m_RendererID)
        {
            glDeleteFramebuffers(1, &m_RendererID);
            glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
            glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
        }

        glCreateFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		// Attachments
		bool multiSampled = m_Specification.Samples > 1;

		if (!m_ColorAttachmentSpecifications.empty())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			Utils::CreateTextures(multiSampled, m_ColorAttachments.data(), m_ColorAttachments.size());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				Utils::BindTexture(multiSampled, m_ColorAttachments[i]);
				Utils::AttachTexture(m_ColorAttachments[i], m_Specification.Samples, m_ColorAttachmentSpecifications[i].TextureFormat, GL_COLOR_ATTACHMENT0, m_Specification.Width, m_Specification.Height, i);
			}
		}

		if (m_DepthAttachmentSpecification.TextureFormat != FrameBufferTextureFormat::None)
		{
			Utils::CreateTextures(multiSampled, &m_DepthAttachment, 1);
			Utils::BindTexture(multiSampled, m_DepthAttachment);
			Utils::AttachTexture(m_DepthAttachment, m_Specification.Samples, m_DepthAttachmentSpecification.TextureFormat, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
		}

		if (m_ColorAttachments.empty())
		{
			// Only depth-pass
			glDrawBuffer(GL_NONE);
		}
		else
		{
			PRS_CORE_ASSERT(m_ColorAttachments.size() <= 4, "No more than 4 color attachments are supported at the moment!");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}

        PRS_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFrameBuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Specification.Width, m_Specification.Height);
    }

    void OpenGLFrameBuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize)
        {
            PRS_CORE_WARN("Attempted to resize FrameBuffer to {0}, {1}", width, height);
            return;
        }

        m_Specification.Width = width;
        m_Specification.Height = height;

        Invalidate();
    }

	int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		PRS_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

	void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		PRS_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
		glClearTexImage(m_ColorAttachments[attachmentIndex], 0, Utils::PixelDataFormat(spec.TextureFormat), GL_INT, &value);
	}

}
