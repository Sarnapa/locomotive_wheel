#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM - do matematyki
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL.h>

#include <iostream>

#include "shprogram.h"
#include "Wheel.h"
#include "Camera.h"

#define WHEELS_COUNT 4
#define CYLINDER_BODY_VERTICES_COUNT 60 // liczba wierzcholkow do stworzenia pow bocznej walca
#define CYLINDER_BASE_VERTICES_COUNT 60 // liczba wierzcholkow do stworzenia podstawy walca

using namespace std;

const GLuint WIDTH = 1024, HEIGHT = 768;

Camera cam;
bool keys[1024]; // tabela przetrzymujaca info czy przycisk jest wcisniety
glm::vec3 lightDiffuse(0.8f, 0.8f, 0.8f); // zmienna przetrzymujaca wartosc swiatla rozproszonego

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	cout << key << endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	// to umozliwia zeby kamera przesuwala sie w obydwoch kierunkach w tym samym czasie
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void lightDiffuseUpdate(bool* keys)
{
	if (keys[GLFW_KEY_UP])
	{
		if (lightDiffuse.x < 1.0f) // sprawdzamy tylko 1 wspolrzedna bo wszystkie sa rowne 
		{
			lightDiffuse.x += 0.01f;
			lightDiffuse.y += 0.01f;
			lightDiffuse.z += 0.01f;
		}
	}
	if (keys[GLFW_KEY_DOWN])
	{
		if (lightDiffuse.x > 0.0f)
		{
			lightDiffuse.x -= 0.01f;
			lightDiffuse.y -= 0.01f;
			lightDiffuse.z -= 0.01f;
		}
	}
}

