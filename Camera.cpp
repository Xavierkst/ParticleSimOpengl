#include "Camera.h"

// Takes current yaw and pitch angles and converts it to a cam front/cam dir vector
void Camera::updateCameraVectors() {
	glm::vec3 dir;
	dir.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	dir.y = glm::sin(glm::radians(pitch));
	dir.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	setFront(glm::normalize(dir));
	camRight = glm::normalize(glm::cross(camFront, worldUp));
	camUp = glm::normalize(glm::cross(camRight, camFront));
}

Camera::Camera(glm::vec3 pos, glm::vec3 target, glm::vec3 up, float y, float p) :
	camPos(pos), camFront(target), camUp(up), worldUp(glm::vec3(.0f, 1.0f, .0f)), gravity(-9.8f), fov(45.0f), pitch(p), yaw(y), grounded(true) {
	updateCameraVectors();
}

void Camera::ProcessKeyboard(const CameraMovement dir, const float deltaTime) {
	glm::vec3 camRight(glm::normalize(glm::cross(getFront(), getUp())));
	velo = SPEED * deltaTime;
	switch (dir) {
	case FORWARD:
		setPos(getPos() + (velo * glm::vec3(getFront())));
		break;
	case BACKWARD:
		setPos(getPos() - (velo * glm::vec3(getFront())));
		break;
	case LEFT:
		setPos(getPos() - (velo * glm::vec3(camRight)));
		break;
	case RIGHT:
		setPos(getPos() + (velo * glm::vec3(camRight)));
		break;
	case UP:
		setPos(getPos() + (velo * glm::vec3(.0f, 1.0f, .0f)));
		break;
	case DOWN:
		setPos(getPos() - (velo * glm::vec3(.0, 1.0f, 0.0f)));
		break;
	case JUMP: {
		veloY = SPEED;
		grounded = false;
		break;
	}
	default:
		break;
	}
}

void Camera::ProcessMouseMovement(const float dx, const float dy, const GLboolean constrainPitch) {
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

void Camera::ProcessMouseScroll(const float yoffset) {
	setFov(getFov() - float(yoffset));

	if (getFov() > 45.0f) {
		setFov(45.0f);
	}
	if (getFov() < 1.0f) {
		setFov(1.0f);
	}
}

void Camera::applyGravity(const float& deltaT) {
	if (!grounded) {
		glm::vec3 newPos = getPos() + veloY * deltaT * worldUp;
		veloY += gravity * deltaT;
		// clamp new position to never go below ground plane + set vertical velo back to 0
		if (newPos.y <= .0f) {
			newPos.y = .0f;
			veloY = .0f;
			grounded = true;
		}
		setPos(newPos);
	}
}

bool Camera::isGrounded() const { return grounded; }

glm::mat4 Camera::getViewMatrix() const {
	return glm::lookAt(getPos(), getPos() + getFront(), getUp());
}

glm::mat4 Camera::getViewProjMatrix() const
{
	// TODO: call getViewMatrix, L-multiply with glm::perspective()
	// requires shifting screen width & height into ctor
	return glm::mat4(1.0f);
}

glm::vec3 Camera::getPos() const {
	return camPos;
}

glm::vec3 Camera::getFront() const {
	return camFront;
}

glm::vec3 Camera::getUp() const {
	return camUp;
}

float Camera::getFov() const {
	return fov;
}

float Camera::getYaw() const
{
	return yaw;
}

float Camera::getPitch() const
{
	return pitch;
}

void Camera::setFov(const float fv) {
	fov = fv;
}

void Camera::setPos(const glm::vec3 pos) {
	camPos = pos;
}

void Camera::setFront(const glm::vec3 target) {
	camFront = target;
}

void Camera::setUp(const glm::vec3 up) {
	camUp = up;
}

void Camera::setYaw(const float y) {
	yaw = y;
}

void Camera::setPitch(const float p) {
	pitch = p;
}
