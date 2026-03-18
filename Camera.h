#ifndef Camera_H
#define Camera_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 4.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP
};

class Camera {
private: 
	glm::vec3 camPos, camFront, camUp, worldUp, camRight;
	float velo;
	float veloY = 0.0f;
	float gravity;
	float fov;
	double pitch;
	double yaw;
	bool grounded;

	// Takes current yaw and pitch angles and converts it to a cam front/cam dir vector
	void updateCameraVectors();

public: 
	// Because the camera looks in the -z direction (at the origin),
	// we want the positive Z dir of the cam to be opposite of where the cam is looking.
	Camera(glm::vec3 pos = glm::vec3(.0f, .0f, 3.0f), 
		glm::vec3 target = glm::vec3(.0f, .0f, -1.0f), 
		glm::vec3 up = glm::vec3(.0f, 1.0f, .0f), 
		float y = YAW, float p = PITCH);

	void ProcessKeyboard(const CameraMovement dir, const float deltaTime);
	void ProcessMouseMovement(const float dx, const float dy, const GLboolean constrainPitch = true);
	void ProcessMouseScroll(const float yoffset);
	void applyGravity(const float& deltaT);
	bool isGrounded() const;

	glm::mat4 getViewMatrix() const;
	glm::vec3 getPos() const;
	glm::vec3 getFront() const;
	glm::vec3 getUp() const;
	float getFov() const;

	void setFov(const float fv);
	void setPos(const glm::vec3 pos);
	void setFront(const glm::vec3 target);
	void setUp(const glm::vec3 up);
};

#endif // Camera_H