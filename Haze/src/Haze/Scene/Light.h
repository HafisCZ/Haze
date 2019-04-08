#pragma once

#include "Haze/Core.h"

#include <glm/glm.hpp>
#include <array>

namespace Haze 
{

	enum LightType {
		AMBIENT, VECTOR, POINT, POINT_SHADOW
	};

	class HAZE_API Light 
	{
		public:
			Light(LightType type) : _Type(type) {}

			inline LightType GetType() const { return _Type; }
			inline std::array<glm::vec3, 4>& GetData() { return _Data; }

		protected:
			void Color(glm::vec3 color) { _Data[0] = color; }
			void Vector(glm::vec3 vector) { _Data[1] = vector; }
			void Intensity(glm::vec3 intensity) { _Data[2] = intensity; }
			void Attenuation(glm::vec3 attenuation) { _Data[3] = attenuation; }

		protected:
			LightType _Type;
			std::array<glm::vec3, 4> _Data;
	};

	class HAZE_API AmbientLight : public Light 
	{
		public:
			AmbientLight(glm::vec3 color, float intensity) : Light(AMBIENT) 
			{
				Color(color);
				Intensity({ intensity, 0, 0 });
			}
	};

	class HAZE_API VectorLight : public Light 
	{
		public:
			VectorLight(glm::vec3 color, glm::vec3 direction, float di, float si) : Light(VECTOR) 
			{
				Color(color);
				Vector(direction);
				Intensity({ 0, di, si });
			}
	};

	class HAZE_API PointLight : public Light {
		public:
			PointLight(glm::vec3 color, glm::vec3 position, float di, float si, float la, float qa, bool shadow = false) : Light(shadow ? POINT_SHADOW : POINT) {
				Color(color);
				Vector(position);
				Intensity({ 0, di, si });
				Attenuation({ 1, la, qa });
			}
	};

}