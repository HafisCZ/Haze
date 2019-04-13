#include "hzpch.h"
#include "VertexArray.h"

#include <glad/glad.h>

namespace Haze 
{

	VertexArray::VertexArray() {
		glGenVertexArrays(1, &_Handle);
		glBindVertexArray(_Handle);
	}

	void VertexArray::BindBuffer(const VertexBuffer& buffer, const VertexFormat& format)
	{
		Bind();
		buffer.Bind();

		for (unsigned int i = 0, offset = 0; i < format.GetCount(); offset += format.GetSize(i++)) 
		{
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, format.GetCount(i), format.GetType(i), GL_FALSE, format.GetSize(), (const void *)(offset));
		}
	}

	VertexArray::~VertexArray() {
		glDeleteVertexArrays(1, &_Handle);
	}

	void VertexArray::Bind() const 
	{
		glBindVertexArray(_Handle);
	}

	void VertexArray::Unbind() const 
	{
		glBindVertexArray(0);
	}

}