#include "hzpch.h"
#include "VertexBuffer.h"

#include <glad/glad.h>

namespace Haze 
{

	VertexBuffer::VertexBuffer(const void* data, unsigned int count)
	{
		glGenBuffers(1, &_Handle);
		glBindBuffer(GL_ARRAY_BUFFER, _Handle);
		glBufferData(GL_ARRAY_BUFFER, count * 56, data, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &_Handle);
	}

	void VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, _Handle);
	}

	void VertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

}