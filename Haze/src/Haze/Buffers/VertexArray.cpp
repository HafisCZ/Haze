#include "hzpch.h"
#include "VertexArray.h"

#include <glad/glad.h>

namespace Haze 
{

	VertexArray::VertexArray() {
		glGenVertexArrays(1, &_Handle);
		glBindVertexArray(_Handle);
	}

	void VertexArray::BindBuffer(const VertexBuffer& buffer) 
	{
		Bind();
		buffer.Bind();
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (const void*)0);
		glEnableVertexAttribArray(0);
	
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (const void*)(3* sizeof(float)));
		glEnableVertexAttribArray(1);
		
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (const void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (const void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);
		
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (const void*)(11 * sizeof(float)));
		glEnableVertexAttribArray(4);
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