// funkcja do ladowania tekstury
GLuint LoadMipmapTexture(GLuint texId, const char* fname)
{
	int width, height;
	// wczytujemy teksture - 4 argument to numer kanalu, a 5 oznacza ze interesuje nas tylko rgb
	unsigned char* image = SOIL_load_image(fname, &width, &height, 0, SOIL_LOAD_RGB); 
	if (image == nullptr)
		throw exception("Failed to load texture file");

	GLuint texture;
	glGenTextures(1, &texture);

	glActiveTexture(texId);
	glBindTexture(GL_TEXTURE_2D, texture);
	// 2 argument to poziom mipmapy (lepiej zostawic na 0), 3 - przechowujemy tylko rgb, 7 i 8 - pobieramy rgb jako unsigned
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

int main()
{
	if (glfwInit() != GL_TRUE)
	{
		cout << "GLFW initialization failed" << endl;
		return -1;
	}

	// opengl wersja 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	try
	{
		// tworzymy okno - dwa ostatnie argumenty - mniej wazne ale jesli w pierwszym ustawimy glfwGetPrimaryMonitor() to bedziemy mieli okno na caly ekran
		GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Kola lokomotywy", nullptr, nullptr);
		if (window == nullptr)
			throw exception("GLFW window not created");
		glfwMakeContextCurrent(window); // kontekst naszego okienka glownym kontekstem w aktualnym watku
		glfwSetKeyCallback(window, key_callback); // rejestrujemy ta funkcje "nasluchiwania" przycisków klawy

		glewExperimental = GL_TRUE; // wiecej nowoczesnych technik do opengl
		if (glewInit() != GLEW_OK)
			throw exception("GLEW Initialization failed");

		glViewport(0, 0, WIDTH, HEIGHT); // okno renderingu

		glEnable(GL_DEPTH_TEST); // wlaczamy z-bufor

		// tworzymy nasz program shadera
		ShaderProgram ourShader("shader.vert", "shader.frag");
		ShaderProgram lightShader("lightShader.vert", "lightShader.frag");

		glm::vec3 lightPos(8.0f, 5.0f, 8.0f);

		GLfloat lampVertices[] = {
			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			-0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,

			-0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,

			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,

			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, -0.5f
		};

		GLfloat groundVertex[] =
		{
			-1.0f, -1.0f, -0.25f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			1.0f, -1.0f, -0.25f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, -0.25f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, -0.25f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
			-1.0f, 1.0f, -0.25f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, -0.25f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f

		};

		GLfloat rectangularPrismVertex[] = {
			// 1 podstawa
			-0.75f, -2.0f, -0.1f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			0.75f, -2.0f, -0.1f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			0.75f, 2.0f, -0.1f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
			0.75f, 2.0f, -0.1f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
			-0.75f, 2.0f, -0.1f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
			-0.75f, -2.0f, -0.1f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,

			//2 podstawa
			-0.75f, -2.0f, 0.1f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			0.75f, -2.0f, 0.1f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			0.75f, 2.0f, 0.1f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
			0.75f, 2.0f, 0.1f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
			-0.75f, 2.0f, 0.1f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
			-0.75f, -2.0f, 0.1f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,

			// sciana boczna - lewo
			-0.75f, 2.0f, 0.1f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			-0.75f, 2.0f, -0.1f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			-0.75f, -2.0f, -0.1f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			-0.75f, -2.0f, -0.1f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			-0.75f, -2.0f, 0.1f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			-0.75f, 2.0f, 0.1f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

			// sciana boczna - gora
			-0.75f, 2.0f, 0.1f,  0.0f, 1.0f,  0.0f, 1.0f,  0.0f,
			0.75f, 2.0f, 0.1f,  1.0f, 1.0f,  0.0f, 1.0f,  0.0f,
			0.75f, 2.0f, -0.1f,  1.0f, 0.0f,  0.0f, 1.0f,  0.0f,
			0.75f, 2.0f, -0.1f,  1.0f, 0.0f,  0.0f, 1.0f,  0.0f,
			-0.75f, 2.0f, -0.1f,  0.0f, 0.0f, 0.0f, 1.0f,  0.0f,
			-0.75f, 2.0f, 0.1f,  0.0f, 1.0f,  0.0f, 1.0f,  0.0f,

			// sciana boczna - prawo
			0.75f, 2.0f, 0.1f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			0.75f, 2.0f, -0.1f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			0.75f, -2.0f, -0.1f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			0.75f, -2.0f, -0.1f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			0.75f, -2.0f, 0.1f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			0.75f, 2.0f, 0.1f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,

			// sciana boczna - tyl
			-0.75f, -2.0f, 0.1f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
			0.75f, -2.0f, 0.1f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
			0.75f, -2.0f, -0.1f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
			0.75f, -2.0f, -0.1f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
			-0.75f, -2.0f, -0.1f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
			-0.75f, -2.0f, 0.1f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		};

		GLfloat barVertex[] = {
			// 1 podstawa
			0.95f, 1.5f, 0.05f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			1.05f, 1.5f, 0.05f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			1.05f, -1.5f, 0.05f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
			1.05f, -1.5f, 0.05f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
			0.95f, -1.5f, 0.05f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
			0.95f, 1.5f, 0.05f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

			//2 podstawa
			0.95f, 1.5f, -0.05f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
			1.05f, 1.5f, -0.05f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
			1.05f, -1.5f, -0.05f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
			1.05f, -1.5f, -0.05f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
			0.95f, -1.5f, -0.05f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
			0.95f, 1.5f, -0.05f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

			// sciana boczna - lewo
			0.95f, 1.5f, 0.05f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
			0.95f, 1.5f, -0.05f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
			0.95f, -1.5f, -0.05f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			0.95f, -1.5f, -0.05f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			0.95f, -1.5f, 0.05f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			0.95f, 1.5f, 0.05f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

			// sciana boczna - gora
			0.95f, 1.5f, 0.05f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			1.05f, 1.5f, 0.05f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			1.05f, 1.5f, -0.05f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			1.05f, 1.5f, -0.05f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.95f, 1.5f, -0.05f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.95f, 1.5f, 0.05f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

			// sciana boczna - prawo
			1.05f, 1.5f, 0.05f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			1.05f, 1.5f, -0.05f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			1.05f, -1.5f, -0.05f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			1.05f, -1.5f, -0.05f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			1.05f, -1.5f, 0.05f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			1.05f, 1.5f, 0.05f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

			// sciana boczna - tyl
			0.95f, -1.5f, 0.05f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
			1.05f, -1.5f, 0.05f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
			1.05f, -1.5f, -0.05f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			1.05f, -1.5f, -0.05f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			0.95f, -1.5f, -0.05f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			0.95f, -1.5f, 0.05f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f
		};

		// wspolrzedne kolek w ukladzie œwiata
		glm::vec3 wheelsPositions[] = {
			glm::vec3(-0.85f, 1.5f, 0.0f),
			glm::vec3(-0.85f, -1.5f, 0.0f),
			glm::vec3(0.85f, 1.5f, 0.0f),
			glm::vec3(0.85f, -1.5f, 0.0f)
		};

		// wspolrzedne podstaw tych naszych walcow w ukladzie swiata
		glm::vec3 basesWheelsPosition[] = {
			glm::vec3(-0.75f, 1.5f, 0.0f),
			glm::vec3(-0.95f, 1.5f, 0.0f),
			glm::vec3(-0.75f, -1.5f, 0.0f),
			glm::vec3(-0.95f, -1.5f, 0.0f),
			glm::vec3(0.95f, 1.5f, 0.0f),
			glm::vec3(0.75f, 1.5f, 0.0f),
			glm::vec3(0.95f, -1.5f, 0.0f),
			glm::vec3(0.75f, -1.5f, 0.0f),
		};

		Wheel wheel(CYLINDER_BODY_VERTICES_COUNT, CYLINDER_BASE_VERTICES_COUNT);
		GLfloat bodyVertices[(CYLINDER_BODY_VERTICES_COUNT + 2) * 8];
		GLfloat* bodyVerticesPtr = wheel.getCylinderBodyVertices();
		GLfloat baseVertices[(CYLINDER_BASE_VERTICES_COUNT + 2) * 8];
		GLfloat* baseVerticesPtr = wheel.getCylinderBaseVertices();
		
		for (int i = 0; i < (CYLINDER_BODY_VERTICES_COUNT  + 2) * 8; ++i)
			bodyVertices[i] = bodyVerticesPtr[i];

		for (int i = 0; i < (CYLINDER_BASE_VERTICES_COUNT + 2) * 8; ++i)
			baseVertices[i] = baseVerticesPtr[i];
			
		// Ustawiamy rozciaganie tesktury wzdluz wspolrzednych x i y - bedzie tekstura sie powtarzac
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Ustawiamy opcje mapowania pikseli tekstury na wspolrzedne tekstury - w zaleznosci od tego czy powiekszamy teksture czy pomniejszamy
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		GLuint texture0 = LoadMipmapTexture(GL_TEXTURE0, "asphalt.jpg");
		GLuint texture1 = LoadMipmapTexture(GL_TEXTURE1, "Brushed_Titanium.jpg");
		GLuint texture2 = LoadMipmapTexture(GL_TEXTURE2, "Brushed_Silver_Steel.jpg");

		// VBO - Vertex Buffer Object, VAO - Vertex Array Object
		GLuint VBO1, VBO2, VBO3, VBO4, VBO5, VAO;
		GLuint VBOLight, VAOLight;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO1); // pow boczna walca
		glGenBuffers(1, &VBO2); // podst walca
		glGenBuffers(1, &VBO3); // prostopadloscian
		glGenBuffers(1, &VBO4); // belki
		glGenBuffers(1, &VBO5); // podloze - asfalt

		glGenVertexArrays(1, &VAOLight);
		glGenBuffers(1, &VBOLight);
		glBindVertexArray(VAOLight);
		glBindBuffer(GL_ARRAY_BUFFER, VBOLight);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lampVertices), lampVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);

		GLfloat deltaTime = 0.0f;	// czas miedzy ostatnimi klatkami
		GLfloat lastFrame = 0.0f;  	// czas ostatniej klatki

		while (!glfwWindowShouldClose(window))
		{
			// okreslamy ile czasu minelo miedzy ostatnimi klatkami
			GLfloat currentFrame = (GLfloat)glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			glfwPollEvents();
			cam.move(deltaTime, keys);
			lightDiffuseUpdate(keys);

			glClearColor(0.21f, 0.6f, 0.7f, 1.0f); // kolor
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			ourShader.Use();

			// transformacje do przeniesienia obiektu do odpowiedniego ukladu (na razie bez model bo model bedzie rozny)
			glm::mat4 view;
			glm::vec3 cameraPos = cam.getCameraPos();
			glm::vec3 cameraTarget = cam.getCameraTarget();
			glm::vec3 cameraUp = cam.getCameraUp();
			GLfloat fov = cam.getFov();
			view = glm::lookAt(cameraPos, cameraTarget , cameraUp);
			glm::mat4 projection;
			projection = glm::perspective(fov, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
			// ustawiamy uniformy dla odpowiednich macierzy
			GLint modelLoc = glGetUniformLocation(ourShader.get_programID(), "model");
			GLint viewLoc = glGetUniformLocation(ourShader.get_programID(), "view");
			GLint projLoc = glGetUniformLocation(ourShader.get_programID(), "projection");
			// przesylamy do shadera (2 argument to ile macierzy przesylamy, a 3 czy chcemy transponowac macierz)
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			
			// ustawiamy swiatlo  - 3 skladowe swiatla - Ambient, Diffuse i Specular
			GLint lightAmbientLoc = glGetUniformLocation(ourShader.get_programID(), "light.ambient");
			GLint lightDiffuseLoc = glGetUniformLocation(ourShader.get_programID(), "light.diffuse");
			GLint lightSpecularLoc = glGetUniformLocation(ourShader.get_programID(), "light.specular");
			GLint lightPosLoc = glGetUniformLocation(ourShader.get_programID(), "light.pos");
			GLint viewPosLoc = glGetUniformLocation(ourShader.get_programID(), "viewPos");
			glUniform3f(lightAmbientLoc, 0.4f, 0.4f, 0.4f);
			glUniform3f(lightDiffuseLoc, lightDiffuse.x, lightDiffuse.y, lightDiffuse.z);
			glUniform3f(lightSpecularLoc, 0.4f, 0.4f, 0.4f);
			glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
			glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

			glBindVertexArray(VAO);

			// specyfikujemy uniformy dla naszego materialu oraz nie przesylamy tekstury
			glUniform1i(glGetUniformLocation(ourShader.get_programID(), "isTexture"), false);
			GLint matAmbientLoc = glGetUniformLocation(ourShader.get_programID(), "material.ambient");
			GLint matDiffuseLoc = glGetUniformLocation(ourShader.get_programID(), "material.diffuse");
			GLint matSpecularLoc = glGetUniformLocation(ourShader.get_programID(), "material.specular");
			GLint matShineLoc = glGetUniformLocation(ourShader.get_programID(), "material.shininess");

			// jakie wartosci dostarczamy do tych uniformow
			glUniform3f(matAmbientLoc, 0.19225f, 0.19225f, 0.19225f);
			glUniform3f(matDiffuseLoc, 0.50754f, 0.50754f, 0.50754f);
			glUniform3f(matSpecularLoc, 0.508273f, 0.508273f, 0.508273f);
			glUniform1f(matShineLoc, 0.4f);

			// rysujemy powierzchnie boczne walcow
			glBindBuffer(GL_ARRAY_BUFFER, VBO1);
			glBufferData(GL_ARRAY_BUFFER, sizeof(bodyVertices), bodyVertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);
			for(GLuint i = 0; i < WHEELS_COUNT; ++i)
			{
				glm::mat4 model1;
				model1 = glm::translate(model1, wheelsPositions[i]);
				model1 = glm::rotate(model1, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				model1 = glm::rotate(model1, (GLfloat)glfwGetTime() * glm::radians(-60.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model1));
				glDrawArrays(GL_TRIANGLE_STRIP, 0, (CYLINDER_BODY_VERTICES_COUNT + 2));
			}
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glUniform1i(glGetUniformLocation(ourShader.get_programID(), "isTexture"), false);

			glUniform3f(matAmbientLoc, 0.3f, 0.3f, 0.3f);
			glUniform3f(matDiffuseLoc, 0.6f, 0.6f, 0.6f);
			glUniform3f(matSpecularLoc, 0.508273f, 0.508273f, 0.508273f);
			glUniform1f(matShineLoc, 0.4f);

			// rysujemy podstawy walcow
			glBindBuffer(GL_ARRAY_BUFFER, VBO2);
			glBufferData(GL_ARRAY_BUFFER, sizeof(baseVertices), baseVertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);
			int cylinderBaseCount = WHEELS_COUNT * 2;
			for (GLuint i = 0; i < cylinderBaseCount; ++i)
			{
				int rotationDir = i % 2 == 0 ? 1 : -1; // do dobrego zwrotu wektora normalnego
				glm::mat4 model2;
				model2 = glm::translate(model2, basesWheelsPosition[i]);
				model2 = glm::rotate(model2, glm::radians(rotationDir * 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				model2 = glm::rotate(model2, (GLfloat)glfwGetTime() * glm::radians(-60.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
				glDrawArrays(GL_TRIANGLE_FAN, 0, (CYLINDER_BASE_VERTICES_COUNT + 2));
			}
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glUniform1i(glGetUniformLocation(ourShader.get_programID(), "isTexture"), true);
			glUniform1i(glGetUniformLocation(ourShader.get_programID(), "materialTexture.diffuse"), 1);
			matSpecularLoc = glGetUniformLocation(ourShader.get_programID(), "materialTexture.specular");
			matShineLoc = glGetUniformLocation(ourShader.get_programID(), "materialTexture.shininess");
			glUniform3f(matSpecularLoc, 0.508273f, 0.508273f, 0.508273f);
			glUniform1f(matShineLoc, 0.5f);

			// rysujemy glowny prostopadloscian
			glBindBuffer(GL_ARRAY_BUFFER, VBO3);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangularPrismVertex), rectangularPrismVertex, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);
			glm::mat4 model3;
			model3 = glm::translate(model3, glm::vec3(0.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, texture2);
			glUniform1i(glGetUniformLocation(ourShader.get_programID(), "isTexture"), true);
			glUniform1i(glGetUniformLocation(ourShader.get_programID(), "materialTexture.diffuse"), 2);
			matSpecularLoc = glGetUniformLocation(ourShader.get_programID(), "materialTexture.specular");
			matShineLoc = glGetUniformLocation(ourShader.get_programID(), "materialTexture.shininess");
			glUniform3f(matSpecularLoc, 0.4f, 0.4f, 0.4f);
			glUniform1f(matShineLoc, 0.7f);

			// rysujemy belki
			glBindBuffer(GL_ARRAY_BUFFER, VBO4);
			glBufferData(GL_ARRAY_BUFFER, sizeof(barVertex), barVertex, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);
			for (int i = 0; i < 2; ++i)
			{
				glm::mat4 model4;
				GLfloat angle = (GLfloat)glfwGetTime() * glm::radians(-60.0f); // kat do obrysowania kola
				GLfloat deltaY = 0.15f * glm::cos(angle);
				GLfloat y = -0.2f + deltaY;
				GLfloat z = 0.15f * glm::sin(angle);
				GLfloat angle2 = 0.065f * glm::asin(sin(angle)); // kat nachylenia
				model4 = glm::translate(model4, glm::vec3(i * (-2.0f), 0.2f, 0.0f));
				model4 = glm::translate(model4, glm::vec3(0.0f, y, z));
				model4 = glm::translate(model4, glm::vec3(0.0f, 1.5f, 0.0f));
				model4 = glm::rotate(model4, angle2, glm::vec3(1.0f, 0.0f, 0.0f));
				model4 = glm::translate(model4, glm::vec3(0.0f, -1.5f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		
			glBindBuffer(GL_ARRAY_BUFFER, 0);
				
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture0);
			glUniform1i(glGetUniformLocation(ourShader.get_programID(), "isTexture"), true);
			glUniform1i(glGetUniformLocation(ourShader.get_programID(), "materialTexture.diffuse"), 0);
			matSpecularLoc = glGetUniformLocation(ourShader.get_programID(), "materialTexture.specular");
			matShineLoc = glGetUniformLocation(ourShader.get_programID(), "materialTexture.shininess");
			glUniform3f(matSpecularLoc, 0.2f, 0.2f, 0.2f);
			glUniform1f(matShineLoc, 0.9f);
			
			// rysujemy podloze
			glBindBuffer(GL_ARRAY_BUFFER, VBO5);
			glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertex), groundVertex, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);
			glm::mat4 model5;
			model5 = glm::scale(model5, glm::vec3(20, 20, 1));
			model5 = glm::rotate(model5, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model5));
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(0);

			lightShader.Use();

			modelLoc = glGetUniformLocation(lightShader.get_programID(), "model");
			viewLoc = glGetUniformLocation(lightShader.get_programID(), "view");
			projLoc = glGetUniformLocation(lightShader.get_programID(), "projection");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			glm::mat4 lightModel;
			lightModel = glm::translate(lightModel, lightPos);
			lightModel = glm::scale(lightModel, glm::vec3(0.2f)); // zeby bylo troche mniejsze
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(lightModel));
			
			// rysujemy zrodlo swiatla
			glBindVertexArray(VAOLight);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);

			// zamienia bufor koloru ktory zawiera wartosc koloru kazdego piksela
			glfwSwapBuffers(window);
		}

		glDeleteVertexArrays(1, &VAO);
		glDeleteVertexArrays(1, &VAOLight);
		glDeleteBuffers(1, &VBO1);
		glDeleteBuffers(1, &VBO2);
		glDeleteBuffers(1, &VBO3);
		glDeleteBuffers(1, &VBO4);
		glDeleteBuffers(1, &VBO5);
		glDeleteBuffers(1, &VBOLight);

	}
	catch (exception ex)
	{
		cout << ex.what() << endl;
	}

	glfwTerminate();
	return 0;
}