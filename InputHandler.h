#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_ 

#include "Command.h"
#include <GLFW/glfw3.h>
#include <vector>

class InputHandler {
public:
	// allow for controls editting by user
	InputHandler(GLFWwindow* win);
	std::vector<Command*> processInput(GLFWwindow* window);
	~InputHandler();

	// TODO: Add a function that manages re-mapping which button pertains to which action/command.

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