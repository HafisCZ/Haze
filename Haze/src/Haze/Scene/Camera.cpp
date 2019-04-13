#include "hzpch.h"
#include "Camera.h"

#include "Haze/Application.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

namespace Haze
{

	void Camera::Reset() 
	{
		_Pitch = 0.0f;
		_Yaw = 0.0f;
		_Fov = 60.0f;
		_MoveSpeed = 0.2f;
		_LookSpeed = 0.1f;
		_WorldPosition = { 0.0f, 0.0f, 0.0f };
		_Direction = { 0.0f, 0.0f, -1.0f };
		_UpVector = { 0.0f, 0.0f, 0.0f };
		_WorldUpVector = { 0.0f, 1.0f, 0.0f };

		Window& window = Application::Get().GetWindow();
		_Viewport = { 0.0f, 0.0f, window.GetWidth(), window.GetHeight() };

		_Projection = glm::perspective(glm::radians(_Fov), _Viewport.z / _Viewport.w, 0.1f, 100.0f);

		UpdateMatrices();
	}
	
	void Camera::Move(float x, float y, float z, bool worldY) 
	{
		if (worldY)
		{
			glm::vec3 direction = z * _Direction + x * _RightVector;
			direction.y = 0.0f;

			_WorldPosition += _MoveSpeed * (direction + y * _WorldUpVector);
		} 
		else 
		{
			_WorldPosition += _MoveSpeed * (z * _Direction + y * _UpVector + x * _RightVector);
		}

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

	void Camera::Set(float x, float y, float z) 
	{
		_WorldPosition = { x, y, z };

		UpdateMatrices();
	}

	void Camera::Set(float yaw, float pitch)
	{
		_Yaw = yaw;
		_Pitch = fmin(89.0f, fmax(-89.0f, pitch));

		UpdateMatrices();
	}

	void Camera::SetSpeed(float move, float look) 
	{
		_MoveSpeed = move;
		_LookSpeed = look;
	}

	void Camera::SetFov(float fov) 
	{
		_Fov = fov;
		_Projection = glm::perspective(glm::radians(_Fov), _Viewport.z / _Viewport.w, 0.1f, 1000.0f);

		UpdateMatrices();
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
		glReadPixels((int) _Viewport.z / 2, (int) _Viewport.w / 2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
		glReadPixels((int)_Viewport.z / 2, (int) _Viewport.w / 2, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);
		
		return { glm::unProject(glm::vec3(_Viewport.z / 2.0f, _Viewport.w / 2.0f, depth), _View, _Projection, _Viewport), index };
	}

	void Camera::OnWindowResizeEvent(WindowResizeEvent& event)
	{
		_Viewport = glm::vec4(0.0f, 0.0f, event.GetWidth(), event.GetHeight());
		_Projection = glm::perspective(glm::radians(_Fov), _Viewport.z / _Viewport.w, 0.1f, 1000.0f);

		UpdateMatrices();
	}

}