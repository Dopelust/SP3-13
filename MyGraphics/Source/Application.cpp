
#include "Application.h"

//Include GLEW
#include <GL/glew.h>

//Include GLFW
#include <GLFW/glfw3.h>

//Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>

GLFWwindow* m_window;
const unsigned char FPS = 90; // FPS of this game	
const unsigned int frameTime = 1000 / FPS; // time for each frame

float Application::textspace[255];

int Application::mouseScroll = 0;

int Application::m_width;
int Application::m_height;

//Define an error callback
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

//Define the key input callback
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}
void scroll_callback(GLFWwindow* window, double x, double y)
{
	Application::mouseScroll = int(y);
}

void resize_callback(GLFWwindow* window, int w, int h)
{
	Application::m_width = w;
	Application::m_height = h;
	glViewport(0, 0, w, h);
}

bool Application::IsKeyPressed(unsigned short key)
{
    return ((GetAsyncKeyState(key) & 0x8001) != 0);
}

bool Application::IsMousePressed(unsigned short button) //0 - Left, 1 - Right, 2 - Middle
{
	return glfwGetMouseButton(m_window, button) != 0;
}

void Application::GetCursorPos(double *xpos, double *ypos)
{
	glfwGetCursorPos(m_window, xpos, ypos);
}

void Application::setWindowSize(double w, double h)
{
	m_width = w;
	m_height = h;
	glfwSetWindowSize(m_window, w, h);
}

Application::Application()
{
}

Application::~Application()
{
}

void Application::Init()
{
	//Set the error callback
	glfwSetErrorCallback(error_callback);
	
	//Initialize GLFW
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Set the GLFW window creation hints - these are optional
	glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 


	//Create a window and create its OpenGL context
	m_window = glfwCreateWindow(1920, 1080, "myFramework", NULL, NULL);
	glfwSetWindowPos(m_window, 100, 8);

	//If the window couldn't be created
	if (!m_window)
	{
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(m_window);

	glfwSetWindowSizeCallback(m_window, resize_callback);

	//Sets the key callback
	//glfwSetKeyCallback(m_window, key_callback);
	glfwSetScrollCallback(m_window, scroll_callback);

	glewExperimental = true; // Needed for core profile
	//Initialize GLEW
	GLenum err = glewInit();

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//If GLEW hasn't initialized
	if (err != GLEW_OK) 
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		//return -1;
	}

	m_dElapsedTime = 0.0;
	m_dAccumulatedTime_ThreadOne = 0.0;
	m_dAccumulatedTime_ThreadTwo = 0.0;
	Math::InitRNG();

	ifstream inData;
	string data;
	inData.open ("Assets//Font//ascii.txt"); 

	for (unsigned i = 0; i < 255; i++)
	{
		textspace[i] = 0.3f;
	}

	int index = 33;

	while (!inData.eof())
	{
		getline (inData, data);
		if (data == "")
		continue;

		int number = 0;

		for (unsigned i = 0; i < data.size(); i++)
		{
			if (data[i] == '=')
			{
				number = i + 1;
				break;
			}
		}

		textspace[index] = atoi(&data[number]) / 11.f;
		index++;
	}

	inData.close (); 
}

bool Application::getKeyboardUpdate()
{
	if(IsKeyPressed('W'))
		scene->UpdateInput('w');

	if(IsKeyPressed('A'))
		scene->UpdateInput('a');
	
	if(IsKeyPressed('S'))
		scene->UpdateInput('s');

	if(IsKeyPressed('D'))
		scene->UpdateInput('d');

	for (unsigned char i = '1'; i <= '9'; ++i)
	{
		if (IsKeyPressed(i))
			scene->UpdateInput(i);
	}

	if(IsKeyPressed(VK_SPACE))
		scene->UpdateInput(' ');
	
	if(IsKeyPressed(VK_SHIFT))
		scene->UpdateInput('S');

	if(IsKeyPressed(VK_CONTROL))
		scene->UpdateInput('C');
	
	if (IsKeyPressed(VK_RETURN))
		scene->UpdateInput('R');

	return true;
}

bool Application::getMouseUpdate()
{
	return true;
}

float Application::getTextWidth(string t)
{
	float textWidth = 0.f;
	for(unsigned i = 0; i < t.length(); ++i)
	{
		textWidth += textspace[t[i]];
	}
	return textWidth;
}

void Application::Run()
{
	scene = new MinScene;
	scene->Init();
	glfwSetCursorPos(m_window, double(m_width)/2, double(m_height)/2);

	m_timer.startTimer();    // Start timer to calculate how long it takes to render this frame
	while (!glfwWindowShouldClose(m_window) && !IsKeyPressed(VK_ESCAPE))
	{
		getKeyboardUpdate();
		scene->Update(m_timer.getElapsedTime());
		scene->Render();
		
		glfwSetCursorPos(m_window, double(m_width)/2, double(m_height)/2);
		//Swap buffers
		glfwSwapBuffers(m_window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...
		glfwPollEvents();
        //m_timer.waitUntil(frameTime);       // Frame rate limiter. Limits each frame to a specified time in ms.  
	}

	scene->Exit();
	delete scene;
}

void Application::Exit()
{
	//Close OpenGL window and terminate GLFW
	glfwDestroyWindow(m_window);
	//Finalize and clean up GLFW
	glfwTerminate();
}
