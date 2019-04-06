#pragma once

#include "Haze/Core.h"

#include "Haze/Buffers/VertexBuffer.h"

namespace Haze 
{

	class HAZE_API VertexArray 
	{
		public:
			VertexArray(const VertexBuffer& buffer);
			~VertexArray();

			void Bind() const;
			void Unbind() const;

		private:
			unsigned int _Handle;
	};

}