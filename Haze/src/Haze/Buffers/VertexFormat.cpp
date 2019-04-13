#include "hzpch.h"
#include "VertexFormat.h"

#include <glad/glad.h>

namespace Haze
{

	unsigned int VertexFormat::GetSize(int gindex) const {
		return gindex >= 0 ? _Subs[gindex]._Size : _Size;
	}

	unsigned int VertexFormat::GetCount(int gindex) const {
		return gindex >= 0 ? _Subs[gindex]._Count : (unsigned int) _Subs.size();
	}

	unsigned int VertexFormat::GetType(int gindex) const {
		return _Subs[gindex]._Type;
	}

	void VertexFormat::NextItem(int count, int size, int type) {
		_Subs.emplace_back(count, size * count, type);
		_Size += count * size;
	}

	VertexFormat& VertexFormat::GetDefault() {
		static VertexFormat& vf = []() -> VertexFormat {
			VertexFormat vf;
			vf.NextItem(3, sizeof(float), GL_FLOAT);
			vf.NextItem(3, sizeof(float), GL_FLOAT);
			vf.NextItem(2, sizeof(float), GL_FLOAT);
			vf.NextItem(3, sizeof(float), GL_FLOAT);
			vf.NextItem(3, sizeof(float), GL_FLOAT);
			return vf;
		}();

		return vf;
	}

}