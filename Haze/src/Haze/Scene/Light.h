#pragma once

#include "Haze/Core.h"

#include <glm/glm.hpp>
#include <array>

namespace Haze 
{

	enum LightType {
		AMBIENT, VECTOR, POINT
	};

	class HAZE_API Light 
	{
		public:
			Light(LightType type) : _Type(type) {}

			inline LightType GetType() const { return _Type; }
			inline std::array<glm::vec3, 4>& GetData() { return _Data; }
			inline bool& IsCastingShadow() { return _Shadow; }

			void Set(glm::vec3 col, glm::vec3 vec, glm::vec3 itt, glm::vec3 att, bool shadow) {
				Color(col);
				Vector(vec);
				Intensity(itt);
				Attenuation(att);
				Shadow(shadow);
			}

		protected:
			void Color(glm::vec3 color) { _Data[0] = color; }
			void Vector(glm::vec3 vector) { _Data[1] = vector; }
			void Intensity(glm::vec3 intensity) { _Data[2] = intensity; }
			void Attenuation(glm::vec3 attenuation) { _Data[3] = attenuation; }
			void Shadow(bool shadow) { _Shadow = shadow; }

		protected:
			LightType _Type;
			bool _Shadow;
			std::array<glm::vec3, 4> _Data;
	};

	class HAZE_API AmbientLight : public Light 
	{
		public:
			AmbientLight() : AmbientLight(glm::vec3(1.0f), 0.0f) {}
			AmbientLight(glm::vec3 color, float intensity) : Light(AMBIENT) 
			{
				Color(color);
				Intensity({ intensity, 0, 0 });
			}
	};

	class HAZE_API VectorLight : public Light 
	{
		public:
			VectorLight() : VectorLight(glm::vec3(1.0f), glm::vec3(0.0f), 0.0f, 0.0f) {}
			VectorLight(glm::vec3 color, glm::vec3 direction, float di, float si) : Light(VECTOR) 
			{
				Color(color);
				Vector(direction);
				Intensity({ 0, di, si });
			}
	};

	class HAZE_API PointLight : public Light {
		public:
			PointLight() : PointLight(glm::vec3(1.0f), glm::vec3(0.0f), 0.0f, 0.0f, 0.002f, 0.0007f, false) {}
			PointLight(glm::vec3 color, glm::vec3 position, float di, float si, float la, float qa, bool shadow = false) : Light(POINT) {
				Color(color);
				Vector(position);
				Intensity({ 0, di, si });
				Attenuation({ 1, la, qa });
				Shadow(shadow);
			}
	};

}