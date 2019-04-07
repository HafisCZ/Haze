#pragma once

#include "Haze/Core.h"
#include "Haze/Events/ApplicationEvent.h"
#include "Haze/Events/MouseEvent.h"
#include "Haze/Events/KeyEvent.h"

#include <glm/glm.hpp>

namespace Haze 
{

	class HAZE_API Camera 
	{
		friend class RendererLayer;

		public:
			Camera();

			inline const glm::mat4& GetProjectionMatrix() const { return _Projection; }
			inline const glm::mat4& GetViewMatrix() const { return _View; }

			inline const glm::vec3& GetWorldPosition() const { return _WorldPosition; }
			inline const glm::vec3& GetDirection() const { return _Direction; }

			void Move(float x, float y, float z);
			void Look(float y, float p);

		private:
			void UpdateMatrices();

		private:
			glm::mat4 _Projection;
			glm::mat4 _View;

			glm::vec3 _WorldPosition;
			glm::vec3 _Direction;

			glm::vec3 _RightVector;
			glm::vec3 _UpVector;
			glm::vec3 _WorldUpVector;

			float _Pitch;
			float _Yaw;
	};

}