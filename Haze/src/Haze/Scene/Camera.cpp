#include "hzpch.h"
#include "Camera.h"

#include "Haze/KeyCodes.h"
#include "Haze/Input.h"
#include "Haze/Application.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Haze
{

	Camera::Camera() :
		_Pitch(0.0f),
		_Yaw(-90.0f),
		_WorldPosition(0.0f, 0.0f, 0.0f),
		_Direction(0.0f, 0.0f, -1.0f),
		_UpVector(0.0f, 1.0f, 0.0f),
		_WorldUpVector(0.0f, 1.0f, 0.0f)
	{
		_Projection = glm::perspective(glm::radians(45.0f), ((float)Application::Get().GetWindow().GetWidth()) / ((float)Application::Get().GetWindow().GetHeight()), 0.1f, 100.0f);
		UpdateMatrices();
	}
	
	void Camera::Move(float x, float y, float z) 
	{
		_WorldPosition += z * _Direction + y * _UpVector + x * _RightVector;
		UpdateMatrices();
	}

	void Camera::Look(float y, float p) 
	{
		_Yaw += y;
		_Pitch = (_Pitch + p) > 89.0f ? 89.0f : ((_Pitch + p) < -89.0f ? -89.0f : (_Pitch + p));
		UpdateMatrices();
	}

	void Camera::UpdateMatrices() 
	{
		_Direction = glm::normalize(glm::vec3(cos(glm::radians(_Yaw)) * cos(glm::radians(_Pitch)), sin(glm::radians(_Pitch)), sin(glm::radians(_Yaw)) * cos(glm::radians(_Pitch))));
		_RightVector = glm::normalize(glm::cross(_Direction, _WorldUpVector));
		_UpVector = glm::normalize(glm::cross(_RightVector, _Direction));

		_View = glm::lookAt(_WorldPosition, _WorldPosition + _Direction, _UpVector);
	}

}