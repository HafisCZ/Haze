#pragma once

#include "Haze/Core.h"

namespace Haze 
{

	class HAZE_API VertexFormat
	{
		private:
			struct VertexSubFormat 
			{
				unsigned int _Count;
				unsigned int _Size;
				unsigned int _Type;

				VertexSubFormat(unsigned int count, unsigned int size, unsigned int type) : _Count(count), _Size(size), _Type(type) {}
			};

		public:
			unsigned int GetSize(int gindex = -1) const;
			unsigned int GetCount(int gindex = -1) const;
			unsigned int GetType(int gindex) const;

			void NextItem(int count, int size, int type);

			static VertexFormat& GetDefault();

		private:
			unsigned int _Size = 0;
			std::vector<VertexSubFormat> _Subs;

	};

}