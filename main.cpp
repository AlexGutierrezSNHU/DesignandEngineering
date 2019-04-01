/*OpenGL Program that utilizes 3 independent vertex shaders to render and light a model from 2 seperate light sources.
Shaders are included in main.cpp file and cannot be accessed by any other class. Future updates will move shaders to
independent header file to be used by multiple models. Current enviroment to be manually manipulated, keyboard controls
to include automatic rotation, panning and zooming.*/

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>


#include "shader.h"
#include "texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL2.h>

using namespace std;

int width, height;
const double PI = 3.14159;
const float toRadians = PI / 180.0f;


// Declare Input Callback Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode);

glm::mat4 viewMatrix;

// Camera Field of View
GLfloat fov = 45.0f;

// Define default camera attributes for OpenGl context
void initiateCamera();
void AutoRotateCamera();
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraPosition - target);
glm::vec3 worldUp = glm::vec3(0.0, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
glm::vec3 CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);



// Camera Transformation Prototype
void TransformCamera();

// Boolean array for keys and mouse buttons
bool keys[1024], mouseButtons[3];

// Input state booleans
bool isPanning = false, isOrbiting = false, rotateLeft = false, rotateRight = false;

// Pitch and Yaw rates for interaction with enviroment
GLfloat radius = 3.0f, rawYaw = 0.0f, rawPitch = 0.0f, degYaw, degPitch;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat lastX = 320, lastY = 240, xChange, yChange;
bool firstMouseMove = true;


//light source positions in environment to light model
glm::vec3 lightPosition(0.0f, 1.0f, 1.0f);
glm::vec3 lightPositionLow(-1.0f, 0.0f, 0.0f);


//Method will draw triangle primitives to render model in enviroment.
void draw()
{
	GLenum mode = GL_TRIANGLES;
	GLsizei indices = 500;
	glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);
}

