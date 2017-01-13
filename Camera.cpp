#include "Camera.h"

Camera::Camera()
{
}

void Camera::move(GLfloat deltaTime, bool* keys)
{
	GLfloat cameraSpeed = 2.0f * deltaTime;
	GLfloat newX, newY;
	if (keys[GLFW_KEY_W])
	{
		if (cameraPos.z < 4.0f)
		{
			cameraPos.z += 1.0f * cameraSpeed;
			updateCameraUp();
		}
	}
	if (keys[GLFW_KEY_S])
	{
		if (cameraPos.z > 0.0f)
		{
			cameraPos.z -= 1.0f * cameraSpeed;
			updateCameraUp();
		}
	}
	if (keys[GLFW_KEY_A])
	{
		angle -= 1.0f * cameraSpeed;
		newY = radius * sin(angle);
		newX = radius * cos(angle);
		cameraPos.x = newX;
		cameraPos.y = newY;
		updateCameraUp();
	}
	if (keys[GLFW_KEY_D])
	{
		angle += 1.0f * cameraSpeed;
		newY = radius * sin(angle);
		newX = radius * cos(angle);
		cameraPos.x = newX;
		cameraPos.y = newY;
		updateCameraUp();
	}
}

void Camera::updateCameraUp()
{
	cameraDirection = glm::normalize(cameraPos - cameraTarget);
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	cameraUp = glm::cross(cameraDirection, cameraRight);
}

glm::vec3 Camera::getCameraPos()
{
	return cameraPos;
}

glm::vec3 Camera::getCameraTarget()
{
	return cameraTarget;
}

glm::vec3 Camera::getCameraUp()
{
	return cameraUp;
}

GLfloat Camera::getFov()
{
	return fov;
}

Camera::~Camera()
{
}

