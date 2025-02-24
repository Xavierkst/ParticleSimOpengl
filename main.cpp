#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

// Values / Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HT = 600;

const char* vertShaderSrc = "#version 450 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"out vec4 vertexColor;"
	"void main()\n"
	"{\n"
	"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"	vertexColor = vec4(0.5, 0.0, 0.0, 1.0);\n"
	"}\0";

const char* fragShaderSrc = "#version 450 core\n"
	"out vec4 FragColor;\n"
	"// in vec4 vertexColor;\n"
	"uniform vec4 ourColor;\n"
	"void main()\n"
	"{\n"
	"   // FragColor = vertexColor;\n"
	"   FragColor = ourColor;\n"
	"}\0";

unsigned int indices1[] = {  // note that we start from 0!
    0, 1, 2,   // first triangle
};  

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HT, "GL WINDOW", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create window!" << std::endl;
		glfwTerminate();

		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;

		return -1;
	}
	
	// create the viewport--first 2 params set 
	// the loc of the lower left hand of the object
	glViewport(0, 0, SCR_WIDTH, SCR_HT);
	
	// ******************************************************
	// ** Set callbacks for when gl window resizes
	// ******************************************************
	// Register the function as a new callback to be triggered when buffer 
	// size changes
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Compile the vert and frag shaders

	GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, 1, &vertShaderSrc, NULL);
	glCompileShader(vshader);

	int success;
	char infoLog[512];
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vshader, 512 * sizeof(char), NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, &fragShaderSrc, NULL);
	glCompileShader(fshader);

	glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fshader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Create shader program and link it with compiled 
	// vert & frag shaders
	unsigned int shaderProg = glCreateProgram();
	glAttachShader(shaderProg, vshader);
	glAttachShader(shaderProg, fshader);
	glLinkProgram(shaderProg);
	glGetProgramiv(shaderProg, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProg, 512, NULL, infoLog);
		std::cout << "SHADER PROGRAM FAILED TO COMPILE\n" << infoLog << std::endl;
	}

	// another nice to have: unbind the curr bound VAO, tho not
	// needed since binding another VAO will alr unbind the last 
	// VAO for you.
	// Note: VAO stores all binds for buffers that you bind 
	// (eg. VBO, EBO), as well as all unbind up to the point
	// where you unbind the VAO. So make sure you don't unbind a
	// EBO b4 you unbind the VAO. If not, it won't store the 
	// EBO in it!

	float verticesAll[] = {
		 0.0f,  0.5f, 0.0f,  // top 
		-0.5f, -0.5f, 0.0f,  // left
		 0.5f, -0.5f, 0.0f,  // right
	};

	// create shader object, compile shader code, attach compiled shader objects to shader program, 
	// link all attached code for a given program
	unsigned int fShader2, shaderProg2;
	fShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader2, 1, &fragShaderSrc, NULL);
	glCompileShader(fShader2);
	// glCreateshaderProgram
	// glCreateShaderProgramv()
	shaderProg2 = glCreateProgram();
	glAttachShader(shaderProg2, vshader);
	glAttachShader(shaderProg2, fShader2);
	glLinkProgram(shaderProg2);

	// after linking frag/vert shader executables to prog, 
	// you can delete them
	glDeleteShader(vshader);
	glDeleteShader(fShader2);

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// set up 1st VAO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAll), verticesAll, GL_STATIC_DRAW);;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // this is locIndex in the vert shader (set to 0)

	// Unbind VAO 
	// calling glVertexAttribPtr alr binds VBO_1 as the corresp vbo for VAO_1
	// it sort of "locks it in". So we can unbind any curr bound VBO right after
	glBindVertexArray(0);

	// Set OGL to render objects in wireframe mode:
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// To unset this with another cmd, use:
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	int ourColorLoc = glGetUniformLocation(shaderProg, "ourColor");
	 

	// render loop
	while (!glfwWindowShouldClose(window)) {
		// Process key/mouse input commands
		processInput(window);
		// fill viewport with flat color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// Render
		float t = glfwGetTime();
		float greenValue = (sin(t)/2.0f) + 0.5f;
		glUseProgram(shaderProg); // Tri1
		glUniform4f(ourColorLoc, .0f, greenValue, .0f, .0f);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*) 0);

		// Swap front pixel buffer w/ back
		glfwSwapBuffers(window);

		// Check for keys pressed, mouse moves/clicks etc.
		// and call all registered callback fns
		glfwPollEvents(); 
	}

	// De-allocating all resources when no longer in use
	glDeleteProgram(shaderProg);
	glDeleteProgram(shaderProg2);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
	// glDeleteBuffers(1, vshader);
	// pushing, popping!

	// int numAttribs;
	// glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttribs);
	// std::cout << numAttribs << std::endl;

	// De-alloc all prev allocated GLFW resources
	glfwTerminate();

	return 0;
}

// when window dimension is altered by the user, gl should immediately
// pick up on it and resize the viewport accordingly
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void bindArrBuffer(unsigned int& VAO, unsigned int& VBO, int* vertices) {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void bindAndDrawElemBuffer(unsigned int& EBO)
{
	// tempEBO holds the index for the generated buffer 
	unsigned int tempEBO;
	glGenBuffers(1, &tempEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tempEBO);
	// Send the index data over to the GPU side, where tempEBO "points" to this buffer.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);
	glDrawElements(GL_TRIANGLES, sizeof(indices1)/sizeof(GLuint), GL_UNSIGNED_INT, 0);
}


