#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
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

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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
	
	Shader lightingShader("texture.vert", "lighting.frag");
	Shader lightSrcShader("texture.vert", "lightSource.frag");

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
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
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

	unsigned int cubeVAO, VBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);
	// glGenBuffers(1, &EBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);;
	// position attribute
	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL/*GL_LINE*/);

	// Set up VAO for light source, use same VBO
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// describe how to interpret the VBO data / set vertex attribute
	glBindVertexArray(lightCubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// light's position in world space
	glm::vec3 lightPos(1.2f, 0.2f, 2.0f);

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Process multiple key/mouse input commands for camera actor per frame
		std::vector<Command*> cmds = inputHandler.processInput(window);
		for (auto& cmd : cmds) {
			cmd->execute(camActor, deltaTime);
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Ensure your order or transform is SRT: Scale, rotate, then translate
		// if you tried someth like translate bef scale, you'll scale the translation by the same amt

		glm::mat4 proj = glm::perspective(glm::radians(camActor.getFov()), (float)SCR_WIDTH / (float)SCR_HT, 0.1f, 100.0f);
		lightingShader.setMat4("proj", proj);
		glm::mat4 view = camActor.getViewMatrix();
		lightingShader.setMat4("view", view);
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.use();
		lightingShader.setMat4("proj", proj);
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("model", model);
		lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("lightPos", lightPos);
		lightingShader.setVec3("viewPos", camActor.getCamPos());

		// Draw cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		lightSrcShader.use();
		glm::mat4 lightModel(1.0f);
		lightModel = glm::translate(lightModel, lightPos);
		lightModel = glm::scale(lightModel, glm::vec3(0.2f));
		lightSrcShader.setMat4("model", lightModel);
		lightSrcShader.setMat4("view", view);
		lightSrcShader.setMat4("proj", proj);
		lightSrcShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		// Draw cube light
		glBindVertexArray(lightCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// render boxes
		// glBindVertexArray(VAO);
		// for (int i = 0; i < 10; ++i) {
		// 	glm::mat4 model(1.0f);
		// 	model = glm::translate(model, cubePositions[i]);
		// 	float time = (i%3) ? 1 : (float)glfwGetTime();
		// 	float angle = 20.0f * time;
		// 	model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, .5f));
		// 	lightingShader.setMat4("model", model);
		// 	if (i == 0)
		// 		continue;
		// 	glDrawArrays(GL_TRIANGLES, 0, 36);
		// }
		

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents(); 
	}

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
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
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}
