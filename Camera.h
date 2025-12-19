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
	RIGHT
};

class Camera {
public: 
	Camera(glm::vec3 pos = glm::vec3(.0f, .0f, 3.0f), 
		glm::vec3 target = glm::vec3(.0f, .0f, -1.0f), glm::vec3 up = glm::vec3(.0f, 1.0f, .0f), 
		float y = YAW, float p = PITCH) : 
		camPos(pos), camFront(target), camUp(up), worldUp(glm::vec3(.0f, 1.0f, .0f)), fov(45.0f), pitch(p), yaw(y) {
		updateCameraVectors();
	}
	void ProcessKeyboard(CameraMovement dir, float deltaTime) {
		glm::vec3 camRight(glm::normalize(glm::cross(getCamFront(), getCamUp())));
		float velocity = SPEED * deltaTime;
		switch (dir) {
		case FORWARD:
			setCamPos(getCamPos() + (velocity * getCamFront()));
			break;
		case BACKWARD:
			setCamPos(getCamPos() - (velocity * getCamFront()));
			break;
		case LEFT:
			setCamPos(getCamPos() - (velocity * camRight));
			break;
		case RIGHT:
			setCamPos(getCamPos() + (velocity * camRight));
			break;
		default:
			break;
		}
	}
	void ProcessMouseMovement(float dx, float dy, GLboolean constrainPitch = true) {
		setYaw(getYaw() + dx);
		setPitch(getPitch() + dy);

		if (constrainPitch) {
			if (getPitch() > 89.0f) {
				setPitch(89.0f);
			}
			if (getPitch() < -89.0f) {
				setPitch(- 89.0f);
			}
		}
		updateCameraVectors();
	}
	void ProcessMouseScroll(float yoffset) {
		setFov(getFov() - float(yoffset));

		if (getFov() > 45.0f) {
			setFov(45.0f);
		} 
		if (getFov() < 1.0f) {
			setFov(1.0f);
		}
	}
	glm::mat4 getViewMatrix() {
		return glm::lookAt(getCamPos(), getCamPos() + getCamFront(), getCamUp());
	}
	void setCamPos(glm::vec3 pos) {
		camPos = pos;
	}
	glm::vec3 getCamPos() {
		return camPos;
	}
	void setCamFront(glm::vec3 target) {
		camFront = target;
	}
	glm::vec3 getCamFront() {
		return camFront;
	}
	void setCamUp(glm::vec3 up) {
		camUp = up;
	}
	glm::vec3 getCamUp() {
		return camUp;
	}
	float getFov() {
		return fov;
	}
	void setFov(float fv) {
		fov = fv;
	}
	void setPitch(double p) {
		pitch = p;
	}
	double getPitch() {
		return pitch;
	}
	void setYaw(double y) {
		yaw = y;
	}
	double getYaw() {
		return yaw;
	}

private: 
	glm::vec3 camPos, camFront, camUp, worldUp, camRight;
	float fov;
	double pitch;
	double yaw;

	void updateCameraVectors() {
		glm::vec3 direction;
		direction.x = cos(glm::radians(getYaw()))*cos(glm::radians(getPitch()));
		direction.y = sin(glm::radians(getPitch()));
		direction.z = sin(glm::radians(getYaw()))*cos(glm::radians(getPitch()));
		setCamFront(glm::normalize(direction));
		camRight = glm::normalize(glm::cross(camFront, worldUp));
		camUp = glm::normalize(glm::cross(camRight, camFront));
	}

};

#endif // Camera_H