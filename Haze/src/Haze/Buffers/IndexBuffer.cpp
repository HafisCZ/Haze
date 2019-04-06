#include "hzpch.h"
#include "IndexBuffer.h"

#include <glad/glad.h>

namespace Haze 
{

	IndexBuffer::IndexBuffer(const void* data, unsigned int count)
	{
		glGenBuffers(1, &_Handle);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _Handle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer() 
	{
		glDeleteBuffers(1, &_Handle);
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _Handle);
	}

	void IndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}