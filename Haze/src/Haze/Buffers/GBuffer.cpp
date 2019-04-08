#include"hzpch.h"
#include "GBuffer.h"

#include <glad/glad.h>

namespace Haze 
{

	GBuffer::GBuffer(unsigned int width, unsigned int height) 
	{
		glGenFramebuffers(1, &_Handle);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _Handle);

		glGenTextures(3, _TextureHandles);

		for (unsigned int i = 0; i < 3; i++) 
		{
			glBindTexture(GL_TEXTURE_2D, _TextureHandles[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, i == TEXTURE ? GL_RGBA : GL_RGB16F, width, height, 0, i == TEXTURE ? GL_RGBA : GL_RGB, i == TEXTURE ? GL_UNSIGNED_BYTE : GL_FLOAT, nullptr);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _TextureHandles[i], 0);
		}

		glDrawBuffers(3, new unsigned int[3] { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 });

		glGenRenderbuffers(1, &_Depth);
		glBindRenderbuffer(GL_RENDERBUFFER, _Depth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _Depth);

		unsigned int status;
		if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) 
		{
			HZ_CORE_CRITICAL("GBUFFER CREATION ERROR! {0}", status);
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	void GBuffer::Resize(unsigned int width, unsigned int height) 
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _Handle);

		for (unsigned int i = 0; i < 3; i++) 
		{
			glBindTexture(GL_TEXTURE_2D, _TextureHandles[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, i == TEXTURE ? GL_RGBA : GL_RGB16F, width, height, 0, i == TEXTURE ? GL_RGBA : GL_RGB, i == TEXTURE ? GL_UNSIGNED_BYTE : GL_FLOAT, nullptr);
		}

		glBindRenderbuffer(GL_RENDERBUFFER, _Depth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	void GBuffer::BindTextures(unsigned int slot) 
	{
		for (unsigned int i = 0; i < 3; i++) 
		{
			glActiveTexture(GL_TEXTURE0 + slot + i);
			glBindTexture(GL_TEXTURE_2D, _TextureHandles[i]);
		}
	}

	void GBuffer::Bind() 
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _Handle);
	}

	void GBuffer::Unbind() 
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GBuffer::Copy(unsigned int width, unsigned int height)
	{ 
		glBindFramebuffer(GL_READ_FRAMEBUFFER, _Handle);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}