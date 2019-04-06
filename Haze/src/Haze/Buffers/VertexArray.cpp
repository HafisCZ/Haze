#include "hzpch.h"
#include "VertexArray.h"

#include <glad/glad.h>

namespace Haze 
{

	VertexArray::VertexArray(const VertexBuffer& buffer) {
		glGenVertexArrays(1, &_Handle);
		glBindVertexArray(_Handle);

		buffer.Bind();

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 56, (const void*) 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 56, (const void*) 12);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 56, (const void*) 24);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 56, (const void*) 32);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 56, (const void*) 44);
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