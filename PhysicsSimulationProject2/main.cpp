#include "glad/glad.h"
#include "GLFW/glfw3.h" 

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <utility>
#include <array>
#include <fstream>

#include "vector.h"
#include "GraphLogic.h"

#include "InputHandler.h"

// GLOBAL VARIABLES, const because they will never change 
static const unsigned int height = 600;
static const unsigned int width = 800; 
static const unsigned int fps = 144;

// settings

static bool shouldDisplaySettings = false; 
static bool shouldSaveOnExit = true; 
static unsigned int performanceSetting = 3; 

static std::array<unsigned int, 10> functionVertexArrayObjects; 

GLFWwindow* window_init(); // declaring our function signature 

void exit(); 

// https://github.com/ocornut/imgui (Retrieved from their example code) 
static void help_marker(const char* desc) 
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

/**
 * \brief function that will be used to create the help marker and colour label that is next to the input box for each graph 
 * \param index Used to decide the colour so that we can colour the graph and the button label in the same colour
 */
void graph_helper_marker_and_icon(int index)
{
	ImGui::SameLine(); // ensures all the widgets are on the same line 
	help_marker("Enter any polynomial or rational function with variables 'x' and 'y'"); // Made to help the user
	ImGui::SameLine();
	float t = index / 9.f;

	float h; 
	float s;
	float v; 
	ImGui::ColorConvertRGBtoHSV(pow(1 - t, 2), 2 * (1 - t) * t, pow(t, 2), h, s, v); // convert RGB colours to HSV 

	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(h, s, v)); // pushing colour setting to ImGui STACK 
	ImGui::Button("##", ImVec2(17, 17)); // creating a button that has an empty label 
	ImGui::PopStyleColor(1); // popping our colour setting off the stack 
}


/**
 * \brief Update the vertex array object of a specific graph 
 * \param i The index of the VBO being updated 
 * \param userInput given in infix form. This input has NOT been validated 
 */
void update_current_function_data(unsigned int i, std::string userInput)
{
	bool errorFlag = false; // The error flag is originally set to false

	std::vector<std::string> postfixExpression = InputHandler::verify_and_convert_function(userInput, &errorFlag); 

	if (errorFlag) return; // The program should not update the VAO if the graph provided by the user is INVALID

	glBindVertexArray(functionVertexArrayObjects[i]); // binding the vertex array object to the openGL context

	std::pair<std::vector<glm::vec3>, std::vector<unsigned int>> graphData = GraphLogic::sample_points(postfixExpression, performanceSetting); 

	unsigned int vbo; // vbo = vertex buffer object 
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // Binding our vertex buffer to the vertex array object 
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * graphData.first.size(), graphData.first.data(), GL_STATIC_DRAW); // preparing our vertex data that will be sent to the GPU

	unsigned int ebo; // ebo = element buffer object
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * graphData.second.size(), graphData.second.data(), GL_STATIC_DRAW); // preparing data to send to GPU

	std::cout << "Size of EBO: " <<  graphData.second.size() << std::endl;


	// The GPU is given a stream of data but does not know how to deal with it
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0); //  SEPCIFY sizeof(GL_FLOAT) * 3 because we are not sent additional texture data or normal vector data

	glEnableVertexAttribArray(0); 
}

char main()
{
	GLFWwindow* window = window_init(); 

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	///
	/// Handling Shaders 
	///

	std::ifstream inFile;
	std::stringstream stream;

	std::string vertexShaderString;
	std::string fragmentShaderString; 

	// Shaders require their code as a cString
	// Here we from our shader files and convert the text inside them into a cString
	inFile.open("vertex_shader.txt");  
	stream << inFile.rdbuf(); // reading the vertex_shader.txt file into our string stream 
	vertexShaderString = stream.str(); // reading the string stream into our vertexShaderString Object 
	const char* vertexShaderCString = vertexShaderString.c_str(); 
	stream.str(""); 
	inFile.close(); 

	// There are two shaders that OpenGL requires from us 
	inFile.open("fragment_shader.txt");
	stream << inFile.rdbuf();
	fragmentShaderString = stream.str();
	const char* fragmentShaderCString = fragmentShaderString.c_str(); 


	unsigned int vertexShaderObject = glCreateShader(GL_VERTEX_SHADER); // creating a Shader object 
	glShaderSource(vertexShaderObject, 1, &vertexShaderCString, NULL); // passing our cString into our shader Object 
	glCompileShader(vertexShaderObject); // compiling our shader 

	int success; 
	char log[512];

	// checking shader compilation status 
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderObject, 512, NULL, log);
		std::cout << log << std::endl; 
	}

	// The same needs to be done for the fragment shader 

	unsigned int fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER); 
	glShaderSource(fragmentShaderObject, 1, &fragmentShaderCString, NULL);
	glCompileShader(fragmentShaderObject);

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderObject, 512, NULL, log);
		std::cout << log << std::endl;
	}

	// creating, linking and using Shader Program 

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShaderObject);
	glAttachShader(shaderProgram, fragmentShaderObject);
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	for (unsigned int& vaoId : functionVertexArrayObjects) // getting the reference 
	{
		glGenVertexArrays(1, &vaoId); 
	}

	//
	Camera camera(glm::vec3(0.f, 0.f, 3.f), 0.f, -90.f);
	InputHandler inputHandler; 

	// Matrices and Cameras
	
