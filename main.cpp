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
	glfwWindowHint(GLFW_SAMPLES, 4);

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

	float quadVertices[] = {   // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

	 float cubeVertices[] = {
        // positions       
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

	struct Matrices {
		glm::mat4 view;
		glm::mat4 proj;
	};

	glEnable(GL_MULTISAMPLE);

	// Shader geomShader("geomShader.vert", "geomShader.frag", "geomShader.geom");
	Shader planetShader("planet.vert", "planet.frag");
	// Shader depthTestShader("depthTest.vert", "depthTest.frag", "explodingGeom.geom");
	Shader depthTestShader("depthTest.vert", "depthTest.frag");
	Shader instancingShader("framebuffer.vert", "framebuffer.frag");

	Shader cubeShader("geomShader.vert", "geomShader.frag");
	Shader screenShader("screen.vert", "screen.frag");

	unsigned int quadVAO, quadVBO; 
	glGenBuffers(1, &quadVBO);
	glGenVertexArrays(1, &quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices[0], GL_STATIC_DRAW);
	glBindVertexArray(quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float)));
	glBindVertexArray(0);

	unsigned int cubeVAO, cubeVBO; 
	glGenBuffers(1, &cubeVBO);
	glGenVertexArrays(1, &cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices[0], GL_STATIC_DRAW);
	glBindVertexArray(cubeVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glBindVertexArray(0);

	unsigned int fbo;
	glGenBuffers(1, &fbo);
	glBindBuffer(GL_FRAMEBUFFER, fbo);
	unsigned int numSamples = 4;
	// Attach 2D multisample texture image as the color buffer (but with higher resolution)
	unsigned int fbTex;
	glGenTextures(1, &fbTex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fbTex);
	// alloc space for this texture obj on GPU
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, numSamples, GL_RGB, SCR_WIDTH, SCR_HT, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	// bind this texture obj to framebuffer obj
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, fbTex, 0);

	// Create a renderbuffer object that can represent depth, stencil, depth + stencil, or color buffer (the multisampled versions)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	// Alloc space for the depth+stencil buffers
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, numSamples, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HT);
	// bind this renderbuffer obj to framebuffer obj
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glBindBuffer(GL_FRAMEBUFFER, 0);

	// Since multisample color + depth + stencil buffers are very larger in size than the resulting color buffer we want to render to
	// need to downscale the color buffer:
	// 
	// Read from off-screen rendered framebuffer object and write to default framebuffer after scene is drawn off-screen
	glBindFramebuffer(GL_READ_BUFFER, fbo);
	glBindFramebuffer(GL_DRAW_BUFFER, 0);

	// Down-res the multisampled color buffer before copying the color buffer pixels over
	glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HT, 0, 0, SCR_WIDTH, SCR_HT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	unsigned int ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrices), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// bind the ubo to block index 2
	glBindBufferRange(GL_UNIFORM_BUFFER, 2, ubo, 0, sizeof(Matrices));

	// get the uniform block location in each shader and bind it to an index
	unsigned int instMatIdx = glGetUniformBlockIndex(instancingShader.ID, "Matrices");
	glUniformBlockBinding(instancingShader.ID, instMatIdx, 2);
	unsigned int planetShaderMatIdx = glGetUniformBlockIndex(planetShader.ID, "Matrices");
	glUniformBlockBinding(planetShader.ID, planetShaderMatIdx, 2);
	// obtain the "Matrices" uniform block location for depthTestShader
	unsigned int explodeMatrixIdx = glGetUniformBlockIndex(depthTestShader.ID, "Matrices");
	// bind the unif block location to some block index for each shader prog:
	glUniformBlockBinding(depthTestShader.ID, explodeMatrixIdx, 2);
	unsigned int cubeMatIdx = glGetUniformBlockIndex(cubeShader.ID, "Matrices");
	glUniformBlockBinding(cubeShader.ID, cubeMatIdx, 2);

	// Model backpackModel("./BackpackModel/backpack.obj");
	// Model asteroidModel("./textures/rock/rock.obj");
	// Model planetModel("./textures/planet/planet.obj");

	// store button commands, process them, and clear out at the end of the frame
	std::vector<Command*> cmds;
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
	int count = 10000;
	std::unique_ptr<glm::mat4[]> modelMats;
	modelMats = std::make_unique<glm::mat4[]>(count);
	srand(glfwGetTime());
	float radOuter = 100.0f;
	float radInner = 50.0f;

	float offset = 10.0f;
	// for (int i = 0; i < count; ++i) {
	// 	glm::mat4 model(1.0);
	// 	float angle = ((float)i / (float)count) * 360.0f;
	// 	// generate any random value between 0.0 and 2.5:
	// 	float displacement = (rand() % (int)(2 * offset * 100)) / 100.0 - offset;
	// 	float x = sin(angle) * radInner + displacement;
	// 	displacement = (rand() % (int)(2 * offset * 100)) / 100.0 - offset;
	// 	float z = cos(angle) * radInner + displacement;
	// 	displacement = (rand() % (int)(2 * offset * 100)) / 100.0 - offset;
	// 	// limit y displacement so that asteroids scattering still looks like a 'belt'
	// 	float y = displacement * 0.4f;

	// 	model = glm::translate(model, glm::vec3(x, y, z));

	// 	// influence the scale 
	// 	// 0.05 and 0.25
	// 	model = glm::scale(model, glm::vec3((rand() % 20)/100.0 + (0.05)));

	// 	// influence rotation
	// 	model = glm::rotate(model, float(rand() % 360), glm::vec3(0.25, 0.6, 0.9));
	// 	modelMats[i] = model;
	// }
	
	// bind the VAO for the asteroid mesh
	// then bind this IBO into the VAO

	// Instance buffer object (an additional layout location required on the shader)
	// unsigned int IBO;
	// glGenBuffers(1, &IBO);
	// glBindBuffer(GL_ARRAY_BUFFER, IBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4)*count, &modelMats.get()[0], GL_STATIC_DRAW);
	// for (int i = 0; i < asteroidModel.meshes.size(); ++i) {
	// 	glBindVertexArray(asteroidModel.meshes[i].VAO);
	// 	glEnableVertexAttribArray(3);
	// 	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	// 	glEnableVertexAttribArray(4);
	// 	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	// 	glEnableVertexAttribArray(5);
	// 	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2*sizeof(glm::vec4)));
	// 	glEnableVertexAttribArray(6);
	// 	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3*sizeof(glm::vec4)));

	// 	// Go to the next mat4 elem in the instance array (in this case, an array of 1000 glm::mat4's)
	// 	glVertexAttribDivisor(3, 1);
	// 	glVertexAttribDivisor(4, 1);
	// 	glVertexAttribDivisor(5, 1);
	// 	glVertexAttribDivisor(6, 1);

	// 	glBindVertexArray(0);
	// }

	// Generate another FBO attached with a color buffer of the same dimensions, in order to blit 
	// the multisampled 2D texture (down-res), and then render that intermediate FBO onto a 2D texture 
	// Switch over to default frame buffer and render the Quad (taking up the entire screen space) using the 
	// intermediate FBO's attached texture as the texture for the quad

	unsigned int interFBO;
	glGenFramebuffers(1, &interFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, interFBO);
	unsigned int interTex;
	glGenTextures(1, &interTex);
	glBindTexture(GL_TEXTURE_2D, interTex);
	// This color buffer remains empty for now --hence NULL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// bind the tex ID to the framebuffer as a color attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, interTex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

		// clear default frame buffer depth and colors
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// clear multi-sample fbo depth and colors
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		// Enable depth test so that objects in the scene get overwritten if they are closer to the camera
		glEnable(GL_DEPTH_TEST);
			
		// Set all transf matrices
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camActor.getViewMatrix()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		cubeShader.use();
		glm::mat4 model(1.0f);
		cubeShader.setMat4("model", model);

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Copy the multisample color buffer to the regular color buffer (write this downres-ed image to the intermediate FBO's attached texture)
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, interFBO);
		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HT, 0, 0, SCR_WIDTH, SCR_HT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		
		// set to default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// Disable depth test so that quad gets rendered regardless of whats in the depth buffer
		glDisable(GL_DEPTH_TEST);

		screenShader.use();
		screenShader.setInt("quadTex", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, interTex);
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// planetShader.use();
		// glm::mat4 model(1.0f);
		// model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		// model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		// planetShader.setMat4("model", model);
		// planetModel.Draw(planetShader);

		// instancingShader.use();
		// instancingShader.setInt("mat.texture_diffuse1", 0);
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, asteroidModel.textures_loaded[0].ID);
		// for (int i = 0; i < asteroidModel.meshes.size(); ++i) {
		// 	glBindVertexArray(asteroidModel.meshes[i].VAO);
		// 	glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(asteroidModel.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, count);
		// 	// asteroidModel.Draw(instancingShader);
		// 	glBindVertexArray(0);
		// }

		// QUESTION: Why doesn't the screen show if the view matrix is not updated every frame?..

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents(); 
	}

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

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

