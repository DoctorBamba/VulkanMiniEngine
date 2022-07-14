#pragma once

#include "CPcInput.h"
#include "CWindow.h"
#include "CGraphics.h"

class CApplication
{
	private:
		std::string m_AppName;
		bool		b_Running;

		CWindow*	p_RootWindow;

	public:

		CApplication();
		~CApplication();

		Int Run();

		CWindow* GetRootWindow() { return p_RootWindow; };
};

extern CApplication* Application;