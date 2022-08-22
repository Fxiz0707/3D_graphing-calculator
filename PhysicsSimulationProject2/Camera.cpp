#include "Camera.h"

Camera::Camera(glm::vec3 position, float pitch, float yaw) :
	mPosition_(position),
	mPitch_(pitch),
	mYaw_(yaw)
{
	update_position(); // we want to update the forward vector in the constructor, so that its initial position is valid
}

glm::mat4 Camera::get_view_matrix() const 
{
	// Here we use GLM to generate an orthographic projection 
	return glm::lookAt(mPosition_, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

void Camera::move_forward(float movementDistance)
{
	mPosition_ += movementDistance * mForwardVector_; // If the user pressed W 
}

void Camera::move_backward(float movementDistance)
{
	mPosition_ -= movementDistance * mForwardVector_; // If the user presses S 
}

void Camera::move_left(float movementDistance)
{
	mPosition_ -= movementDistance * glm::cross(mForwardVector_, glm::vec3(0, 1, 0)); // If the user presses A 
}

void Camera::move_right(float movementDistance)
{
	mPosition_ += movementDistance * glm::cross(mForwardVector_, glm::vec3(0, 1, 0)); // If the user pressed D
}

void Camera::turn(float pitchOffset, float yawOffset)
{

	mPitch_ -= pitchOffset;
	mYaw_ += yawOffset;

	update_position(); 
}

void Camera::update_position()
{
	// Using trigonometry to calculate the new position of the camera 
	float x = 5 * glm::cos(glm::radians(mYaw_)) * glm::cos(glm::radians(mPitch_));
	float y = 5 * glm::sin(glm::radians(mYaw_));
	float z = 5 * glm::cos(glm::radians(mYaw_)) * glm::sin(glm::radians(mPitch_));
	mPosition_ = glm::vec3(x, y, z); 
}

