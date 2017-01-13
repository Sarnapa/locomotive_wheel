#pragma once
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

#include <math.h>

class Wheel
{
private:
	const int bodyVerticesCount;
	const int baseVerticesCount;
	GLfloat* cylinderBodyVertices;
	GLfloat* cylinderBaseVertices;

	void makeCylinderBodyVertices();
	void makeCylinderBaseVertices();

public:
	Wheel(const int, const int);
	~Wheel();
	GLfloat* getCylinderBodyVertices();
	GLfloat* getCylinderBaseVertices();
	void printCylinderBodyVertices();
	void printCylinderBaseVertices();
};

