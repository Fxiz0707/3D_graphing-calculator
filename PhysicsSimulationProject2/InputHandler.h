#pragma once
#include <cassert>
#include <iostream>
#include <GLFW/glfw3.h>
#include <vector>
#include <stack>

#include "Camera.h"

class InputHandler 
{
public:
	InputHandler();
	~InputHandler();
	InputHandler(const InputHandler&) = delete;
	InputHandler(InputHandler&&) = delete;
	InputHandler& operator=(const InputHandler&) = delete; 
	InputHandler& operator=(InputHandler&&) = delete; 

	void handle_glfw_input(GLFWwindow* window, Camera& camera, double dt); 
	static std::vector<std::string> verify_and_convert_function(std::string input, bool* errorFlag); 

private:
	const float mMovementSpeed_; 
	const float mSensitivity_;

	// variables used to calculate graph interactions 
	double mXPrior_;
	double mYPrior_; 
	bool mFirstMouse_; 

	bool mMouseIsHeld;

private:
	static bool is_operator(const char& character_to_check); // checks if the character is an operator 
	static bool is_left_associative(const char& operator_to_check); // checks if the operator is left associative
	static int set_precedence(const char& operation); 
	static bool is_left_parenthesis(const char& character_to_check);
	static bool is_right_parenthesis(const char& character_to_check); 
	static bool a_parenthesis_exists_in_stack(std::stack<char> stack_to_check);
	static bool parenthesis_checker(std::string input); 

	static std::string convert_implicit_expression_to_explicit(std::string input);
	static std::vector<std::string> shunting_yard_algorithm(std::string input); 

private:
	static bool mIsInstantiated_; 
};


