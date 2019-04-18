#pragma once

#include "Haze/Core.h"
#include "Haze/Buffers/VertexFormat.h"

namespace Haze 
{

	class VertexBuffer
	{
		public:
			VertexBuffer(const void* data, unsigned int count, const VertexFormat& format);
			~VertexBuffer();

			void Bind() const;
			void Unbind() const;

		private:
			unsigned int _Handle;
	};

}