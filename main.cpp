#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Point.h"
#include <memory>
#include <algorithm>

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
void renderScene(Shader shader, bool cullFront);
void renderQuad();
void renderCube();

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam);

// Values / Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HT = 600;

const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HT = 1024;

Camera camActor;
Camera lightActor(glm::vec3 (-2.0, 4.0, -1.0f), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HT / 2.0;
bool firstMouse = true;

unsigned int planeVAO = 0;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);  

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

	int flags; 
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	// GLint profileMask;
	// glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profileMask);
	// if (profileMask & GL_CONTEXT_CORE_PROFILE_BIT) {
	// 	std::cout << "You are using the Core Profile" << std::endl;
	// } else if (profileMask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) {
	// 	std::cout << "You are using the Compatibility Profile" << std::endl;
	// }

	float planeVertices[] = {
		// positions            // normals         // texcoords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };

	struct Matrices {
		glm::mat4 view;
		glm::mat4 proj;
	};

	glEnable(GL_MULTISAMPLE);

	Shader blinnPhongShader("blinnPhong.vert", "blinnPhong.frag");
	Shader screenShader("screen.vert", "screen.frag");
	Shader shadowMapShader("shadowMap.vert", "shadowMap.frag");
	Shader bpShadowShader("bphongShadow.vert", "bphongShadow.frag");

	unsigned int woodTex = LoadTexture("./textures/wood.png");

	unsigned int planeVBO;
	glGenBuffers(1, &planeVBO);
	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3* sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6* sizeof(float)));
	glBindVertexArray(0);

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	unsigned int numSamples = 4;
	// Attach 2D multisample texture image as the color buffer (but with higher resolution)
	unsigned int fbTex;
	glGenTextures(1, &fbTex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fbTex);
	// alloc space for this texture obj on GPU
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, numSamples, GL_RGB, SCR_WIDTH, SCR_HT, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	// attach this texture obj to framebuffer obj
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, fbTex, 0);

	// Create a renderbuffer object that can represent depth, stencil, depth + stencil, or color buffer (the multisampled versions)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	// Alloc space for the depth+stencil buffers in the currently bound renderbuff obj
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, numSamples, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// attach this renderbuffer obj to framebuffer obj
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	unsigned int ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrices), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// bind the ubo to the same index: block index 2
	glBindBufferRange(GL_UNIFORM_BUFFER, 2, ubo, 0, sizeof(Matrices));

	// get the uniform block location in each shader and bind it to an index
	unsigned int blinnPhongIdx = glGetUniformBlockIndex(blinnPhongShader.ID, "Matrices");
	glUniformBlockBinding(blinnPhongShader.ID, blinnPhongIdx, 2);

	// unsigned int shadowMapIdx = glGetUniformBlockIndex(shadowMapShader.ID, "Matrices");
	// glUniformBlockBinding(shadowMapShader.ID, shadowMapIdx, 2);

	// Model backpackModel("./BackpackModel/backpack.obj");
	// Model asteroidModel("./textures/rock/rock.obj");
	// Model planetModel("./textures/planet/planet.obj");

	Matrices mat1;
	mat1.view = glm::mat4(glm::mat3(camActor.getViewMatrix()));
	mat1.proj = glm::perspective(glm::radians(camActor.getFov()), (float)SCR_WIDTH / (float)SCR_HT, 0.1f, 100.0f);

	// Pass in the matrix data to the buffer:
	// glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat1.view));
	// glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(mat1.proj));
	// glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Generate another FBO attached with a color buffer of the same dimensions, in order to blit 
	// the multisampled 2D texture (down-res), which allows us to perform post-processing on the 
	// rendered scene before ending up as the final pixels on display.
	unsigned int interFBO;
	glGenFramebuffers(1, &interFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, interFBO);
	unsigned int interTex;
	glGenTextures(1, &interTex);
	glBindTexture(GL_TEXTURE_2D, interTex);
	// This color buffer remains empty for now --hence NULL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// bind the tex ID to the framebuffer as a color attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, interTex, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	screenShader.use();
	screenShader.setInt("quadTex", 0);

	blinnPhongShader.use();
	blinnPhongShader.setFloat("pointLights[0].constant", 1.0f);
	blinnPhongShader.setFloat("pointLights[0].linear", 0.09f);
	blinnPhongShader.setFloat("pointLights[0].quadratic", 0.032f);
	blinnPhongShader.setVec4("pointLights[0].position", glm::vec4(0.0f,.0f,.0f,1.0f));
	blinnPhongShader.setVec3("pointLights[0].ambient", glm::vec3(0.05f));
	blinnPhongShader.setVec3("pointLights[0].diffuse", glm::vec3(1.0f));
	blinnPhongShader.setVec3("pointLights[0].specular", glm::vec3(0.3f));
	blinnPhongShader.setInt("mat.texture_diffuse0", 0);

	// COMPUTE SHADER result -- Create image object (as a texture)
	unsigned int compShaderTexture;
	unsigned int TEXTURE_WIDTH = 512, TEXTURE_HEIGHT = 512;
	glGenTextures(1, &compShaderTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, compShaderTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(0, compShaderTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F); // bind 32f texture to tex ID

	Shader compShader("compShader.comp"); // Cr8 shader program and attach shader ID to it

	// mini shadow map fbo and depth attachment
	unsigned int msfbo, mdepthMap;
	glGenFramebuffers(1, &msfbo);
	glGenTextures(1, &mdepthMap);
	glBindTexture(GL_TEXTURE_2D, mdepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, msfbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mdepthMap, 0);
	// specify that this fbo doesn't hold or write to a color attachment
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// shadow map fbo and depth attachment
	unsigned int sfbo, depthMap;
	glGenFramebuffers(1, &sfbo);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// Set all 4 border sides to contain farthest distance possible so that they'll never be in shadow
	// in the frag shader shadow calculations
	float bordColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bordColor);

	glBindFramebuffer(GL_FRAMEBUFFER, sfbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	// specify that this fbo doesn't hold or write to a color attachment
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// store button commands, process them, and clear out at the end of the frame
	std::vector<Command*> cmds;
	glm::vec3 lightPos(-2.0, 4.0, -1.0f);
	
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Process key/mouse input
		inputHandler.processInput(window, cmds, &blinnPhongShader);
		for (auto& cmd : cmds) {
			cmd->execute(camActor, deltaTime);
		}
		cmds.clear();

		// clear default frame buffer depth and colors
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_DEPTH_TEST);

		float nearPlane = 1.0f, farPlane = 7.5f;
		glm::mat4 lightView = lightActor.getViewMatrix();
		glm::mat4 lightProj(glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane));
		glm::mat4 lightTransform = lightProj * lightView;
		glm::mat4 model(1.0f);

		// shadowMapShader.use();
		// shadowMapShader.setMat4("model", model);
		// shadowMapShader.setMat4("lightTransform", lightTransform);
		// glBindFramebuffer(GL_FRAMEBUFFER, msfbo);
		// glViewport(0, 0, SHADOW_WIDTH, SHADOW_HT);
		// glClear(GL_DEPTH_BUFFER_BIT);
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, woodTex);
		// renderScene(shadowMapShader, /*cullBack*/false);

		// lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		// lightTransform = lightProj * lightView;
		// shadowMapShader.setMat4("lightTransform", lightTransform);
		// glBindFramebuffer(GL_FRAMEBUFFER, sfbo);
		// glViewport(0, 0, SHADOW_WIDTH, SHADOW_HT);
		// glClear(GL_DEPTH_BUFFER_BIT);
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, woodTex);
		// renderScene(shadowMapShader, /*cullBack*/true);

		// glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// glViewport(0, 0, SCR_WIDTH, SCR_HT);
		// glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bpShadowShader.use();
		// bpShadowShader.setMat4("model", model);
		// bpShadowShader.setMat4("view", camActor.getViewMatrix());
		// bpShadowShader.setMat4("proj", mat1.proj);
		// bpShadowShader.setMat4("lightTransform", lightTransform);
		// bpShadowShader.setVec3("viewPos", camActor.getPos());
		// bpShadowShader.setVec3("lightPos", lightPos);
		// bpShadowShader.setInt("diffuseTexture", 0);
		// bpShadowShader.setInt("shadowMap", 1);
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, woodTex);
		// glActiveTexture(GL_TEXTURE1);
		// glBindTexture(GL_TEXTURE_2D, depthMap);
		// renderScene(bpShadowShader, /*cullBack*/false);
	
		// Render a texture buffer with same dimensions as screen and then 
		// pass that in as the texture for the quad
		compShader.use();
		// run compute shader, and set bit barrier so that the texture cannot be 
		// read from until compute shader is done writing to it --i.e. host program 
		// gets halted.
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, compShaderTexture);
		glDispatchCompute((unsigned int) TEXTURE_WIDTH, (unsigned int) TEXTURE_HEIGHT, 1); 
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
				
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// switch to default framebuffer and render depth map onto it
		glDisable(GL_DEPTH_TEST);
		screenShader.use();
		screenShader.setInt("quadTex", 0);
		screenShader.setFloat("near_plane", nearPlane);
		screenShader.setFloat("far_plane", farPlane);
		// glBindTexture(GL_TEXTURE_2D, mdepthMap);
		renderQuad();
	
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents(); 
	}

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
	glDeleteFramebuffers(1, &fbo);
	glDeleteFramebuffers(1, &interFBO);
	glDeleteProgram(screenShader.ID);
	glDeleteProgram(compShader.ID);

	// glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
	return 0;
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
	lightActor.ProcessMouseMovement(xOffset, yOffset);
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

