#include "CApplication.h"
#include "Engine/Engine_Events.h"
#include "Engine/Engine_WinConnection.h"

FT_Library FT_FontLibrary;

CApplication::CApplication()
{
	//Install glfw...
	if (!glfwInit())
	{
		throw std::runtime_error("GLEW Init failed!");
		return;
	}

	if (FT_Init_FreeType(&FT_FontLibrary))
	{
		throw std::runtime_error("FreeType Init failed!");
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