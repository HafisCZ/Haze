#include "hzpch.h"
#include "ModelMatrix.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Haze 
{

	void ModelMatrix::UpdateMatrices(bool force) {
		if (_Invalid || force) {
			_TempMatrix = glm::mat4(1.0f);

			_TempMatrix = glm::translate(_TempMatrix, _Position);

			static const glm::vec3 axis[] = { { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } };
			_TempMatrix = glm::rotate(_TempMatrix, _Rotation.x, axis[0]);
			_TempMatrix = glm::rotate(_TempMatrix, _Rotation.y, axis[1]);
			_TempMatrix = glm::rotate(_TempMatrix, _Rotation.z, axis[2]);

			_TempMatrix = glm::scale(_TempMatrix, _Scale);

			_TempMatrixI = glm::transpose(glm::inverse(_TempMatrix));

			_Invalid = false;
		}
	}

}