// Main function will create a new OpenGL, check for required libraries and initialize mouse and keyboard controls to interact 
// with openGL environment.
int main(void)
{
	width = 640; height = 480;
	GLFWwindow* window;

	// Initialize the GLFW library
	if (!glfwInit())
		std::cout << "Failed to Open GLFW library" << std::endl;
		return EXIT_FAILURE;

	/* Create a new mode window and assign OpenGL context */
	window = glfwCreateWindow(width, height, "MODEL SCENE", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Set input callback functions and make the window's context current
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwMakeContextCurrent(window);

	// Initialize GLEW framework
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}
	// Declare View Matrix
	glm::mat4 viewMatrix;


	// Define element indices for lamp/light source in environment
	GLfloat lampVertices[] =
	{
		0.0, 0.4, 0.0, 	// index 0
		-0.3, 0.0, -0.3, // index 1
		0.3, 0.0, -0.3, // index 2	
		0.0, 0.0, 0.0  	// index 3	
	};

	/* Vertex index mapping for complete chair model in environment*/

	GLfloat vertices[] =
	{

		// Vertex locations, color, UV map and Light reflection coordinates for Leg Top
		-0.275, -0.05, -0.250, // index 0
		1.0, 0.0, 0.0, 		// red
		1.0, 0.5, 			// UV (mid top)
		0.0f, 1.0f, 1.0f,  	// +Z normal 

		-0.225, -0.05, -0.250, 	// index 1
		0.0, 1.0, 0.0, 		// green
		0.0, 0.0, 			//UV bl
		0.0f, 0.0f, 1.0f,	// +Z normal

		-0.225, -0.05, -0.200, 	// index 2	
		0.0, 0.0, 1.0, 		// blue
		0.0, 1.0, 			// UV bl
		0.0f, 0.0f, 1.0f,	// +Z normal


		-0.275, -0.05, -0.200,  // index 3	
		1.0, 0.0, 1.0, 		// purple
		0.5, 0.5, 			// UV mc
		-1.0f, -1.0f, 1.0f,	// -Z normal


			// Vertex locations, color, UV map and Light reflection coordinates for Leg Bottom
		-0.275, -0.4, -0.250, // index 4
		1.0, 0.0, 0.0, 		// red
		1.0, 0.5, 			// UV (mid top)
		0.0f, 1.0f, 1.0f,  	// +Z normal 

		-0.225, -0.4, -0.250, 	// index 5
		0.0, 1.0, 0.0, 		// green
		0.0, 0.0, 			//UV bl
		0.0f, 0.0f, 1.0f,	// +Z normal

		-0.225, -0.4, -0.200, 	// index 6	
		0.0, 0.0, 1.0, 		// blue
		0.0, 1.0, 			// UV bl
		0.0f, 0.0f, 1.0f,	// +Z normal


		-0.275, -0.4, -0.200,  // index 7	
		1.0, 0.0, 1.0, 		// purple
		0.5, 0.5, 			// UV mc
		-1.0f, -1.0f, 1.0f,	// -Z normal

		//Vertex locations, color, UV map and Light reflection coordinates for Seat Rail Top
		0.225, -0.05, -0.225, // index 8
		1.0, 0.0, 0.0, 		// red
		1.0, 0.5, 			// UV (mid top)
		0.0f, 1.0f, 1.0f,  	// +Z normal 

		0.225, -0.05, -0.200, 	// index 9
		0.0, 1.0, 0.0, 		// green
		0.0, 0.0, 			//UV bl
		0.0f, 0.0f, 1.0f,	// +Z normal



		//Vertex locations, color, UV map and Light reflection coordinates for Seat Rail Bottom 
		-0.225, -0.1, -0.250, // index 10
		1.0, 0.0, 0.0, 		// red
		1.0, 0.5, 			// UV (mid top)
		0.0f, 1.0f, 1.0f,  	// +Z normal 

		0.255, -0.1, -0.250, 	// index 11
		0.0, 1.0, 0.0, 		// green
		0.0, 0.0, 			//UV bl
		0.0f, 0.0f, 1.0f,	// +Z normal

		0.225, -0.1, -0.200, // index 12
		1.0, 0.0, 0.0, 		// red
		1.0, 0.5, 			// UV (mid top)
		0.0f, 1.0f, 1.0f,  	// +Z normal 

		-0.225, -0.1, -0.200, 	// index 13
		0.0, 1.0, 0.0, 		// green
		0.0, 0.0, 			//UV bl
		0.0f, 0.0f, 1.0f	// +Z normal


	};

	//Vertex locations, color, UV map and Light reflection coordinates for back  vertices
	GLfloat backVertices[] = {

		-0.116, 0.0, 0.250, 	// index 0
		0.0, 1.0, 0.0, 		// green
		0.0, 0.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		0.116, 0.0, 0.250, 	// index 1
		0.0, 1.0, 0.0, 		// green
		1.0, 0.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		0.116, 0.0, 0.30, 	// index 2
		0.0, 1.0, 0.0, 		// green
		1.0, 1.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		-0.116, 0.0, 0.30, 	// index 3
		0.0, 1.0, 0.0, 		// green
		0.0, 1.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		-0.116, 0.30, 0.250, 	// index 4
		0.0, 1.0, 0.0, 		// green
		0.0, 0.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		0.116, 0.30, 0.250, 	// index 5
		0.0, 1.0, 0.0, 		// green
		1.0, 0.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		0.116, 0.30, 0.30, 	// index 6
		0.0, 1.0, 0.0, 		// green
		1.0, 1.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		-0.116, 0.30, 0.30, 	// index 7
		0.0, 1.0, 0.0, 		// green
		0.0, 1.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		//Vertex locations, color, UV map and Light reflection coordinates for BackRest Top

		-0.325, 0.50, 0.250, 	// index 8
		0.0, 1.0, 0.0, 		// green
		0.0, 0.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		0.325, 0.50, 0.250, 	// index 9
		0.0, 1.0, 0.0, 		// green
		1.0, 0.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		0.325, 0.50, 0.30, 	// index 10
		0.0, 1.0, 0.0, 		// green
		1.0,1.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		-0.325, 0.50, 0.30, 	// index 11
		0.0, 1.0, 0.0, 		// green
		0.0, 1.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal


		//Vertex locations, color, UV map and Light reflection coordinates for backRest Bottom

		-0.325, 0.30, 0.250, 	// index 12
		0.0, 1.0, 0.0, 		// green
		0.0, 0.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		0.325, 0.30, 0.250, 	// index 13
		0.0, 1.0, 0.0, 		// green
		1.0, 0.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		0.325, 0.30, 0.30, 	// index 14
		0.0, 1.0, 0.0, 		// green
		1.0, 1.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		-0.325, 0.30, 0.30, 	// index 15
		0.0, 1.0, 0.0, 		// green
		0.0, 1.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

	};

	//Vertex locations, color, UV map and Light reflection coordinates for Seat vertices
	GLfloat seatVertices[] =
	{
		-0.325, 0.0, -0.3, 	// index 0
		0.0, 1.0, 0.0, 		// green
		0.0, 0.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		0.325, 0.0, -0.3, 	// index 1
		0.0, 1.0, 0.0, 		// green
		1.0, 0.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		0.325, 0.0, 0.35, 	// index 2
		0.0, 1.0, 0.0, 		// green
		1.0,1.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		-0.325, 0.0, 0.35, 	// index 3
		0.0, 1.0, 0.0, 		// green
		0.0, 1.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		-0.325, -0.05, -0.3, 	// index 4
		0.0, 1.0, 0.0, 		// green
		0.0, 0.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		0.325, -0.05, -0.3, 	// index 5
		0.0, 1.0, 0.0, 		// green
		1.0, 0.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		0.325, -0.05, 0.35, 	// index 6
		0.0, 1.0, 0.0, 		// green
		1.0, 1.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal

		-0.325, -0.05, 0.35, 	// index 7
		0.0, 1.0, 0.0, 		// green
		0.0, 1.0, 			//UV bl
		0.0f, 0.0f, 1.0f,  // +Z normal
	};

	// Define element indices of seat rails for model in enviroment
	GLubyte indices[] = {
		0, 1, 2,
		2, 3, 0,
		4, 5, 6,
		6, 7, 4,
		0, 4, 1,
		1, 4, 5,
		1, 5, 2,
		2, 5, 6,
		2, 5, 7,
		3, 6, 7,
		3, 7, 1,
		0, 4, 3,
		1, 2, 8,
		2, 8, 9,
		10, 11, 12,
		10, 12, 13,
		1, 8, 10,
		8, 10,11,
		2, 13, 9,
		11, 14 ,15
	};

	// Define indices of chair model in enviroment
	GLubyte backIndices[] = {
		0, 1, 2,
		2, 3, 0,
		4, 5, 6,
		6, 7, 4,
		0, 4, 1,
		1, 4, 5,
		1, 5, 2,
		2, 5, 6,
		2, 6, 7,
		2, 3, 7,
		3, 7, 1,
		0, 4, 3,
		8, 9, 10,
		8, 10, 11,
		12, 13, 14,
		12, 14, 15,
		8, 9, 12,
		9, 12, 13,
		10, 14, 15,
		10 ,11 ,15,
		9, 13, 14,
		9 ,10 ,14,
		8, 11, 15,
		8 ,12 ,15
	};

	//Define Indices of seat model in enviroment
	GLubyte seatIndices[] = {
		0, 1, 2,
		2, 3, 0,
		4, 5, 6,
		4, 6, 7,
		0, 4, 1,
		1, 4, 5,
		1, 5, 2,
		2, 5, 6,
		2, 3, 7,
		2, 6, 7,
		0, 3, 7,
		0, 4, 7
	};

	// Plane Transforms
	glm::vec3 planePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(0.0f,  0.0f,  0.0f),

	};

	//Rotates planes to reduce vertices needed for render symetrical/duplicate model features in enviroment
	glm::float32 planeRotations[] = {
		0.0f, 90.0f, 180.0f, -90.0f
	};



	// Initialize OpenGL buffers for visual representations of model and lightsource in enviroment
	glEnable(GL_DEPTH_TEST);


	/*Visual representation of model in environment */

		//Define Model VAO
	GLuint chairVBO, chairEBO, chairVAO;
	glGenVertexArrays(1, &chairVAO);
	glGenBuffers(1, &chairVBO);
	glGenBuffers(1, &chairEBO);
	glBindVertexArray(chairVAO);
	glBindBuffer(GL_ARRAY_BUFFER, chairVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chairEBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Specify attribute location and layout to GPU
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glBindVertexArray(0);

	//Define back VAO
	GLuint backVBO, backEBO, backVAO;
	glGenVertexArrays(1, &backVAO);
	glGenBuffers(1, &backVBO);
	glGenBuffers(1, &backEBO);
	glBindVertexArray(backVAO);
	glBindBuffer(GL_ARRAY_BUFFER, backVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backEBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(backVertices), backVertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(backIndices), backIndices, GL_STATIC_DRAW);

	// Specify attribute location and layout to GPU
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glBindVertexArray(0);


	//Define seat VAO
	GLuint seatVBO, seatEBO, seatVAO;
	glGenVertexArrays(1, &seatVAO);
	glGenBuffers(1, &seatVBO);
	glGenBuffers(1, &seatEBO);
	glBindVertexArray(seatVAO);
	glBindBuffer(GL_ARRAY_BUFFER, seatVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, seatEBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(seatVertices), seatVertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(backIndices), seatIndices, GL_STATIC_DRAW);

	// Specify attribute location and layout to GPU 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glBindVertexArray(0);

	/* Visual Representation of lamp/light source in enviroment*/

	//Define Lamp VAO 
	GLuint lampVBO, lampEBO, lampVAO;
	glGenVertexArrays(1, &lampVAO);
	glGenBuffers(1, &lampVBO);
	glGenBuffers(1, &lampEBO);
	glBindVertexArray(lampVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lampVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lampEBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lampVertices), lampVertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	//Load texture from project source file
	GLuint woodTexture = TextureLoading::LoadTexture("res/images/wood.png");
	GLuint fabricTexture = TextureLoading::LoadTexture("res/images/fabric.png ");

	// Creating Shader Program
	Shader lamp("res/shaders/lamp.vs", "res/shaders/lamp.frag");
	Shader model("res/shaders/model.vs", "res/shaders/model.frag");


		// Loop will continue to update enviroment until the user closes the window. 
		while (!glfwWindowShouldClose(window))
		{
			// Set frame time
			GLfloat currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			// Resize window and graphics simultaneously
			glfwGetFramebufferSize(window, &width, &height);
			glViewport(0, 0, width, height);

			//Renders OpenGl graphic
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			// Call Shader per-frame when updating attributes
			model.Use( );

			// Declare transformations (can be initialized outside loop)		
			glm::mat4 projectionMatrix;

			// Define LookAt Matrix
			viewMatrix = glm::lookAt(cameraPosition, target, worldUp);

			// Define projection matrix
			projectionMatrix = glm::perspective(fov, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

			// Get matrix's uniform location and set matrix
			GLint modelLoc = glGetUniformLocation(model.Program, "model");
			GLint viewLoc = glGetUniformLocation(model.Program, "view");
			GLint projLoc = glGetUniformLocation(model.Program, "projection");

			//Get light position and object color location
			GLint objectColorLoc = glGetUniformLocation(model.Program, "objectColor");
			GLint lightColorLoc = glGetUniformLocation(model.Program, "lightColor");
			GLint lightPosLoc = glGetUniformLocation(model.Program, "lightPos");
			GLint viewPosLoc = glGetUniformLocation(model.Program, "viewPos");

			//Assign light and Object Colors
			glUniform3f(objectColorLoc, 0.46f, 0.36f, 0.25f);
			glUniform3f(lightColorLoc, lightPosition.x, lightPosition.y, lightPosition.z);

			//Set Lamp/light source Position in enviroment
			glUniform3f(lightPosLoc, 1.0f, 1.0f, 1.0f);
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

			//Bind texture map to vertex array
			glBindTexture(GL_TEXTURE_2D, woodTexture);
			glBindVertexArray(chairVAO);

			// Transform planes to form chair
			for (GLuint i = 0; i < 4; i++)
			{
				glm::mat4 modelMatrix;
				modelMatrix = glm::translate(modelMatrix, planePositions[i]);
				modelMatrix = glm::rotate(modelMatrix, planeRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
				// Draw primitive(s)
				draw();
			}

			// Unbind Shader exe and VOA after drawing per frame
			glBindVertexArray(0);

			//Bind Texture, Vertex Array to Model matrix  and render
			glBindTexture(GL_TEXTURE_2D, woodTexture);
			glBindVertexArray(backVAO);
			glm::mat4 modelMatrix;
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			draw();

			// Unbind Shader exe and VOA after drawing per frame
			glBindVertexArray(0);

			//Bind Texture, Vertex Array to Model matrix  and render
			glBindTexture(GL_TEXTURE_2D, fabricTexture);
			glBindVertexArray(seatVAO);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			draw();

			// Unbind Shader and VOA after rendering frame
			// Initialize default VAO and shader program
			glBindVertexArray(0);
			glUseProgram(0);

			// Call LampShader program
			lamp.Use ();

			// Get matrix's uniform location and set matrix
			GLint lampModelLoc = glGetUniformLocation(lamp.Program, "model");
			GLint lampViewLoc = glGetUniformLocation(lamp.Program, "view");
			GLint lampProjLoc = glGetUniformLocation(lamp.Program, "projection");
			glUniformMatrix4fv(lampViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(lampProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

			glBindVertexArray(lampVAO);

			// Loop will transform and render a visual representation of light source in enviroment
			for (GLuint i = 0; i < 4; i++)
			{
				glm::mat4 modelMatrix;
				modelMatrix = glm::translate(modelMatrix, planePositions[i] / glm::vec3(8.0, 8.0, 8.0) + lightPosition);
				modelMatrix = glm::rotate(modelMatrix, planeRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(.125f, .125f, .125f));


				glUniformMatrix4fv(lampModelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

				// Draw primitive(s)
				draw();
			}

			// Unbind Shader and VOA after rendering frame
			// Initialize default VAO and shader program
			glBindVertexArray(0);
			glUseProgram(0);


			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();

			/*Poll Camera Transformations*/
			TransformCamera();

		}

	//Clear GPU resources and sets buffers to default state. 
	glDeleteVertexArrays(1, &chairVAO);
	glDeleteBuffers(1, &chairVBO);
	glDeleteBuffers(1, &chairEBO);
	glDeleteVertexArrays(1, &backVAO);
	glDeleteBuffers(1, &backVBO);
	glDeleteBuffers(1, &backEBO);

	//Clear memory
	glfwTerminate();
	return 0;
}

// Define and initialized ASCII keyboard input functions to control enviroment.  
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// Close window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// Assign true to Element ASCII if key pressed
	// Assign false to Element ASCII if key released
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;

}

// Function will set starting parameters for camera location and field of View.  
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

	// Clamp FOV
	if (fov >= 1.0f && fov <= 55.0f)
		fov -= yoffset * 0.01;

	// Default FOV
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 55.0f)
		fov = 55.0f;

}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	if (firstMouseMove)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouseMove = false;
	}
	// Calculate mouse offset (Easing effect)
	xChange = xpos - lastX;
	yChange = lastY - ypos;

	// Get current mouse (always starts at 0)
	lastX = xpos;
	lastY = ypos;


	if (isOrbiting)
	{
		// Updates raw yaw and pitch with mouse movement
		rawYaw += xChange;
		rawPitch += yChange;

		// Converts yaw and pitch to degrees, and clamp pitch
		degYaw = glm::radians(rawYaw);
		degPitch = glm::clamp(glm::radians(rawPitch), -glm::pi<float>() / 2.f + .1f, glm::pi<float>() / 2.f - .1f);

		// Azimuth Altitude formula
		cameraPosition.x = target.x + radius * cosf(degPitch) * sinf(degYaw);
		cameraPosition.y = target.y + radius * sinf(degPitch);
		cameraPosition.z = target.z + radius * cosf(degPitch) * cosf(degYaw);
	}
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode)
{
	// Assign boolean state to element Button code to allow mouse control in environment
	if (action == GLFW_PRESS)
		mouseButtons[button] = true;
	else if (action == GLFW_RELEASE)
		mouseButtons[button] = false;
}



// Define TransformCamera function
// Camera Orbit and Camera Focus
void TransformCamera()
{	// Orbit camera
	if (keys[GLFW_KEY_LEFT_ALT] && mouseButtons[GLFW_MOUSE_BUTTON_LEFT])
		isOrbiting = true;
	else
		isOrbiting = false;

	// Focus camera
	if (keys[GLFW_KEY_F])
		initiateCamera();

	// Rotate cameras
	else if (keys[GLFW_KEY_RIGHT]) {
		rotateRight = true;
		AutoRotateCamera();

	}
	else if (keys[GLFW_KEY_LEFT])
		rotateLeft = true;
		AutoRotateCamera();

};

// Rotate the Camera with Keyboard Direction Input
void AutoRotateCamera()
{
	if (rotateLeft = true)
		glRotatef(-45, 0, 1, 0);

	else if (rotateRight = true)
		glRotatef(45, 0, 1, 0);

}


void initiateCamera()
{	// Define Camera Attributes
	// Camera to move 3 units back along Z axis and be 1 unit from "lens" 
	cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
	cameraDirection = glm::normalize(cameraPosition - cameraDirection);
	worldUp = glm::vec3(0.0, 1.0f, 0.0f);
	cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
	cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
	CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
}
