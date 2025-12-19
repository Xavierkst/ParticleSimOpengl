#ifndef Camera_H
#define Camera_H
#include <glm/glm.hpp>

class Camera {
public: 
	Camera(glm::vec3 pos, glm::vec3 target, glm::vec3 up) : 
		camPos(pos), camFront(target), camUp(up), fov(45.0f), pitch(0.0f), yaw(-90.0f) {
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
	glm::vec3 camPos, camFront, camUp;
	float fov;
	double pitch;
	double yaw;

};

#endif // Camera_H