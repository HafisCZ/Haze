#pragma once

#include "Haze/Core.h"
#include "Haze/Events/ApplicationEvent.h"

#include "Haze/ImGui/Presets.h"
#include "Haze/Scripts/Interpreter.h"

#include <glm/glm.hpp>

namespace Haze 
{

	class HAZE_API Camera
	{
		friend class RendererLayer;
		friend struct Interpreter;
		friend struct GUI;

		public:
			Camera() { Reset(); }

			inline const glm::mat4& GetProjectionMatrix() const { return _Projection; }
			inline const glm::mat4& GetViewMatrix() const { return _View; }

			inline const glm::vec3& GetWorldPosition() const { return _WorldPosition; }
			inline const glm::vec3& GetDirection() const { return _Direction; }

			std::pair<glm::vec3, unsigned int> GetWorldPointer();

			void Move(float x, float y, float z, bool worldY = false);
			void Look(float y, float p);

			void SetSpeed(float move, float look);
			
			void Set(float x, float y, float z, float yaw, float pitch);
			void Set(float x, float y, float z);
			void Set(float yaw, float pitch);

			void SetFov(float fov);

			void Reset();

		private:
			void UpdateMatrices();
			
			void OnWindowResizeEvent(WindowResizeEvent& event);

		private:
			glm::mat4 _Projection;
			glm::mat4 _View;

			glm::vec3 _WorldPosition;
			glm::vec3 _Direction;

			glm::vec3 _RightVector;
			glm::vec3 _UpVector;
			glm::vec3 _WorldUpVector;

			const glm::vec3 _WUpVector = glm::vec3(0.0f, 1.0f, 0.0f);

			glm::vec4 _Viewport;

			float _Pitch;
			float _Yaw;
			float _Fov;

			float _MoveSpeed;
			float _LookSpeed;
	};

}