#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Camera
{
	glm::vec3 mPosition_;
	float mPitch_;
	float mYaw_;
	glm::vec3 mForwardVector_;

	void update_position();

public: 
	Camera(glm::vec3 position, float pitch, float yaw);

	glm::mat4 get_view_matrix() const; // gets the matrix required to transform the camera to the desired position and orientation

	void move_forward(float movementDistance);
	void move_backward(float movementDistance);
	void move_left(float movementDistance);
	void move_right(float movementDistance);

	void turn(float pitchOffset, float yawOffset); // turns the camera by the given parameters 
};


