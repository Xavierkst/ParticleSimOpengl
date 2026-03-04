#include "Command.h"

// execute idle updates
void NoCommand::execute(Camera& actor, float deltaTime) {
	actor.applyGravity(deltaTime);
}

void JumpCommand::execute(Camera& actor, float deltaTime) {
	if (actor.isGrounded()) {
		actor.ProcessKeyboard(UP, deltaTime);
	}
}

MoveCommand::MoveCommand(CameraMovement d) : dir(d) {}

MoveCommand::~MoveCommand() {}

void MoveCommand::execute(Camera& actor, float deltaTime) {
	actor.ProcessKeyboard(dir, deltaTime);
}

CloseWinCommand::CloseWinCommand(GLFWwindow* win) : window(win) {
}

CloseWinCommand::~CloseWinCommand() {}

void CloseWinCommand::execute(Camera& actor, float deltaTime) {
	glfwSetWindowShouldClose(window, GL_TRUE);
}
