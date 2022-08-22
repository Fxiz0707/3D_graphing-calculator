#pragma once
#include <iostream>
#include <vector> 
#include <string> 
#include <stack>
#include <cmath> 
#include <cassert> 
#include <utility>

#include "glm/glm.hpp"

class GraphLogic
{
public:

	/**
	 * \brief Takes in abstract graph data and generates an array of coordinates form this data, from which the graph can be draw 
	 * \param input - it is assumed that the input list provided has already been validated by the input handler class 
	 * \param setting - Either a 1, 2 or 3 (There are three performance settings) 
	 * \return a vector that contains all points needed to draw the graph
	 */
	static std::pair<std::vector<glm::vec3>, std::vector<unsigned int>> sample_points(std::vector<std::string> input, int setting);

private:

	static bool is_operator(const char& characterToCheck); // checks if the character provided as an argument, is a valid operator 

	/**
	 * \brief 
	 * \param input - will be in reverse polish notation, but will only contain numerical values as opposed to 'x' and 'y'
	 * \return 
	 */
	static float eval_postfix_expression(std::vector<std::string> input); // we are assuming that the input has already been validated 
};



