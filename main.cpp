#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Point.h"
#include <memory>

#include <glm/gtc/matrix_transform.hpp>
#include <string>

#include "Shader.h"
#include "Camera.h"
#include "InputHandler.h"
#include "Model.h"
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int LoadTexture(const char* fPath);
unsigned int LoadCubemap(std::vector<std::string>& faces);

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

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	float cubeVertices[] = {
		// Back face		  // normals          // tex coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right    
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right              
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left                
		// Front face        
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, // bottom-left
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f, // top-right
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, // bottom-right        
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f, // top-right
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, // bottom-left
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 1.0f, // top-left        
		// Left face         
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left       
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		// Right face        
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right      
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right          
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 // Bottom face         
		 -0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		  0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		  0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left        
		  0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		 -0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		 -0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		 // Top face         
		 -0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		  0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
		  0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right                 
		  0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		 -0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // bottom-left  
		 -0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f  // top-left              
	};

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	float quadVertices[] = {
		// positions     // colors
		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,   
		 0.05f,  0.05f,  0.0f, 1.0f, 1.0f		    		
	};  

	float points[] = {
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
		 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
		-0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
	};  

	struct Matrices {
		glm::mat4 view;
		glm::mat4 proj;
	};

	glm::vec2 translations[100];
	int index = 0;
	float offset = 0.1f;
	for (int i = -10; i < 10; i+=2) {
		for (int j = -10; j < 10; j+=2) {
			glm::vec2 coords((float)i / 10.0f + offset, (float)j/10.0f + offset);
			translations[index++] = coords;
		}
	}

	unsigned int instanceVAO, instanceVBO, iVBO;;
	glGenVertexArrays(1, &instanceVAO);
	glGenBuffers(1, &instanceVBO);
	glGenBuffers(1, &iVBO);
	glBindVertexArray(instanceVAO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (2*sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, iVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(translations), &translations[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(2, 1);
	glBindVertexArray(0);


	Shader geomShader("geomShader.vert", "geomShader.frag", "geomShader.geom");
	unsigned int pointsVAO, pointsVBO;
	glGenVertexArrays(1, &pointsVAO);
	glGenBuffers(1, &pointsVBO);
	glBindVertexArray(pointsVAO);
	glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (2*sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	
	Shader phongShader("texture.vert", "phongShading.frag");
	// Shader depthTestShader("depthTest.vert", "depthTest.frag", "explodingGeom.geom");
	Shader depthTestShader("depthTest.vert", "depthTest.frag");
	Shader normalDisplayShader("normalDisplay.vert", "normalDisplay.frag", "normalDisplay.geom");
	Shader instancingShader("framebuffer.vert", "framebuffer.frag");

	Shader blueShader("uboShader.vert", "blueShader.frag");
	Shader greenShader("uboShader.vert", "greenShader.frag");
	Shader redShader("uboShader.vert", "redShader.frag");
	Shader yellowShader("uboShader.vert", "yellowShader.frag");

	unsigned int ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrices), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// bind the ubo to block index 2
	glBindBufferRange(GL_UNIFORM_BUFFER, 2, ubo, 0, sizeof(Matrices));

	// get the uniform block location in each shader and bind it to an index
	unsigned int bMatIdx = glGetUniformBlockIndex(blueShader.ID, "Matrices");
	unsigned int gMatIdx = glGetUniformBlockIndex(greenShader.ID, "Matrices");
	unsigned int rMatIdx = glGetUniformBlockIndex(redShader.ID, "Matrices");
	unsigned int yMatIdx = glGetUniformBlockIndex(yellowShader.ID, "Matrices");

	unsigned int instMatIdx = glGetUniformBlockIndex(instancingShader.ID, "Matrices");
	glUniformBlockBinding(instancingShader.ID, instMatIdx, 2);

	// obtain the "Matrices" uniform block location for depthTestShader
	unsigned int explodeMatrixIdx = glGetUniformBlockIndex(depthTestShader.ID, "Matrices");
	unsigned int normalDispMatrixIdx = glGetUniformBlockIndex(normalDisplayShader.ID, "Matrices");
	// bind this unif blk idx on the GPU / Shader side to binding number 2 (Prior, we already binded the UBO to binding no. 2 as well)
	glUniformBlockBinding(depthTestShader.ID, explodeMatrixIdx, 2);
	glUniformBlockBinding(normalDisplayShader.ID, normalDispMatrixIdx, 2);

	// bind the unif block location to some block index for each shader prog:
	glUniformBlockBinding(blueShader.ID, bMatIdx, 2);
	glUniformBlockBinding(greenShader.ID, gMatIdx, 2);
	glUniformBlockBinding(redShader.ID, rMatIdx, 2);
	glUniformBlockBinding(yellowShader.ID, yMatIdx, 2);

	unsigned int metalTex = LoadTexture("textures/metal.png");
	unsigned int marbleTex = LoadTexture("textures/marble.jpg");
	unsigned int windowTex = LoadTexture("textures/blending_transparent_window.png");

	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	// always bind buffer before passing in buffer data
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Model backpackModel("./BackpackModel/backpack.obj");
	Model asteroidModel("./textures/planets/asteroid/rock.obj");
	Model planetModel("./textures/planets/planet.obj");

	// store button commands, process them, and clear out at the end of the frame
	std::vector<Command*> cmds;

	std::vector<std::string> faces {
	    "textures/skybox/right.jpg",
	    "textures/skybox/left.jpg",
	    "textures/skybox/top.jpg",
	    "textures/skybox/bottom.jpg",
	    "textures/skybox/front.jpg",
	    "textures/skybox/back.jpg"
	};

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	unsigned int cubemapTex = LoadCubemap(faces);

	// bind the ubo to the same index
	Matrices mat1;

	mat1.view = glm::mat4(glm::mat3(camActor.getViewMatrix()));
	mat1.proj = glm::perspective(glm::radians(camActor.getFov()), (float)SCR_WIDTH / (float)SCR_HT, 0.1f, 1000.0f);

	// Pass in the matrix data to the buffer:
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat1.view));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(mat1.proj));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glEnable(GL_DEPTH_TEST);

	// Calculate the displament for 1000 asteroid model matrices
	int count = 1000;
	std::unique_ptr<glm::mat4[]> modelMats;
	modelMats = std::make_unique<glm::mat4[]>(1000);
	srand(glfwGetTime());
	float radius = 50.0f;
	offset = 2.5f;
	for (int i = 0; i < count; ++i) {
		glm::mat4 model(1.0);
		float angle = ((float)i / (float)count) * 360.0f;
		// generate any random value between 0.0 and 2.5:
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0 - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0 - offset;
		// limit y displacement so that asteroids scattering still looks like a 'belt'
		float y = displacement * 0.4f; 
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0 - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// influence the scale 
		// 0.05 and 0.25
		model = glm::scale(model, glm::vec3((rand() % 20)/100.0 + (0.05)));

		// influence rotation
		model = glm::rotate(model, float(rand() % 360), glm::vec3(0.25, 0.6, 0.9));
		modelMats[i] = model;
	}

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Process multiple key/mouse input commands for camera actor per frame
		inputHandler.processInput(window, cmds);
		for (auto& cmd : cmds) {
			cmd->execute(camActor, deltaTime);
		}
		cmds.clear();

		// do off-screen rendering on the bound frame buffer object
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			
		// geomShader.use();
		// glBindVertexArray(pointsVAO);
		// glDrawArrays(GL_POINTS, 0, 4);
		

		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camActor.getViewMatrix()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		
		instancingShader.use();
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		instancingShader.setMat4("model", model);
		planetModel.Draw(instancingShader);
		for (int i = 0; i < count; ++i) {
			instancingShader.setMat4("model", modelMats[i]);
			asteroidModel.Draw(instancingShader);
		}
		// glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);

		// QUESTION: Why doesn't the screen show if the view matrix is not updated every frame?..

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents(); 
	}

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteVertexArrays(1, &pointsVAO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------

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

unsigned int LoadTexture(const char* fPath) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	int width, ht, numColorChannels;
	unsigned char* data = stbi_load(fPath, &width, &ht, &numColorChannels, 0);

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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		// set how to choose + sample mipmaps to render objs that are far away
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	} else {
		std::cout << "Texture failed to load at path: " << fPath << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
unsigned int LoadCubemap(std::vector<std::string>& faces) {
	unsigned int texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	int w, h, nrChannels;
	for (int i = 0; i < faces.size(); ++i) {
		void* data = stbi_load(faces[i].c_str(), &w, &h, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		} else {
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
		}
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return texID;
}

