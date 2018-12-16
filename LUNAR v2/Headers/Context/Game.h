#pragma once

#include "../../Headers/Context/Context.h"
#include <chrono>
#include <vector>

namespace osomi{
	class InputHandler;
	class Camera;
	class Light;
	class Game{
	public:
		static Game *create(u32 w, u32 h, std::string name, void (*update)(double), void (*render)(), void (*renderForward)(), void (*postRender)(), Camera *(*getCamera)(), std::vector<Light>(*getLights)(), InputHandler **ih);
		~Game(){ if (c != nullptr) delete c; }
		void start();
		
		void setSize(u32 x, u32 y){
			if (prevW == x && prevH == y)return;
			newW = x;
			newH = y;
		}
		InputHandler *getIH() { return ih; }
	private:
		Game(){}
		
		Context *c;
		InputHandler *ih;
		
		int prevW, prevH;
		int newW, newH;

		bool hasFBOS;
		GLuint color, normal, depth, output, fbo;
		
		u64 frames;
		f64 time;
		
		void(*update)(double);
		void(*render)();
		void(*renderForward)();
		void(*postRender)();
		Camera *(*cam)();
		std::vector<Light>(*light)();

		void recreateFBOS();

		void renderScene(GLuint vbo, GLuint vao);
		
		std::chrono::high_resolution_clock::time_point t, t0;
	};
}