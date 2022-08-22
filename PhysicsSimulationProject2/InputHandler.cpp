#include "InputHandler.h"

bool InputHandler::mIsInstantiated_ = false; 

InputHandler::InputHandler() :
	mMovementSpeed_(10.f),
	mSensitivity_(1.f),
	mXPrior_(0.f),
	mYPrior_(0.f),
	mFirstMouse_(true),
	mMouseIsHeld(false)
{
	assert(!InputHandler::mIsInstantiated_);
	InputHandler::mIsInstantiated_ = true;
}

InputHandler::~InputHandler()
{
	InputHandler::mIsInstantiated_ = false; 
}

void InputHandler::handle_glfw_input(GLFWwindow* window, Camera& camera, double dt) 
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) // The user is holding down their mouse (we should turn our camera) 
	{
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos); // get mouse positions from GLFW 

		// calculating the offset
		const float xOffset = xPos - mXPrior_; 
		const float yOffset = yPos - mYPrior_;

		camera.turn(-xOffset * mSensitivity_ * dt, -yOffset * mSensitivity_ * dt);

		mMouseIsHeld = true;
	}
	else
	{
		double xPos, yPos; 
		glfwGetCursorPos(window, &xPos, &yPos); // get mouse positions from GLFW 

		//if (mMouseIsHeld) // The user has released their mouse after holding it 
		//{

		//	// calculating the offset
		//	const float xOffset = xPos - mXPrior_;
		//	const float yOffset = yPos - mYPrior_;


		//	camera.turn(xOffset * mSensitivity_ * dt, yOffset * mSensitivity_ * dt); // use our camera class to now turn the camera 
		//}

		// if not already false, this will set mouseHeld to false 
		mMouseIsHeld = false; 

		// updating our member variables 
		mXPrior_ = xPos;
		mYPrior_ = yPos;
	}
}

/**
 * \brief This is the main function for handling the input in the text boxes and converting that input into an expresison that the computer can understand 
 * \param input This is the text that the user entered 
 * \param errorFlag  - Pointer to a bool, so that the main function can be notified if there has been an error 
 * \return 
 */
std::vector<std::string> InputHandler::verify_and_convert_function(std::string input, bool* errorFlag)
{
	if (input.length() == 0) // we return an empty function 
	{
		return {}; 
	}

	std::vector<std::string> errorOutput; 

	// check to ensure that the first character is not an operator 
	for (auto x : input)
	{
		if (is_operator(x))
		{
			*errorFlag = true; // This means that the input the user entered was false  
			return errorOutput; // C++ requires that we still return something 4
		}
		if (x != ' ' && x!= '(' && x != ')') // meaning we have reached first non-empty, non-parenthesis character that is NOT an operator 
		{
			break; 
		}
	}

	for (auto x : input) // parsing the input
	{
		// In english, this if statement is saying: It is not an operator, it is not a digit, it is not x, it is not y and it is not a space 
		if (!is_operator(x) && !isdigit(x) && (x != 'x') && (x != 'y') && (x != ' ') && (x != ')') && (x != '('))
		{
			std::cout << "The error lies here: " << x << std::endl; 

			*errorFlag = true; // This means that the input the user entered was false  
			return errorOutput; // C++ requires that we still return something 4
		}
	}
	if (!parenthesis_checker(input))
	{
		std::cout << "problem with parenthesis checker" << std::endl; 

		*errorFlag = true; // This means that the input the user entered was false  
		return errorOutput; // C++ requires that we still return something 4
	}

	char lastSeenCharacter = ' '; // we keep track of the last seen character barring spaces and parenthesis 

	for (auto x : input) // iterating through the user input 
	{
		if (is_operator(x))
		{
			if (is_operator(lastSeenCharacter)) // meaning two operators are seen in a row 
			{
				*errorFlag = true; // This means that the input the user entered was false  
				return errorOutput;  // C++ requires that we still return something 4
			}
			lastSeenCharacter = x; 
		}
		else if (x != ' ' && x != '(' && x != ')')
		{
			lastSeenCharacter = x; 
		}
	}

	// Now that our inputs have been validated, we can send our input to the shunting_yard_algorithm
	// This will convert our mathematical expression from infix notation to postfix notation
	// We can then apply logic on this data 

	
	return shunting_yard_algorithm(input); 
}

// https://github.com/rmonfort/Shunting_yard/blob/master/Shunting_yard/Source.cpp
bool InputHandler::is_operator(const char& character_to_check)
{
	switch (character_to_check)
	{
	case '^':
	case '*':
	case '/':
	case '+':
	case '-':
		return 1;
		break;
	default:
		return 0;
		break;
	}
}

bool InputHandler::is_left_associative(const char& operator_to_check)
{
	switch (operator_to_check)
	{
	case '*':
	case '/':
	case '+':
	case '-':
		return 1;
		break;
	default:
		return 0;
		break;
	}
}

