#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
	GLfloat radius = 3.0f;
	GLfloat angle = 0.0f;
	glm::vec3 cameraPos = glm::vec3(3.0f, 0.0f, 4.0f); // polozenie kamery
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); // na co patrzy kamera
	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget); // wektor mowiacy o kierunku, ale skierowany do kamery
	glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection)); // wektor reprezentujacy os x w przestrzeni kamery
	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight); // wektor reprezentujacy os y w przestrzeni kamery

	GLfloat fov = 45.0f; // poczatkowa wartosc zoomu
public:
	Camera();
	void move(GLfloat, bool*);
	void updateCameraUp();
	glm::vec3 getCameraPos();
	glm::vec3 getCameraTarget();
	glm::vec3 getCameraUp();
	GLfloat getFov();
	~Camera();
	
};