//	glm::mat4 model = glm::rotate(glm::mat4(1.f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 model = glm::mat4(1.f); 

	glm::mat4 view = camera.get_view_matrix();
	glm::mat4 projection = glm::perspective(glm::radians(65.f), width / (float)height, 0.1f, 100.f); 

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection)); 

	char** buffArr = new char*[10];
	for (int i = 0; i < 10; i++)
	{
		buffArr[i] = new char[256]; 
		std::fill(buffArr[i], buffArr[i] + 256, NULL); 
	}

	std::ifstream inputFile;
	inputFile.open("SavedGraphs.txt", std::fstream::app); // automatically creates the file if it is not already created 

	for (int i = 0; i < 10; i++) // we want to get the first 10 lines 
	{
		std::string equation;
		getline(inputFile, equation); // getting the next line from the file and saving the line into equation 

		std::cout << "Equation: " << equation << std::endl; 

		// we dont want to call update using an empty expression! 
		if (equation.length() != 0)
		{
			assert(i <= 9); // abort() incase we are trying to do an illegal access of an array

			strcpy_s(buffArr[i], sizeof(char) * 256, equation.c_str()); // we do a safe string copy
			update_current_function_data(i, equation);
		}
	}

	while (!glfwWindowShouldClose(window))
	{
		static double lastFrame = glfwGetTime();

		double currentFrame = glfwGetTime(); 
		double deltaTime = currentFrame - lastFrame;

		if (deltaTime >= 1 / fps)
		{
			//std::cout << "FPS: " << 1 / deltaTime << std::endl;

			inputHandler.handle_glfw_input(window, camera, deltaTime);
			view = camera.get_view_matrix();
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

			lastFrame = currentFrame; 
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		glEnable(GL_DEPTH_TEST); 

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 

		for (unsigned int vao : functionVertexArrayObjects)
		{
			glBindVertexArray(vao); // bind the desired VAO to the OpenGL context

			float t = vao / 9.f;
			glm::vec3 colour = { pow(1 - t, 2), 2 * (1 - t) * t,  pow(t, 2) };

			glUniform3fv(glGetUniformLocation(shaderProgram, "graphColor"), 1, glm::value_ptr(colour)); 

			int triangleCount = 0; 
			if (performanceSetting == 3)
			{
				triangleCount = 37446; 
			}
			if (performanceSetting == 1)
			{
				triangleCount = 2166; 
			}

			glDrawElements(GL_TRIANGLES, 37446, GL_UNSIGNED_INT, 0);
		}

		// IMGUI new frame 

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// ImGui components 
		ImGui::Begin("Input Graphs");
		ImGui::SetCursorPos(ImVec2(0, 0));

		// Creating a buffer so that inputs given to text boxes can be stored somewhere

		ImGuiInputTextFlags textFlags = 1 << 5;

		char nullString[1] = "";

		ImGui::InputTextWithHint("##text0", "Graph 1", nullString, 0, textFlags);

	/*	for (int i = 0; i < 7; i++)
		{
			if (i > 0)
				ImGui::SameLine();
			ImGui::PushID(i);
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
			ImGui::Button("Click");
			ImGui::PopStyleColor(3);
			ImGui::PopID();
		}*/

		if (ImGui::InputTextWithHint("##text1", "Graph 1", buffArr[0], sizeof(char) * 256, textFlags)) update_current_function_data(0, buffArr[0]);
		graph_helper_marker_and_icon(1); 
		if (ImGui::InputTextWithHint("##text2", "Graph 2", buffArr[1], sizeof(char) * 256, textFlags)) update_current_function_data(1, buffArr[1]);
		graph_helper_marker_and_icon(2);
		if (ImGui::InputTextWithHint("##text3", "Graph 3", buffArr[2], sizeof(char) * 256, textFlags)) update_current_function_data(2, buffArr[2]);
		graph_helper_marker_and_icon(3);
		if (ImGui::InputTextWithHint("##text4", "Graph 4", buffArr[3], sizeof(char) * 256, textFlags)) update_current_function_data(3, buffArr[3]);
		graph_helper_marker_and_icon(4);
		if (ImGui::InputTextWithHint("##text5", "Graph 5", buffArr[4], sizeof(char) * 256, textFlags)) update_current_function_data(4, buffArr[4]);
		graph_helper_marker_and_icon(5);
		if (ImGui::InputTextWithHint("##text6", "Graph 6", buffArr[5], sizeof(char) * 256, textFlags)) update_current_function_data(5, buffArr[5]);
		graph_helper_marker_and_icon(6);
		if (ImGui::InputTextWithHint("##text7", "Graph 7", buffArr[6], sizeof(char) * 256, textFlags)) update_current_function_data(6, buffArr[6]);
		graph_helper_marker_and_icon(7);
		if (ImGui::InputTextWithHint("##text8", "Graph 8", buffArr[7], sizeof(char) * 256, textFlags)) update_current_function_data(7, buffArr[7]);
		graph_helper_marker_and_icon(8);
		if (ImGui::InputTextWithHint("##text9", "Graph 9", buffArr[8], sizeof(char) * 256, textFlags)) update_current_function_data(8, buffArr[8]);
		graph_helper_marker_and_icon(9);
		if (ImGui::InputTextWithHint("##text10", "Graph 10", buffArr[9], sizeof(char) * 256, textFlags)) update_current_function_data(9, buffArr[9]);
		graph_helper_marker_and_icon(10);

		if (ImGui::Button("Settings", ImVec2(80, 45)))
		{
			shouldDisplaySettings = true; 
		}

		if (shouldDisplaySettings)
		{
			ImGui::Begin("Settings");
			ImGui::Checkbox("Save on Exit", &shouldSaveOnExit);
			if (ImGui::CollapsingHeader("Graphics"))
			{
				static int x = 0; // we need a way to link the radio buttons together, this is done through x 

				if (ImGui::RadioButton("High", &x, 0)) // Radio button ensures that only one button can be pressed at a time 
				{
					performanceSetting = 3; 
				};
				ImGui::SameLine();
				help_marker("Only recommended for high performance computers"); // writing an aid for the user 

				if (ImGui::RadioButton("Medium", &x, 1))
				{
					performanceSetting = 2; 
				}
				ImGui::SameLine();
				help_marker("The recommended setting"); // writing an aid for the user 

				if (ImGui::RadioButton("Low", &x, 2))
				{
					performanceSetting = 1; 
				}
				ImGui::SameLine();
				help_marker("Setting Recommended if the application is lagging"); // writing an aid for the user 
			}

			if (ImGui::Button("Close Settings"))
			{
				shouldDisplaySettings = false; 
			}

			ImGui::End(); 
		}

		ImGui::End();

		// rendering imGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwPollEvents();
		glfwSwapBuffers(window); 
	}

	// Save the current Graph to a text file

	std::ofstream outputFile; 
	outputFile.open("SavedGraphs.txt", std::fstream::out || std::ofstream::trunc); // automatically creates the file if it is not already created 

	if (shouldSaveOnExit)
	{
		for (int i = 0; i < 10; i++)
		{
			outputFile << buffArr[i] << '\n'; // we insert a new line 
		}
		outputFile.close(); // close the file, good C++ practice 	
	}
	else
	{
		for (int i = 0; i < 10; i++)
		{
			outputFile << '\n'; // we insert a new line 
		}
		outputFile.close(); // close the file, good C++ practice 	
	}

	exit(); 

	glfwTerminate();
	return 0; 
}

/**
 * \brief packing all window initialisation code into a function, to avoid code clutter 
 * \return The pointer to the window object created 
 */
GLFWwindow* window_init()
{
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // Allows the window to be resizable 

	GLFWwindow* window = glfwCreateWindow(width, height, "3D Graph Visualiser", NULL, NULL); // Creating a window titles "3D Graph Visualiser" 

	if (window == nullptr) // if the window could not be created glfwCreateWindow will just return a nullptr 
	{
		std::cout << "Window Creation Failed" << std::endl;
		glfwTerminate();
		abort(); 
	}

	glfwMakeContextCurrent(window);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed Loading OpenGL Functions" << std::endl;
		abort(); 
	}

	glViewport(0, 0, width, height); 

	return window; 
}

// correclty closes the ImGUI context 
void exit()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext(); 
}


/**
 * \brief Callback function so that the window can be resized correctly
 * \param window The pointer to our main window
 * \param width The new width 
 * \param height The new heigh t
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}