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
	virtual void execute(Camera& actor, float deltaTime);
};

class JumpCommand : public Command {
public:
	virtual void execute(Camera& actor, float deltaTime);
};

class MoveCommand : public Command {
public:
	MoveCommand(CameraMovement d);
	virtual void execute(Camera& actor, float deltaTime);
	~MoveCommand();

private: 
	CameraMovement dir;
};

class CloseWinCommand : public Command {
public:
	CloseWinCommand(GLFWwindow* win);
	~CloseWinCommand();
	virtual void execute(Camera& actor, float deltaTime);
private:
	GLFWwindow* window;
};
#endif
