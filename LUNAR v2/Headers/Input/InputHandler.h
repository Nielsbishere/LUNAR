#pragma once
#include "../Math/MathDefines.h"

#ifdef __RASPBERRY__
#include <pthread.h>
#endif

#include "OsomiKeys.h"

namespace osomi{
	class InputHandler{
	private:
		bool currKeys[256], prevKeys[256];
		bool currMouse[8], prevMouse[8];
		float cursorX, cursorY;
		float prevX, prevY;
		
		bool ded;
		
		#ifdef __RASPBERRY__
		pthread_t keyboard, mouse;
		#endif
		
		InputHandler();
	public:
		bool isKeyDown(u8 handle){ return currKeys[handle]; }
		bool isKeyUp(u8 handle){ return !currKeys[handle]; }
		bool isKeyPressed(u8 handle){ return currKeys[handle] && !prevKeys[handle]; }
		bool isKeyReleased(u8 handle){ return !currKeys[handle] && prevKeys[handle]; }
		
		bool isMouseDown(u8 handle){ return currMouse[handle]; }
		bool isMouseUp(u8 handle){ return !currMouse[handle]; }
		bool isMousePressed(u8 handle){ return currMouse[handle] && !prevMouse[handle]; }
		bool isMouseReleased(u8 handle){ return !currMouse[handle] && prevMouse[handle]; }
		
		void addCursorPos(float x, float y){ cursorX += x; cursorY += y; }
		void getCursorPos(float &x, float &y){ x = cursorX; y = cursorY; }
		void setCursorPos(float x, float y){ cursorX = x; cursorY = y; }
		
		void updateMouse(u8 button, bool state){ currMouse[button] = state; }
		void updateKey(u8 key, bool state){ currKeys[key] = state; }
		
		bool isAlive(){ return !ded; }

		void getDelta(float &x, float &y) { x = cursorX - prevX; y = cursorY - prevY; }
		
		void close(){ ded = true; }
		void update();	
		~InputHandler();
		
		static u8 getKeyCode(u32 someCode);
		
		#ifdef __RASPBERRY__
		static InputHandler *init();
		#else
		static InputHandler *init(GLFWwindow *w);
		#endif
	};
}