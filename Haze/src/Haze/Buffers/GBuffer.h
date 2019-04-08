#pragma once

#include "Haze/Core.h"

namespace Haze 
{

	class HAZE_API GBuffer 
	{
		public:
			GBuffer(unsigned int width, unsigned int height);

			void Resize(unsigned int width, unsigned int height);

			void BindTextures(unsigned int slot);
			void Copy(unsigned int width, unsigned int height);

			void Bind();
			void Unbind();

		private:
			unsigned int _Handle;
			unsigned int _TextureHandles[3];
			unsigned int _Depth;

			enum TextureIndexes 
			{
				POSITION, NORMAL, TEXTURE
			};
	};

}