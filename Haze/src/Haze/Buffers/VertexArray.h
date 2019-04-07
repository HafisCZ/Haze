#pragma once

#include "Haze/Core.h"

#include "Haze/Buffers/VertexBuffer.h"

namespace Haze 
{

	class HAZE_API VertexArray 
	{
		public:
			VertexArray();
			~VertexArray();

			void BindBuffer(const VertexBuffer& buffer);

			void Bind() const;
			void Unbind() const;

		private:
			unsigned int _Handle;
	};

}