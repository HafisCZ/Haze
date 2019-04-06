#pragma once

#include "Haze/Core.h"

namespace Haze 
{

	class HAZE_API IndexBuffer 
	{
		public:
			IndexBuffer(const void* data, unsigned int count);
			~IndexBuffer();

			void Bind() const;
			void Unbind() const;

		private:
			unsigned int _Handle;
	};

}