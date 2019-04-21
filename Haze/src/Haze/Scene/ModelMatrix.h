#pragma once

#include <glm/glm.hpp>

namespace Haze 
{

	class ModelMatrix {
		friend struct Interpreter;
		friend struct GUI;

		public:
			const glm::mat4& Matrix() { UpdateMatrices(); return _TempMatrix; }
			const glm::mat4& Inverse() { UpdateMatrices(); return _TempMatrixI; }

			inline const glm::vec3& GetRotation() const { return _Rotation; }
			inline const glm::vec3& GetScale() const { return _Scale; }
			inline const glm::vec3& GetPosition() const { return _Position; }

			void SetRotation(glm::vec3 rotation) { _Rotation = rotation; _Invalid = true; }
			void SetScale(glm::vec3 scale) { _Scale = scale; _Invalid = true; }
			void SetPosition(glm::vec3 position) { _Position = position; _Invalid = true; }

		private:
			void UpdateMatrices(bool force = false);

		private:
			bool _Invalid = false;

			glm::vec3 _Scale = glm::vec3(1.0f);
			glm::vec3 _Position = glm::vec3(0.0f);
			glm::vec3 _Rotation = glm::vec3(0.0f);

			glm::mat4 _TempMatrix = glm::mat4(1.0f);
			glm::mat4 _TempMatrixI = glm::mat4(1.0f);
	};


}