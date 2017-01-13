#include "wheel.h"

#define PI 3.141592653589793

using namespace std;

Wheel::Wheel(int _bodyVerticesCount, int _baseVerticesCount) :bodyVerticesCount(_bodyVerticesCount), baseVerticesCount(_baseVerticesCount)
{
	cylinderBodyVertices = new GLfloat[(bodyVerticesCount + 2) * 8];
	cylinderBaseVertices = new GLfloat[(baseVerticesCount + 2) * 8];
	makeCylinderBodyVertices();
	makeCylinderBaseVertices();
}

Wheel::~Wheel()
{
	delete[] cylinderBodyVertices;
	delete[] cylinderBaseVertices;

}

void Wheel::makeCylinderBodyVertices()
{
	for (int i = 0; i <= bodyVerticesCount; ++i)
	{
		const GLdouble step = 2.0 * PI / (double)(bodyVerticesCount);
		cylinderBodyVertices[8 * i] = (0.25 * cos(i * step));
		cylinderBodyVertices[8* i + 1] = (0.25 * sin((i * step)));
		cylinderBodyVertices[8 * i + 2] = (0.2 * (i % 2 == 0 ? -0.5 : 0.5));
		cylinderBodyVertices[8 * i + 3] = 0.0f;
		cylinderBodyVertices[8 * i + 4] = 0.0f;
		cylinderBodyVertices[8 * i + 5] = cos(i * step); // wzdluz promienia 
		cylinderBodyVertices[8 * i + 6] = sin(i * step); // wzdluz promienia
		cylinderBodyVertices[8 * i + 7] = 0.0f;
	}
	 //powtarzamy jeszcze na koncu poczatkowe 2 wierzcholki
     for (int i = 0; i < 16; ++i)
		cylinderBodyVertices[8 * bodyVerticesCount + i] = cylinderBodyVertices[i];
}


void Wheel::makeCylinderBaseVertices()
{
	for (int i = 0; i < baseVerticesCount; ++i)
	{
		const GLdouble step = 2.0 * PI / (double)(baseVerticesCount);
		cylinderBaseVertices[8 * i] = (0.25 * cos(i * step));
		cylinderBaseVertices[8 * i + 1] = (0.25 * sin((i * step)));
		cylinderBaseVertices[8 * i + 2] = 0;
		cylinderBaseVertices[8 * i + 3] = 0.0f;
		cylinderBaseVertices[8 * i + 4] = 0.0f;
		cylinderBaseVertices[8 * i + 5] = 0.0f;
		cylinderBaseVertices[8 * i + 6] = 0.0f;
		cylinderBaseVertices[8 * i + 7] = 1.0f;
	}
	// powtarzamy jeszcze na koncu poczatkowe 2 wierzcholki
	for (int i = 0; i < 16; ++i)
		cylinderBaseVertices[8 * baseVerticesCount + i] = cylinderBaseVertices[i];
}


GLfloat* Wheel::getCylinderBodyVertices()
{
	return cylinderBodyVertices;
}

GLfloat* Wheel::getCylinderBaseVertices()
{
	return cylinderBaseVertices;
}

void Wheel::printCylinderBodyVertices()
{
	for (int i = 0; i < (bodyVerticesCount + 2) * 8; ++i)
	{
		cout << i << " " << cylinderBodyVertices[i] << endl;
	}
}


void Wheel::printCylinderBaseVertices()
{
	for (int i = 0; i < (baseVerticesCount + 2) * 8; ++i)
	{
		cout << i << " " << cylinderBaseVertices[i] << endl;
	}
}
