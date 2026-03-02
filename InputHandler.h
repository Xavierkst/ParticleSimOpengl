#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_ 

#include "Command.h"
#include <GLFW/glfw3.h>

class InputHandler {
public:
	// allow for controls editting by user
	InputHandler(GLFWwindow* win) {
		buttonW = new MoveCommand(FORWARD);
		buttonA		= new MoveCommand(LEFT);
		buttonS		= new MoveCommand(BACKWARD);
		buttonD		= new MoveCommand(RIGHT);
		buttonEsc	= new CloseWinCommand(win);
		buttonSpace	= new JumpCommand();
		noButton	= new NoCommand();
	}

	std::vector<Command*> processInput(GLFWwindow* window) {
		std::vector<Command*> cmds;

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
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			cmds.push_back(buttonSpace);
		}
		// Apply gravity on the player always
		cmds.push_back(noButton);

		return cmds;
	}

	~InputHandler() {
		delete buttonW;
		delete buttonA;
		delete buttonS;
		delete buttonD;
		delete buttonEsc;
		delete buttonSpace;
		delete noButton;
	}

private:
	Command* buttonW;
	Command* buttonA;
	Command* buttonS;
	Command* buttonD;
	Command* buttonEsc;
	Command* buttonSpace;
	Command* noButton;
};
#endif