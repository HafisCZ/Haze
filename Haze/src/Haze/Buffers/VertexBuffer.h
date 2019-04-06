#pragma once

#include "Haze/Core.h"

namespace Haze 
{

	class HAZE_API VertexBuffer 
	{
		public:
			VertexBuffer(const void* data, unsigned int count);
			~VertexBuffer();

			void Bind() const;
			void Unbind() const;

		private:
			unsigned int _Handle;
	};

}