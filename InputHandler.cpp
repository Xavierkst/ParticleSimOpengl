#include "InputHandler.h"

// allow for controls editting by user
InputHandler::InputHandler(GLFWwindow* win) {
	buttonW = new MoveCommand(FORWARD);
	buttonA = new MoveCommand(LEFT);
	buttonS = new MoveCommand(BACKWARD);
	buttonD = new MoveCommand(RIGHT);
	buttonE = new MoveCommand(UP);
	buttonQ = new MoveCommand(DOWN);
	buttonEsc = new CloseWinCommand(win);
	buttonSpace = new JumpCommand();
	noButton = new NoCommand();
                                
	mixVal = .0f;
	blinn = false;
}

void InputHandler::processInput(GLFWwindow* window, std::vector<Command*>& cmds, Particles& particles, Shader* shaderProg) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		cmds.push_back(buttonEsc);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cmds.push_back(buttonW);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cmds.push_back(buttonA);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cmds.push_back(buttonS);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cmds.push_back(buttonD);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		cmds.push_back(buttonE);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		cmds.push_back(buttonQ);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		cmds.push_back(buttonSpace);
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		if (shaderProg) {
			blinn = true;
			shaderProg->use();
			shaderProg->setBool("blinn", blinn);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
		if (shaderProg) {
			blinn = false;
			shaderProg->use();
			shaderProg->setBool("blinn", blinn);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (shaderProg) {
			mixVal += .01f;
			if (mixVal > 1.0f)
				mixVal = 1.0f;
			shaderProg->use();
			shaderProg->setFloat("mixValue", mixVal);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (shaderProg) {
			mixVal -= .01f;
			if (mixVal < .0f)
				mixVal = .0f;
			shaderProg->use();
			shaderProg->setFloat("mixValue", mixVal);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		particles.togglePause(true);
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		particles.togglePause(false);
	}
	// Apply gravity on the player always
	// cmds.push_back(noButton);
}

InputHandler::~InputHandler() {
	delete buttonW;
	delete buttonA;
	delete buttonS;
	delete buttonD;
	delete buttonEsc;
	delete buttonSpace;
	delete noButton;
}
