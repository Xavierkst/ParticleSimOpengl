#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <iostream>
#include <filesystem>
#include "shader_s.h"
#include "Log.h"
#include "DummyFile.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Values / Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HT = 600;

int main() {
	// int* var1 = new int[MAXIMUM_NUM];

	SecondLog();
	// This is the commit TWO
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
	// Set callback for when gl window resizes
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	Shader shader1("texture.vert", "texture.frag");

	// Create shader program and link it with compiled 
	// vert & frag shaders
	// another nice to have: unbind the curr bound VAO, tho not
	// needed since binding another VAO will alr unbind the last 
	// VAO for you.
	// Note: VAO stores all binds for buffers that you bind 
	// (eg. VBO, EBO), as well as all unbind up to the point
	// where you unbind the VAO. So make sure you don't unbind a
	// EBO b4 you unbind the VAO. If not, it won't store the 
	// EBO in it!

	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};

	unsigned int indices[] = {  
	    0, 1, 3, // 1st tri
		1, 2, 3  // 2nd tri
	};  
	
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	// Texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL/*GL_LINE*/);


	// load and create texture
	unsigned int texture1, texture2; 
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set texture wrapping params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// texture filtering params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Generate a texture using the data gotten from the jpg file 
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	// const char* filePath = "container.jpg";
	unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create the texture, and generate mip maps, 
	data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	// don't forget to activate/use the shader before setting uniforms!
	shader1.use();
	// glUniform1i(glGetUniformLocation(shader1.ID, "texture1"), 0);
	shader1.setInt("texture1", 0);
	shader1.setInt("texture2", 1);
	// render loop
	// --------------
	while (!glfwWindowShouldClose(window)) {
		// Process key/mouse input commands
		processInput(window);
		// fill viewport with flat color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// bind all textures for each texture unit
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		// Render
		shader1.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// Swap front pixel buffer w/ back buffer, and also
		// Check for keys pressed, mouse moves/clicks etc.
		// and call all registered callback fns
		glfwSwapBuffers(window);
		glfwPollEvents(); 
	}

	// De-allocating all resources when no longer in use
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

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

// void bindAndDrawElemBuffer(unsigned int& EBO)
// {
// 	// tempEBO holds the index for the generated buffer 
// 	unsigned int tempEBO;
// 	glGenBuffers(1, &tempEBO);
// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tempEBO);
// 	// Send the index data over to the GPU side, where tempEBO "points" to this buffer.
// 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
// 	glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(GLuint), GL_UNSIGNED_INT, 0);
// }

// unsigned char* getTexData(const char* filePath) {
// 	unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels);
// 
// 	return data;
// }

