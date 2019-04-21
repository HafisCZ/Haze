#pragma once

#include <utility>

#include <glm/glm.hpp>

#include "Haze/Scene/ModelMatrix.h"

namespace Math
{

	inline glm::mat3 RotationMat3X(float angle) 
	{
		return {
			{ cosf(angle), -sinf(angle), 0.0f },
			{ sinf(angle), cosf(angle), 0.0f },
			{ 0.0f, 0.0f, 0.0f }
		};
	}

	inline glm::mat3 RotationMat3Y(float angle) 
	{
		return {
			{ 0.0f, 0.0f, 0.0f },
			{ 0.0f, cosf(angle), -sinf(angle) },
			{ 0.0f, sinf(angle), cosf(angle) }
		};
	}
	
	inline glm::mat3 RotationMat3Z(float angle) 
	{
		return {
			{ cosf(angle), 0.0f, -sinf(angle) },
			{ 0.0f, 0.0f, 0.0f },
			{ sinf(angle), 0.0f, cosf(angle) }
		};
	}

	inline glm::mat3 RotationMat3(float xangle, float yangle, float zangle) 
	{
		return RotationMat3X(xangle) * RotationMat3Y(yangle) * RotationMat3Z(zangle);
	}

	inline std::pair<bool, float> Intersects_OBB_RAY(glm::vec3 obb_min, glm::vec3 obb_max, glm::vec3 obb_scale, const glm::mat4& modelMatrix, glm::vec3 rayOrigin, glm::vec3 rayDir) 
	{
		float min = 0.0f;
		float max = std::numeric_limits<float>::max();

		glm::vec3 bmin = obb_min * obb_scale * obb_scale;
		glm::vec3 bmax = obb_max * obb_scale * obb_scale;

		glm::vec3 modelOrigin = { modelMatrix[3].x, modelMatrix[3].y, modelMatrix[3].z };
		glm::vec3 delta = modelOrigin - rayOrigin;

		glm::vec3 xAxis = { modelMatrix[0].x, modelMatrix[0].y, modelMatrix[0].z };
		float e = glm::dot(xAxis, delta);
		float f = glm::dot(rayDir, xAxis);
		
		if (fabs(f) > 0.001f) {
			float t1 = (e + bmin.x) / f;
			float t2 = (e + bmax.x) / f;

			if (t1 > t2) {
				float w = t1;
				t1 = t2;
				t2 = w;
			}

			if (t2 < max) max = t2;
			if (t1 > min) min = t1;
			if (max < min) return { false, -1.0f };
		} else {
			if (bmin.x - e > 0.0f || bmax.x - e < 0.0f) return { false, -1.0f };
		}
		
		glm::vec3 yAxis = { modelMatrix[1].x, modelMatrix[1].y, modelMatrix[1].z };
		e = glm::dot(yAxis, delta);
		f = glm::dot(rayDir, yAxis);

		if (fabs(f) > 0.001f) {
			float t1 = (e + bmin.y) / f;
			float t2 = (e + bmax.y) / f;

			if (t1 > t2) {
				float w = t1;
				t1 = t2;
				t2 = w;
			}

			if (t2 < max) max = t2;
			if (t1 > min) min = t1;
			if (max < min) return { false, -1.0f };
		} else {
			if (bmin.y - e > 0.0f || bmax.y - e < 0.0f) return { false, -1.0f };
		}

		glm::vec3 zAxis = { modelMatrix[2].x, modelMatrix[2].y, modelMatrix[2].z };
		e = glm::dot(zAxis, delta);
		f = glm::dot(rayDir, zAxis);

		if (fabs(f) > 0.001f) {
			float t1 = (e + bmin.z) / f;
			float t2 = (e + bmax.z) / f;

			if (t1 > t2) {
				float w = t1;
				t1 = t2;
				t2 = w;
			}

			if (t2 < max) max = t2;
			if (t1 > min) min = t1;
			if (max < min) return { false, -1.0f };
		} else {
			if (bmin.z - e > 0.0f || bmax.z - e < 0.0f) return { false, -1.0f };
		}

		return { true, min };
	} 

}