#include <GL/glew.h> // include GLEW and new version of GL on Windows
#define GLFW_DLL
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <streambuf>

std::string readTextFile(const std::string relativePath)
{
	// File input stream
	std::ifstream file(relativePath);

	// Use string constructor to read file into string
	std::string fileAsString((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());

	return fileAsString;
}

bool loadAndCompileShader(const std::string relativePath, const GLenum type, GLuint *shader)
{
	// Load file
	std::string file = readTextFile(relativePath);
	if (!file.compare(std::string(""))){
		std::cerr << "ERROR: Failed to load " << relativePath << " into string" << std::endl;
		return false;
	}
	const char * fileContents = file.c_str();

	// Compile shader
	*shader = glCreateShader(type);
	glShaderSource(*shader, 1, &fileContents, NULL);
	glCompileShader(*shader);

	// TODO: Check for errors
	return true;
}

int main() 
{
	// start GL context and O/S window using the GLFW helper library
	if (!glfwInit()) 
	{
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(640, 640, "Hello Triangle", NULL, NULL);
	if (!window)
	{
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

	// Square Mesh
	float points[] = 
	{
		 0.5f,  0.5f,  0.0f,
		 0.5f, -0.5f,  0.0f,
		-0.5f, -0.5f,  0.0f,
		-0.5f, -0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		-0.5f,  0.5f,  0.0f
	};

	// Setup vertex array object for square
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	// Setup vertex atrtibute object for square
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// Triangle Mesh
	float points2[] =
	{
		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f
	};

	// Setup vertex array object for triangle
	GLuint vbo2 = 0;
	glGenBuffers(1, &vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points2), points2, GL_STATIC_DRAW);

	// Setup vertex atrtibute object for trinangle
	GLuint vao2 = 0;
	glGenVertexArrays(1, &vao2);
	glBindVertexArray(vao2);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// Load and compile shaders
	GLuint vs = 0;
	if (!loadAndCompileShader("test_vs.glsl", GL_VERTEX_SHADER, &vs))
	{
		std::cout << "ERROR: Failed to load and compile test_vs.glsl" << std::endl;
		return 1;
	}

	GLuint fs = 0;
	if (!loadAndCompileShader("test_fs.glsl", GL_FRAGMENT_SHADER, &fs))
	{
		std::cout << "ERROR: Failed to load and compile test_fs.glsl" << std::endl;
		return 1;
	}

	GLuint fs2 = 0;
	if (!loadAndCompileShader("test_fs2.glsl", GL_FRAGMENT_SHADER, &fs2))
	{
		std::cout << "ERROR: Failed to load and compile test_fs2.glsl" << std::endl;
		return 1;
	}

	// Create shader program, and link
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, fs);
	glAttachShader(shader_program, vs);
	glLinkProgram(shader_program);

	// Create shader program 2, and link
	GLuint shader_program2 = glCreateProgram();
	glAttachShader(shader_program2, fs2);
	glAttachShader(shader_program2, vs);
	glLinkProgram(shader_program2);


	glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Wipe the drawing surface
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Select shader program
		glUseProgram(shader_program);

		// Select vertex array
		glBindVertexArray(vao);

		// Draw 6 points starting from 0
		// from the currently bound VAO with current in-use shader
		glDrawArrays(GL_TRIANGLES, 0, 6);


		// Select shader program
		glUseProgram(shader_program2);

		// Select vertex array
		glBindVertexArray(vao2);

		// Draw 3 points starting from 0
		// from the currently bound VAO with current in-use shader
		glDrawArrays(GL_TRIANGLES, 0, 3);


		// Update other events like input handling
		glfwPollEvents();

		// Put the stuff we've been drawing onto the display
		glfwSwapBuffers(window);
	}

	// close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}