#include "GraphLogic.h"
#include <iostream>

bool GraphLogic::is_operator(const char& characterToCheck)
{
    switch (characterToCheck)
    {
    case '^':
    case '+':
    case '-':
    case '*':
    case '/':
        return true;
    default:
        return false;
    }
}


float GraphLogic::eval_postfix_expression(std::vector<std::string> input)
{
    //assert(input.size() != 0); // calls abort() if a 0 length string is passed as the validation does not check for this 

    std::stack<std::string> stack; // A stack is used to decompose the postfix expression 

    for (std::string x : input)
    {
        if (x.size() == 1)
        {
            if (is_operator(x[0]))
            {
                // we do not need to check that the stack is not empty, because we assumed from the beginning that the provided input was in valid reverse polish notation 

                float operand2 = std::stof(stack.top()); // cast stack.top() from string to float 
                stack.pop();
                float operand1 = std::stof(stack.top()); // cast stack.top() from string to float 
                stack.pop();

                float result = 0;

                switch (x[0]) // we now apply the operator 
                {
                case '^':
                    result = pow(operand1, operand2);
                    break;
                case '+':
                    result = operand1 + operand2;
                    break;
                case '-':
                    result = operand1 - operand2;
                    break;
                case '*':
                    result = operand1 * operand2;
                    break;
                case '/':
                    result = operand1 / operand2;
                    break;
                default:
                    abort(); // Something went wrong 
                }
                stack.push(std::to_string(result));
            }
            else // fixing the issue by an additional condition 
            {
                stack.push(x);
            }
        }
        else // Because the size is not 1, we know that we have an operator  
        {
            stack.push(x);
        }
    }

    const std::string finalOutput = stack.top();

    return std::stof(finalOutput); // Return the final evaluation as a float 
}

/**
 * \brief This function generates the relevant data needed to visualise the graph, which can then be sent to the GPU
 * \param input The expression that the user wants visualised in postfix form 
 * \param setting The desired performance setting, will determine how MANY samples we will take 
 * \return 
 */
std::pair<std::vector<glm::vec3>, std::vector<unsigned int>> GraphLogic::sample_points(std::vector<std::string> input, int setting)
{
    if (input.size() == 0) return {}; // the user entered an empty expression 

    
    for (auto x : input)
    {
        std::cout << x << " "; 
    }
    std::cout << std::endl; 


    int sampleSize = 0; // we will be iterating x, y from -sampleSize / 2 to sampleSize / 2
    switch (setting)
    {
    case (1):
        sampleSize = 20;
        break; 
    case (2):
        sampleSize = 40;
        break; 
    case (3):
        sampleSize = 80; // A higher setting will result in more samples being taken
        break;
    default:
        abort(); // invalid setting input was entered 
    }


    std::vector<glm::vec3> outputPoints; // this will be the object that the function returns

    // We are using a dynamic array to which we could be appending 250,000 vec3, This will call many resizes which are expensive
    // Because we know how many points will be stored inside our array, beforehand, we can tell C++ to reserve space for us, avoiding expensive resize calls 
    outputPoints.reserve(sampleSize * sampleSize);

    int i = 0; 
    // iterating over the xy plane
    for (int x = -sampleSize / 2; x < sampleSize / 2; x++)
    {
        for (int y = -sampleSize / 2; y < sampleSize / 2; y++)
        {
            std::vector<std::string> tempArr = input;

            i++; 

            // Scaling our graph down 
            float xScaled = (float)x / (sampleSize / 10.f); 
            float yScaled = (float)y / (sampleSize / 10.f); 

            // we go through the string here, when ever we see x we replace it with the numerical value of x, vice versa for y
            for (int i = 0; i < tempArr.size(); i++)
            {
                if (input[i] == "x")
                {
                    tempArr[i] = std::to_string(xScaled);
                }
                else if (input[i] == "y")
                {
                    tempArr[i] = std::to_string(yScaled);
                }
            }

            // printing the tempArr

            float z = eval_postfix_expression(tempArr);


            outputPoints.push_back({ (float)xScaled, z, (float)yScaled }); // adding our coordinates to the array 
        }

    }

    std::cout << "i: " << i << std::endl; 

    assert(outputPoints.size() == sampleSize * sampleSize); // Ensuring that my calculations have not been wrong 

    // Creating Index Buffer Data 

    std::vector<unsigned int> indexBufferData;

    // We iterate till sampleSize - 1 as opposed to sampleSize, because we do not want to create triangles using the top boundary or the right boundary
    for (int i = 0; i < sampleSize - 1; i++)
    {
	    for (int j = 0; j < sampleSize - 1; j++)
	    {
            // here 'i' and 'j' are just logical indexes for a '2D' array even though our array is 3D in reality
            unsigned int index = i + j * sampleSize; // converting logical index into a physical index

            // using Method 1 (mentioned in documentation) 
            indexBufferData.push_back(index);
            indexBufferData.push_back(index + sampleSize + 1);
            indexBufferData.push_back(index + 1);

            // using Method 2 
            indexBufferData.push_back(index);
            indexBufferData.push_back(index + sampleSize);
            indexBufferData.push_back(index + sampleSize + 1); 
	    }
    }

    return { outputPoints, indexBufferData }; 
}