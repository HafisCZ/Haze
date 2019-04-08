#pragma once

#include "Haze/Core.h"

namespace Haze 
{

	class HAZE_API CubeFramebufferArray
	{
		public:
			CubeFramebufferArray(unsigned int size, unsigned int count);

			void Resize(unsigned int size, unsigned int count);

			void Bind();
			void Unbind(unsigned int slot);

		private:
			unsigned int _Handle;
			unsigned int _TextureHandle;
			unsigned int _Size;
			unsigned int _Count;

			int _ViewportResolutionTemp[4];
	};

}