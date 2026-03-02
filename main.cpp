#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <filesystem>
#include "shader_s.h"
#include "Camera.h"
#include "STBFile.h"
#include "InputHandler.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Values / Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HT = 600;
	

Camera camActor;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = SCR_WIDTH / 2;
float lastY = SCR_HT / 2;
bool firstMouse = true;


int main() {
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

	InputHandler inputHandler(window);
	glfwMakeContextCurrent(window);
	// Set callback for when gl window resizes
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

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
		// positions          // texture coords
	    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	
	    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	
	     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	
	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	
	    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	unsigned int indices[] = {  
	    0, 1, 3, // 1st tri
		1, 2, 3  // 2nd tri
	};

	glm::vec3 cubePositions[] = {
	    glm::vec3( 0.0f,  0.0f,  0.0f), 
	    glm::vec3( 2.0f,  5.0f, -15.0f), 
	    glm::vec3(-1.5f, -2.2f, -2.5f),  
	    glm::vec3(-3.8f, -2.0f, -12.3f),  
	    glm::vec3( 2.4f, -0.4f, -3.5f),  
	    glm::vec3(-1.7f,  3.0f, -7.5f),  
	    glm::vec3( 1.3f, -2.0f, -2.5f),  
	    glm::vec3( 1.5f,  2.0f, -2.5f), 
	    glm::vec3( 1.5f,  0.2f, -1.5f), 
	    glm::vec3(-1.3f,  1.0f, -1.5f)  
	}; 
	
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);;

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
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
	shader1.setInt("texture1", 0);
	shader1.setInt("texture2", 1);

	// We want the camera to look at the origin

	// Because the camera looks in the -z direction, 
	// we want the positive Z dir of the cam to be opposite of 
	// where the cam is looking (the negative z dir), so:
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);

	// render loop
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// Process key/mouse input commands for camera actor
		std::vector<Command*> cmds = inputHandler.processInput(window);
		for (auto& cmd : cmds) {
			cmd->execute(camActor, deltaTime);
		}

		// fill viewport with flat color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bind all textures for each texture unit
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		
		// Activate shader so we can start sending out matrices from CPU to Shader/GPU
		shader1.use();
		// Always make sure your order or transform is SRT!! Scale, rotate, then translate!! 
		// (if you tried someth like translate, then scale, you'd also scale the translation by the same amt!)

		// Perspective proj transform matrix. Pass proj matrix to shader/send it to the GPU
		glm::mat4 proj = glm::perspective(glm::radians(camActor.getFov()), (float)SCR_WIDTH / (float)SCR_HT, 0.1f, 100.0f);
		shader1.setMat4("proj", proj);
		// View transform matrix
		glm::mat4 view = camActor.getViewMatrix();
		shader1.setMat4("view", view);
		// render boxes
		glBindVertexArray(VAO);
		for (int i = 0; i < 10; ++i) {
			glm::mat4 model(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float time = (i%3) ? 1 : (float)glfwGetTime();
			float angle = 20.0f * time;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, .5f));
			shader1.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents(); 
	}

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
	glfwTerminate();

	return 0;
}

// when window dimension is altered by the user, gl should immediately
// pick up on it and resize the viewport accordingly
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	// When the mouse wheel scrolls fwd, we take it as a zooming effect (i.e. the fov gets smaller)
	camActor.ProcessMouseScroll(yoffset);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	// Check if its first time the app is in focus, do this to avoid
	// suddenly jump in camera
	if (firstMouse) {
		firstMouse = false;
		lastX = xpos;
		lastY = ypos;
	}

	// Calculate the x and y offsets and set that into a dir vector
	double xOffset, yOffset;
	const float moveSensitivity = 0.1f;
	xOffset = (xpos - lastX)*moveSensitivity;
	yOffset = (lastY - ypos)*moveSensitivity; // flipped b/c y coords on screen grow in value jrom top to btm
	lastX = xpos;
	lastY = ypos;

	camActor.ProcessMouseMovement(xOffset, yOffset);
}

void bindArrBuffer(unsigned int& VAO, unsigned int& VBO, int* vertices) {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}