unsigned int cubeVAO=0, cubeVBO=0;
void renderCube() {
	if (cubeVAO == 0) {
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			 // bottom face
			 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 // top face
			 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			  1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

unsigned int quadVAO=0, quadVBO=0;
void renderQuad() {
	if (quadVAO == 0) {
		float quadVertices[] = {
			// positions        // texCoords
			 // 0.75f,  1.0f, 0.0f, 0.0f, 1.0f,
			 // 0.75f,  0.7f, 0.0f, 0.0f, 0.0f,
			 // 1.0f,  0.7f, 0.0f, 1.0f, 0.0f,
			 // 0.75f,  1.0f, 0.0f, 0.0f, 1.0f,
			 // 1.0f,  0.7f,  0.0f, 1.0f, 0.0f,
			 // 1.0f,  1.0f,  0.0f, 1.0f, 1.0f
			 // positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	// render quad:
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void renderScene(const Shader shader, bool cullBack) {
	// floor
	glm::mat4 model(1.0f);
	// model = glm::rotate(model, glm::radians(15.0f), glm::normalize(glm::vec3(0.0, 1.0, 1.0)));
	shader.setMat4("model", model);
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	if (cullBack) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	}

	// cubes
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	renderCube();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	renderCube();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.25));
	shader.setMat4("model", model);
	renderCube();
	if (cullBack) {
		glCullFace(GL_BACK);
		glDisable(GL_CULL_FACE);
	}
}

// when window dimension is altered by the user, gl should immediately
// pick up on it and resize the viewport accordingly
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void APIENTRY glDebugOutput(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

