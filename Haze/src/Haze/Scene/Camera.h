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
		friend void GUI::CameraWindow(bool&, Camera*);
		friend void Interpreter::InvokeCommand(Command* cmd, Scene*& scene, Camera*& camera);

		public:
			Camera();

			inline const glm::mat4& GetProjectionMatrix() const { return _Projection; }
			inline const glm::mat4& GetViewMatrix() const { return _View; }

			inline const glm::vec3& GetWorldPosition() const { return _WorldPosition; }
			inline const glm::vec3& GetDirection() const { return _Direction; }

			std::pair<glm::vec3, unsigned int> GetWorldPointer();

			void Move(float x, float y, float z);
			void Look(float y, float p);

			void SetSpeed(float move, float look);
			
			void Set(float x, float y, float z, float yaw, float pitch);

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

			glm::vec4 _Viewport;

			float _Pitch;
			float _Yaw;

			float _MoveSpeed;
			float _LookSpeed;
	};

}