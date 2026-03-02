#ifndef COMMAND_H_
#define COMMAND_H_

#include "Camera.h"
#include <GLFW/glfw3.h>

class Command {
public:
	virtual ~Command() {}
	virtual void execute(Camera& actor, float deltaTime) = 0;
};

class NoCommand : public Command {
public:
	// execute idle updates
	virtual void execute(Camera& actor, float deltaTime) {
		actor.applyGravity(deltaTime);
	}
};

class JumpCommand : public Command {
public:
	virtual void execute(Camera& actor, float deltaTime) {
		if (actor.isGrounded()) {
			actor.ProcessKeyboard(UP, deltaTime);
		}
	}
};

class MoveCommand : public Command {
public:
	MoveCommand(CameraMovement d) : dir(d) {}
	virtual void execute(Camera& actor, float deltaTime) {
		actor.ProcessKeyboard(dir, deltaTime);
	}
	~MoveCommand() {}

private: 
	CameraMovement dir;
};

class CloseWinCommand : public Command {
public:
	CloseWinCommand(GLFWwindow* win) : window(win) {
	}
	~CloseWinCommand() {}
	virtual void execute(Camera& actor, float deltaTime) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
private:
	GLFWwindow* window;
};
#endif
