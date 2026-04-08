#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

	float planeVertices[] = {
		// positions          // normals           // texture Coords (note we set these higher than 1 together with GL_REPEAT as texture wrapping mode to cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f, -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f, -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
  
		 5.0f, -0.5f,  5.0f,  5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f, -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};

	float transparentVertices[] = {
		// positions	     // normals			  // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
  
		0.0f,  0.5f,  0.0f,  0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};

	float quadVertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	std::vector<glm::vec3> windows{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};

	std::vector<glm::vec3> vegetation{
		glm::vec3(-1.5f,  0.0f, -0.48f),
		glm::vec3(1.5f,  0.0f,  0.51f),
		glm::vec3(0.0f,  0.0f,  0.7f),
		glm::vec3(-0.3f,  0.0f, -2.3f),
		glm::vec3(0.5f,  0.0f, -0.6f)
	};

	float rearViewVertices[] = {
		 // positions   // texCoords
        -0.3f,  1.0f,  0.0f, 1.0f,
        -0.3f,  0.7f,  0.0f, 0.0f,
         0.3f,  0.7f,  1.0f, 0.0f,

        -0.3f,  1.0f,  0.0f, 1.0f,
         0.3f,  0.7f,  1.0f, 0.0f,
         0.3f,  1.0f,  1.0f, 1.0f
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

	Shader phongShader("texture.vert", "phongShading.frag");
	Shader depthTestShader("depthTest.vert", "depthTest.frag");
	Shader stencilTestShader("depthTest.vert", "stencilTest.frag");
	Shader fbShader("framebuffer.vert", "framebuffer.frag");
	Shader skyboxShader("cubeMap.vert", "cubeMap.frag");
	Shader cubeReflShader("reflect.vert", "reflect.frag");

	unsigned int metalTex = LoadTexture("textures/metal.png");
	unsigned int marbleTex = LoadTexture("textures/marble.jpg");
	unsigned int windowTex = LoadTexture("textures/blending_transparent_window.png");
	unsigned int grassTex = LoadTexture("textures/grass.png");

	unsigned int transparentVAO, transparentVBO;
	glGenBuffers(1, &transparentVBO);
	glGenVertexArrays(1, &transparentVAO);
	glBindVertexArray(transparentVAO);
	glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);


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

	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// light's position in world space
	glm::vec3 lightPos(2.0f, 0.6f, -4.0f);
	glm::vec3 lightColor(1.0f);
	
	// Model backpackModel("./BackpackModel/backpack.obj");
	// store button commands, process them, and clear out at the end of the frame
	std::vector<Command*> cmds;

	// Disallow OpenGL from updating the depth each frame after it computes depth test for every fragment. 
	// This makes the depth buffer read-only.
	// (its supposed to be able to update the depth buffer if a fragment passes the depth test)
	glEnable(GL_BLEND);
	// 1st arg is src factor, 2nd is dest factor
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE); 
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

	// glEnable(GL_STENCIL_TEST);
	std::map<float, glm::vec3> sorted;

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// generate the texture
	unsigned int fbtex;
	glGenTextures(1, &fbtex);
	glBindTexture(GL_TEXTURE_2D, fbtex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// attach texture to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbtex, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach a renderbuffer object for the depth buffer
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0); // unbind the renderbuffer object
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	
	// Check to make sure framebuffer attachments have no issues:
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	// finally, unbind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Do offscreen rendering to the texture
	// create a VAO with the vertex data binded to it
	unsigned int texVAO, texVBO;
	glGenVertexArrays(1, &texVAO);
	glGenBuffers(1, &texVBO);
	glBindVertexArray(texVAO);
	glBindBuffer(GL_ARRAY_BUFFER, texVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	
	unsigned int rearVAO, rearVBO;
	glGenBuffers(1, &rearVBO);
	glGenVertexArrays(1, &rearVAO);
	glBindVertexArray(rearVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rearVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rearViewVertices), rearViewVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	std::vector<std::string> faces {
	    "textures/skybox/right.jpg",
	    "textures/skybox/left.jpg",
	    "textures/skybox/top.jpg",
	    "textures/skybox/bottom.jpg",
	    "textures/skybox/front.jpg",
	    "textures/skybox/back.jpg"
	};
	std::vector<std::string> nFaces {
	    "textures/skybox/normalMap.png",
	    "textures/skybox/normalMap.png",
	    "textures/skybox/normalMap.png",
	    "textures/skybox/normalMap.png",
	    "textures/skybox/normalMap.png",
	    "textures/skybox/normalMap.png"
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

	glDepthMask(GL_TRUE);
	unsigned int cubemapTex = LoadCubemap(faces);
	unsigned int normmapTex = LoadCubemap(nFaces);

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
		glClear(GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		 
		// before rendering anything, first render the skybox
		// Ensure your order or transform is SRT: Scale, rotate, then translate
		// if you tried someth like translate bef scale, you'll scale the translation by the same amt
		// Draw cube object
		glm::mat4 proj = glm::perspective(glm::radians(camActor.getFov()), (float)SCR_WIDTH / (float)SCR_HT, 0.1f, 100.0f);
		glm::mat4 view = camActor.getViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE); 
		// floor		
		depthTestShader.use();
		depthTestShader.setMat4("view", view);
		depthTestShader.setMat4("proj", proj);
		model = glm::mat4(1.0f);
		depthTestShader.setMat4("model", model);
		glBindVertexArray(planeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, metalTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// cube 1
		glEnable(GL_CULL_FACE); 
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		depthTestShader.setMat4("model", model);
		glBindVertexArray(cubeVAO);
		glBindTexture(GL_TEXTURE_2D, marbleTex);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// cube 2
		model = glm::mat4(1.0f);
	    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		depthTestShader.setMat4("model", model);
		depthTestShader.setMat4("view", view);
		depthTestShader.setMat4("proj", proj);
		// glDrawArrays(GL_TRIANGLES, 0, 36);

		glDisable(GL_CULL_FACE); 
		depthTestShader.setInt("tex1", 0);
		glBindVertexArray(transparentVAO);

		glBindTexture(GL_TEXTURE_2D, windowTex);
		// sort the windows by increasing dist to the camera
		for (auto& windowPos : windows) {
			float dist = glm::length(camActor.getPos() - windowPos);
			sorted[dist] = windowPos;
		}

		// sorted distances are from closest to farthest--render farthest transparent windows first
		for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
			glm::mat4 transpModel(1.0f);
			transpModel = glm::translate(transpModel, it->second);
			// grab mat4 uniform for this mat4 variable name, and update its value with new mat4 data
			depthTestShader.setMat4("model", transpModel);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		// Render reflective cube
		cubeReflShader.use();
		glBindVertexArray(cubeVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTex);
		model = glm::translate(model, glm::vec3(-3.0f, 0.0f, -3.0f));
		cubeReflShader.setMat4("model", model);
		cubeReflShader.setMat4("view", view);
		cubeReflShader.setMat4("proj", proj);
		cubeReflShader.setVec3("viewPos", camActor.getPos());
		cubeReflShader.setInt("cubeMap", 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		// removes any translation, but keeps all rotation transformations (keeps upper 3x3 matrix)
		glm::mat4 camView = glm::mat3(camActor.getViewMatrix());
		model = glm::mat4(1.0f);
		skyboxShader.use();
		skyboxShader.setMat4("model", model);
		skyboxShader.setMat4("view", camView);
		skyboxShader.setMat4("proj", proj);
		skyboxShader.setInt("cubeMap", 0);
		skyboxShader.setInt("normMap", 1);
		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTex);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		glBindVertexArray(0);

		// Render the backwards scene onto the rear view mirror quad:
		// first get the back facing view matrix
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		depthTestShader.use();
		depthTestShader.setInt("tex1", 0);
		// camActor.setFront(-camActor.getFront());
		camActor.setYaw(camActor.getYaw() + 180.0f);
		camActor.setPitch(-camActor.getPitch());
		// don't constrain pitch (last arg false) b/c we want to also reverse pitch values (?)
		camActor.ProcessMouseMovement(0.0f, 0.0f, false);
		// Get the view matrix of the camera as it looks behind where its currently looking
		glm::mat4 reverseViewMat(camActor.getViewMatrix());
		depthTestShader.setMat4("view", reverseViewMat);
		depthTestShader.setMat4("proj", proj);
		// set it back to normal
		camActor.setYaw(camActor.getYaw() - 180.0f);
		camActor.setPitch(-camActor.getPitch());
		camActor.ProcessMouseMovement(0.0f, 0.0f, false);

		// floor		
		model = glm::mat4(1.0f);
		depthTestShader.setMat4("model", model);
		glBindVertexArray(planeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, metalTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, marbleTex);
		// cube 1
		glEnable(GL_CULL_FACE); 
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		depthTestShader.setMat4("model", model);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// cube 2
		model = glm::mat4(1.0f);
	    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		depthTestShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glDisable(GL_CULL_FACE); 
		glBindVertexArray(transparentVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, windowTex);

		// sorted distances are from closest to farthest--render farthest transparent windows first
		for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
			glm::mat4 transpModel(1.0f);
			transpModel = glm::translate(transpModel, it->second);
			// grab mat4 uniform for this mat4 variable name, and update its value with new mat4 data
			depthTestShader.setMat4("model", transpModel);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		sorted.clear();

		// removes any translation, but keeps all rotation transformations (keeps upper 3x3 matrix)
		camView = glm::mat3(reverseViewMat);
		model = glm::mat4(1.0f);
		skyboxShader.use();
		skyboxShader.setMat4("model", model);
		skyboxShader.setMat4("view", camView);
		skyboxShader.setMat4("proj", proj);
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, normmapTex);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// We're done rendering the back view. Set the view matrix back to its original.
		view = camActor.getViewMatrix();
		depthTestShader.setMat4("view", view);
		depthTestShader.setMat4("proj", proj);

		// Render the smaller quad onto the default framebuffer
		fbShader.use();
		fbShader.setInt("screenTex", 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(rearVAO);
		glBindTexture(GL_TEXTURE_2D, fbtex);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents(); 
	}

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &planeVAO);

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

