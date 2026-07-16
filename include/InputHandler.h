#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_ 

#include <vector>

#include "glad/glad.h"
#include "Particles.h"
#include "Command.h"
#include "Shader.h"

class InputHandler {
public:
	// allow for controls editting by user
	InputHandler(GLFWwindow* win);
	void processInput(GLFWwindow* window, std::vector<Command*>& cmds, Particles& particles, Shader* shaderProg = nullptr);
	~InputHandler();
	// TODO: Add a function that manages re-mapping which button pertains to which action/command.

	bool blinn; // toggles blinn-phong shading
private:
	Command* buttonW;
	Command* buttonA;
	Command* buttonS;
	Command* buttonD;
	Command* buttonE;
	Command* buttonQ;
	Command* buttonEsc;
	Command* buttonSpace;
	Command* noButton;

	float mixVal;
};
#endif