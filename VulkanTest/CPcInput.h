#pragma once
/*
#include "GLFW/glfw3.h"

#define KEYBOARED_KEYS_COUNTS 350

typedef unsigned short Keycode;

enum class Buttonstate
{
	Released,
	Pressed
};

enum class Keystate
{
	Released,
	Pressed
};

class CPcInputState
{
	private:
		Buttonstate mouse_left;
		Buttonstate mouse_right;

		unsigned short mouse_x;
		unsigned short mouse_y;
		short mouse_displacement_x;
		short mouse_displacement_y;

		Keystate key[KEYBOARED_KEYS_COUNTS];

	public:

		CPcInputState()
		{
			mouse_left  = Buttonstate::Released;
			mouse_right = Buttonstate::Released;

			mouse_x				 = 0;
			mouse_y				 = 0;
			mouse_displacement_x = 0;
			mouse_displacement_y = 0;

			memset(&key[0], 0, sizeof(Keycode) * KEYBOARED_KEYS_COUNTS);
		}

		void UpdateCursorPosition(double xpos_, double ypos_)
		{
			unsigned short new_mouse_x = xpos_;
			unsigned short new_mouse_y = ypos_;

			mouse_displacement_x = new_mouse_x - mouse_x;
			mouse_displacement_y = new_mouse_y - mouse_y;

			mouse_x = new_mouse_x;
			mouse_y = new_mouse_y;
		}

		void UpdateMouseState(UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
				case WM_MOUSEMOVE:
				{
					unsigned short new_mouse_x = LOWORD(lParam);
					unsigned short new_mouse_y = HIWORD(lParam);

					mouse_displacement_x = new_mouse_x - mouse_x;
					mouse_displacement_y = new_mouse_y - mouse_y;

					mouse_x = new_mouse_x;
					mouse_y = new_mouse_y;
					break;
				}

				case WM_LBUTTONDOWN:
					left = APP_BUTTON_PRESSED;
					break;
				case WM_LBUTTONUP:
					left = APP_BUTTON_REALLESED;
					break;
				case WM_RBUTTONDOWN:
					right = APP_BUTTON_PRESSED;
					break;
				case WM_RBUTTONUP:
					right = APP_BUTTON_REALLESED;
					break;
			}
		}

		void UpdateKeyState(int key_, int scancode_, int action_, int mods_)
		{
			switch (key_)
			{
				case GLFW_PRESS:
					key[(Keycode)(key_)] = Keystate::Pressed;
					break;
				case GLFW_RELEASE:
					key[(Keycode)(key_)] = Keystate::Released;
					break;
			}
		}

		bool IsMouseLeftPressed() { return (bool)(mouse_left == Buttonstate::Pressed); }
		bool IsNouseLeftReleased() { return (bool)(mouse_left == Buttonstate::Released); }

		unsigned short GetMouseX() { return mouse_x; }
		unsigned short GetMouseY() { return mouse_y; }
		
		bool IsKeyPressed(const Keycode keycode) { return (bool)(key[keycode] == Keystate::Pressed); }
		bool IsKeyReleased(const Keycode keycode) { return (bool)(key[keycode] == Keystate::Released); }
};

*/