#ifndef Camera_H
#define Camera_H
#include <glm/glm.hpp>

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
	void updateCameraVectors() {
		glm::vec3 dir;
		dir.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
		dir.y = glm::sin(glm::radians(pitch));
		dir.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
		setCamFront(glm::normalize(dir));
		camRight = glm::normalize(glm::cross(camFront, worldUp));
		camUp = glm::normalize(glm::cross(camRight, camFront));
	}

public: 
	Camera(glm::vec3 pos = glm::vec3(.0f, .0f, 3.0f), 
		glm::vec3 target = glm::vec3(.0f, .0f, -1.0f), glm::vec3 up = glm::vec3(.0f, 1.0f, .0f), 
		float y = YAW, float p = PITCH) : 
		camPos(pos), camFront(target), camUp(up), worldUp(glm::vec3(.0f, 1.0f, .0f)), gravity(-9.8f), fov(45.0f), pitch(p), yaw(y), grounded(true) {
		updateCameraVectors();
	}
	void ProcessKeyboard(const CameraMovement dir, const float deltaTime) {
		glm::vec3 camRight(glm::normalize(glm::cross(getCamFront(), getCamUp())));
		velo = SPEED * deltaTime;
		switch (dir) {
		case FORWARD:
			setCamPos(getCamPos() + (velo* glm::vec3(getCamFront().x, .0f, getCamFront().z)));
			break;
		case BACKWARD:
			setCamPos(getCamPos() - (velo * glm::vec3(getCamFront().x, .0f, getCamFront().z)));
			break;
		case LEFT:
			setCamPos(getCamPos() - (velo* glm::vec3(camRight.x, .0f, camRight.z)));
			break;
		case RIGHT:
			setCamPos(getCamPos() + (velo * glm::vec3(camRight.x, .0f, camRight.z)));
			break;
		case UP: {
			veloY = SPEED;
			grounded = false;
			break;
		}
		default:
			break;
		}
	}
	void ProcessMouseMovement(const float dx, const float dy, const GLboolean constrainPitch = true) {
		yaw = yaw + dx;
		pitch = pitch + dy;

		if (constrainPitch) {
			if (pitch > 89.0f) {
				pitch = 89.0f;
			}
			if (pitch < -89.0f) {
				pitch = -89.0f;
			}
		}
		updateCameraVectors();
	}
	void ProcessMouseScroll(const float yoffset) {
		setFov(getFov() - float(yoffset));

		if (getFov() > 45.0f) {
			setFov(45.0f);
		} 
		if (getFov() < 1.0f) {
			setFov(1.0f);
		}
	}
	void applyGravity(const float& deltaT) {
		if (!grounded) {
			glm::vec3 newPos = getCamPos() + veloY * deltaT * worldUp;
			veloY += gravity * deltaT;
			// clamp new position to never go below ground plane
			if (newPos.y <= .0f) { 
				newPos.y = .0f;
				veloY = .0f;
				grounded = true;
			}
			setCamPos(newPos);
		}
	}
	bool isGrounded() { return grounded; }
	glm::mat4 getViewMatrix() {
		return glm::lookAt(getCamPos(), getCamPos() + getCamFront(), getCamUp());
	}
	glm::vec3 getCamPos() {
		return camPos;
	}
	glm::vec3 getCamFront() {
		return camFront;
	}
	glm::vec3 getCamUp() {
		return camUp;
	}
	float getFov() {
		return fov;
	}
	void setFov(const float fv) {
		fov = fv;
	}
	void setCamPos(const glm::vec3 pos) {
		camPos = pos;
	}
	void setCamFront(const glm::vec3 target) {
		camFront = target;
	}
	void setCamUp(const glm::vec3 up) {
		camUp = up;
	}
};

#endif // Camera_H