int InputHandler::set_precedence(const char& operation)
{
	switch (operation)
	{
	case '^':
		return 4;
		break;
	case '*':
	case '/':
		return 3;
		break;
	case '+':
	case '-':
		return 2;
		break;
	default:
		return 0;
		break;
	}
}

bool InputHandler::is_left_parenthesis(const char& character_to_check)
{
	return character_to_check == '(' ? 1 : 0;
}

bool InputHandler::is_right_parenthesis(const char& character_to_check)
{
	return character_to_check == ')' ? 1 : 0;
}

bool InputHandler::a_parenthesis_exists_in_stack(std::stack<char> stack_to_check)
{
	while (!stack_to_check.empty())
	{
		if (is_left_parenthesis(stack_to_check.top()) || is_right_parenthesis(stack_to_check.top()))
		{
			return 1;
		}
		stack_to_check.pop();
	}
	return 0;
}

// https://github.com/rmonfort/Shunting_yard/blob/master/Shunting_yard/Source.cpp end 

/**
 * \brief returns true if the input expression has balanced parenthesis
 * \param input - expression to be checked
 * \return boolean 
 */
bool InputHandler::parenthesis_checker(std::string input)
{
	std::stack<char> stack;

	for (auto x : input)
	{
		if (x == '(')
		{
			stack.push('(');
		}
		else if (x == ')') // repeating for ) 
		{
			if (stack.empty()) return false; // this means there is no matching '(' character 

			stack.pop(); // we reach here if there is a match '(' character, we then pop this character from the stack
		}
	}
	if (!stack.empty()) return false; // this means that there is '(' characters left and no ')' characters to balance them

	return true;
}

std::string InputHandler::convert_implicit_expression_to_explicit(std::string input)
{
	for (int i = 0; i < input.size(); i++)
	{
		if ((input[i] == 'x' || input[i] == 'y') && i != 0)
		{
			// if the element behind our 'x' or 'y' is not an operator
			if (!is_operator(input[i - 1]) && input[i - 1] != ' ' && input[i - 1] != '(' && input[i - 1] != ')' && input[i - 1] != '.')
			{
				input.insert(i, "*");
			}
		}
	}

	return input;

}


/**
 * \brief Shunting Yard Algorithm adapted from https://github.com/rmonfort/Shunting_yard/blob/master/Shunting_yard/Source.cpp
 * \param input The explicit statement to be converted from infix to postfix form 
 * \return Postfix output 
 */
std::vector<std::string> InputHandler::shunting_yard_algorithm(std::string input)
{
	std::cout << "the input to the shunting yard algorithm: " << input << std::endl; 

	std::string expression = convert_implicit_expression_to_explicit(input); // Allows the user to input in implicit fuctions such as 2x + y, instead of 2*x + y

	std::cout << "The expression " << expression << std::endl; 

	assert(parenthesis_checker(expression));

	std::string number;
	std::stack<char> operator_stack;

	std::vector<std::string> outputVec;

	for (const auto& character : expression)
	{
		if (isblank(character))
		{
			continue;
		}

		if (isdigit(character) || character == '.' || character == 'x' || character == 'y') // if character is digit, decimal point, x or y append to number 
		{
			number += character;
			continue;
		}
		else if (is_operator(character) || character == ' ')
		{
			if (number != "") // if number isn't empty, output it
			{
				outputVec.push_back(number);
				number = "";
			}
			while (!operator_stack.empty())
			{
				int precedence_of_character = set_precedence(character);
				int precedence_of_operator_on_top_of_stack = set_precedence(operator_stack.top());

				if ((is_left_associative(character) && precedence_of_character <= precedence_of_operator_on_top_of_stack) || precedence_of_character < precedence_of_operator_on_top_of_stack)
				{
					outputVec.push_back(std::string(1, operator_stack.top())); // we need to convert the operator character into a string to append it to outputVec 
					operator_stack.pop();
				}
				else
				{
					break;
				}
			}
			operator_stack.push(character);
		}
		else if (is_left_parenthesis(character))
		{
			if (number != "")
			{
				outputVec.push_back(number);
				number = "";
			}
			operator_stack.push(character);
		}
		else if (is_right_parenthesis(character))
		{
			if (number != "")
			{
				outputVec.push_back(number);
				number = "";
			}
			while (!is_left_parenthesis(operator_stack.top())) // cycle through stack and search for matching parenthesis
			{
				outputVec.push_back(std::string(1, operator_stack.top())); // casting character to string 
				operator_stack.pop();
			}
			operator_stack.pop();
		}
	}
	if (number != "")
	{
		outputVec.push_back(number);
		number = "";
	}


	while (!operator_stack.empty())
	{
		outputVec.push_back(std::string(1, operator_stack.top()));
		operator_stack.pop();
	}

	// printing the result of the shunting yard algorithm:

	std::cout << "Result of shunting yard algorithm: " << std::endl; 
	for (auto x : outputVec)
	{
		std::cout << x << " "; 
	}
	std::cout << std::endl;

	// We now return our outputVec
	return outputVec; 
}