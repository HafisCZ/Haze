#pragma once

#include "Haze/Core.h"

#include "Haze/Buffers/VertexBuffer.h"

namespace Haze 
{

	class VertexArray
	{
		public:
			VertexArray();
			~VertexArray();

			void BindBuffer(const VertexBuffer& buffer, const VertexFormat& format);

			void Bind() const;
			void Unbind() const;

		private:
			unsigned int _Handle;
	};

}