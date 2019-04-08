#include "hzpch.h"
#include "CubeFramebuffer.h"

#include <glad/glad.h>

namespace Haze 
{

	CubeFramebufferArray::CubeFramebufferArray(unsigned int size, unsigned int count) : _Size(size), _Count(count)
	{
		glGenFramebuffers(1, &_Handle);
		glGenTextures(1, &_TextureHandle);
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, _TextureHandle);

		glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT, size, size, count * 6, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);

		float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BORDER_COLOR, color);
		
		glBindFramebuffer(GL_FRAMEBUFFER, _Handle);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _TextureHandle, 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			HZ_CORE_CRITICAL("Framebuffer error!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void CubeFramebufferArray::Resize(unsigned int size, unsigned int count) {
		_Size = size;
		_Count = count;

		glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, _TextureHandle);
		glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT, size, size, count * 6, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}

	void CubeFramebufferArray::Bind() 
	{
		glGetIntegerv(GL_VIEWPORT, _ViewportResolutionTemp);
		glViewport(0, 0, _Size, _Size);

		glBindFramebuffer(GL_FRAMEBUFFER, _Handle);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void CubeFramebufferArray::Unbind(unsigned int slot) 
	{
		glViewport(0, 0, _ViewportResolutionTemp[2], _ViewportResolutionTemp[3]);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, _TextureHandle);
	}

}