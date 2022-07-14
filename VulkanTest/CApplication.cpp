#include "CApplication.h"
#include "Engine/Engine_Events.h"
#include "Engine/Engine_WinConnection.h"



CApplication::CApplication()
{
	//Install glfw...
	if (!glfwInit())
	{
		throw std::runtime_error("GLEW Init failed!");
		return;
	}


	try
	{
		Engine::Install();
		p_RootWindow = new CWindow(1024, 768, "Vulkan", Engine::graphics);
	}
	catch (std::runtime_error& error)
	{
		printf("%s \n", error.what());
		OutputDebugStringA((std::string(error.what()) + '\n').c_str());
		return;
	}
}

Int CApplication::Run()
{
	Int result;
	
	try
	{
		result = p_RootWindow->Loop();
	}
	catch (const std::runtime_error& error)
	{
		printf("%s \n", error.what());
		OutputDebugStringA((std::string(error.what()) + '\n').c_str());
		int y = 5;
	}

	delete this;//Call distructor
	return result;
}

CApplication::~CApplication()
{
	delete p_RootWindow;

	Engine::Clean();
	glfwTerminate();
}