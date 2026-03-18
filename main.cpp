#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Camera.h"
#include "STBFile.h"
#include "InputHandler.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* filePath);

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
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	float verts[] = {
    // positions          // colors           // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0, 1.0,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0, 0.0,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0, 0.0,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0, 1.0    // top left 
	};
	unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
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
	
	Shader lightingShader("texture.vert", "lighting.frag");
	Shader lightSrcShader("texture.vert", "lightSource.frag");
	Shader texShaderProg("texCoord.vert", "texCoord.frag");

	stbi_set_flip_vertically_on_load(true);
	unsigned int texID = loadTexture("container.jpg");
	unsigned int texID2 = loadTexture("awesomeface.png");
	unsigned int diffuseMap = loadTexture("container2.png");
	unsigned int specularMap = loadTexture("container2_specular.png");
	unsigned int emissionMap = loadTexture("matrix.jpg");

	unsigned int texVAO, texVBO, texEBO;
	glGenVertexArrays(1, &texVAO);
	glBindVertexArray(texVAO);
	// note: always bind buffer before passing in buffer data
	glGenBuffers(1, &texVBO);
	glGenBuffers(1, &texEBO);
	glBindBuffer(GL_ARRAY_BUFFER, texVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// make sure you don't unbind an EBO b4 you unbind the VAO. 
	// If not, it won't store the EBO in it!

	unsigned int cubeVAO, VBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);
	// glGenBuffers(1, &EBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);;
	// position attribute
	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Set up VAO for light source, use same VBO
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);

	// light's position in world space
	glm::vec3 lightPos(2.0f, 0.6f, -4.0f);
	glm::vec3 lightColor(1.0f);
	texShaderProg.use();
	texShaderProg.setInt("myTexture", 0);
	texShaderProg.setInt("myTexture2", 1);
	lightingShader.use();
	lightingShader.setFloat("mat.shininess", 32.0f);
	lightingShader.setInt("mat.diffuse", 0);
	lightingShader.setInt("mat.specular", 1);
	lightingShader.setInt("mat.emission", 2);

	std::vector<Command*> cmds;

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Process multiple key/mouse input commands for camera actor per frame
		inputHandler.processInput(window, cmds, &texShaderProg);
		for (auto& cmd : cmds) {
			cmd->execute(camActor, deltaTime);
		}
		cmds.clear();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Ensure your order or transform is SRT: Scale, rotate, then translate
		// if you tried someth like translate bef scale, you'll scale the translation by the same amt

		lightingShader.use();
		// Draw cube object
		glm::mat4 proj = glm::perspective(glm::radians(camActor.getFov()), (float)SCR_WIDTH / (float)SCR_HT, 0.1f, 100.0f);
		glm::mat4 view = camActor.getViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("proj", proj);
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("model", model);

		lightingShader.setVec3("light.ambient", glm::vec3(0.1f));
		lightingShader.setVec3("light.diffuse", glm::vec3(0.8f));
		lightingShader.setVec3("light.specular", glm::vec3(1.0f));
		lightingShader.setFloat("light.constant", 1.0f);
		lightingShader.setFloat("light.linear", 0.09f);
		lightingShader.setFloat("light.quadratic", 0.032f);
		lightingShader.setVec4("light.lightVector", glm::vec4(lightPos, 1.0f));
		lightingShader.setVec4("light.lightVector2", -0.2f, 4.0f, -0.3f, .0f);

		lightingShader.setVec4("light.position", glm::vec4(camActor.getPos(), 1.0f));
		lightingShader.setVec4("light.direction", glm::vec4(camActor.getFront(), 0.0f));
		lightingShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.0f)));
		lightingShader.setVec4("viewPos", glm::vec4(camActor.getPos(), 1.0f));

		lightingShader.setFloat("time", currentFrame);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, emissionMap);
		glBindVertexArray(cubeVAO);

		int i = 0;
		for (auto& pos : cubePositions) {
			glm::mat4 model(1.0f);
			model = glm::translate(model, pos + glm::vec3(.0f, -1.0f, .0f));
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			++i;
		}

		// Draw light cube object
		lightSrcShader.use();
		glm::mat4 lightModel(1.0f);
		lightModel = glm::translate(lightModel, lightPos);
		lightModel = glm::scale(lightModel, glm::vec3(0.2f));
		lightSrcShader.setMat4("model", lightModel);
		lightSrcShader.setMat4("view", view);
		lightSrcShader.setMat4("proj", proj);
		lightSrcShader.setVec3("lightColor", glm::vec3(1.0f));
		glBindVertexArray(lightCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

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

unsigned int loadTexture(const char* filePath) {

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, ht, numColorChannels;
	unsigned char* data = stbi_load(filePath, &width, &ht, &numColorChannels, 0);

	if (data) {
		GLenum format;
		if (numColorChannels == 1) {
			format = GL_RED;
		}
		else if (numColorChannels == 3) {
			format = GL_RGB;
		}
		else if (numColorChannels == 4) {
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		// send the texture/lighting map data to the GPU + gen mipmap for it
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, ht, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// sort what happens if tex coords go beyond range [0, 1.0]
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set how to choose + sample mipmaps to render objs that are far away
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	} else {
	    std::cout << "Texture failed to load at path: " << filePath << std::endl;
        stbi_image_free(data);	
	}

	return textureID;
}
