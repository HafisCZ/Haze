#include "hzpch.h"
#include "Camera.h"

#include "Haze/KeyCodes.h"
#include "Haze/Input.h"
#include "Haze/Application.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

namespace Haze
{

	Camera::Camera() :
		_Pitch(0.0f),
		_Yaw(-90.0f),
		_MoveSpeed(0.4f),
		_LookSpeed(0.2f),
		_WorldPosition(0.0f, 0.0f, 0.0f),
		_Direction(0.0f, 0.0f, -1.0f),
		_UpVector(0.0f, 1.0f, 0.0f),
		_WorldUpVector(0.0f, 1.0f, 0.0f),
		_Viewport(0.0f, 0.0f, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight())
	{
		_Projection = glm::perspective(glm::radians(45.0f), _Viewport.z / _Viewport.w, 0.1f, 100.0f);
		UpdateMatrices();
	}
	
	void Camera::Move(float x, float y, float z) 
	{
		_WorldPosition += _MoveSpeed * (z * _Direction + y * _UpVector + x * _RightVector);
		UpdateMatrices();
	}

	void Camera::Look(float y, float p) 
	{
		y *= _LookSpeed;
		p *= _LookSpeed;

		_Yaw += y;
		_Pitch = fmin(89.0f, fmax(-89.0f, _Pitch + p));
		UpdateMatrices();
	}

	void Camera::Set(float x, float y, float z, float yaw, float pitch) 
	{
		_WorldPosition = { x, y, z };
		_Yaw = yaw;
		_Pitch = fmin(89.0f, fmax(-89.0f, pitch));

		UpdateMatrices();
	}

	void Camera::SetSpeed(float move, float look) 
	{
		_MoveSpeed = move;
		_LookSpeed = look;
	}

	void Camera::UpdateMatrices() 
	{
		_Direction = glm::normalize(glm::vec3(cos(glm::radians(_Yaw)) * cos(glm::radians(_Pitch)), sin(glm::radians(_Pitch)), sin(glm::radians(_Yaw)) * cos(glm::radians(_Pitch))));
		_RightVector = glm::normalize(glm::cross(_Direction, _WorldUpVector));
		_UpVector = glm::normalize(glm::cross(_RightVector, _Direction));

		_View = glm::lookAt(_WorldPosition, _WorldPosition + _Direction, _UpVector);
	}

	std::pair<glm::vec3, unsigned int> Camera::GetWorldPointer() 
	{
		unsigned int index;
		float depth;

		glReadPixels(_Viewport.z / 2.0f, _Viewport.w / 2.0f, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
		glReadPixels(_Viewport.z / 2.0f, _Viewport.w / 2.0f, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);
		
		return { glm::unProject(glm::vec3(_Viewport.z / 2.0f, _Viewport.w / 2.0f, depth), _View, _Projection, _Viewport), index };
	}

	void Camera::OnWindowResizeEvent(WindowResizeEvent& event)
	{
		_Viewport = glm::vec4(0.0f, 0.0f, event.GetWidth(), event.GetHeight());
		_Projection = glm::perspective(glm::radians(45.0f), _Viewport.z / _Viewport.w, 0.1f, 100.0f);
		UpdateMatrices();
	}

}