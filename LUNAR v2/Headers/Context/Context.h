#pragma once

#include "../Math/Types.h"

#ifdef __EGL__
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <EGL/eglext.h>
#endif

#ifdef __RASPBERRY__
#include <assert.h>
#include "bcm_host.h"
#endif

#include <string>

namespace osomi{
	#ifdef __EGL__
		struct TargetState {
			u32 width;
			u32 height;

			EGLDisplay display;
			EGLSurface surface;
			EGLContext context;

			EGL_DISPMANX_WINDOW_T nativewindow;
			void(*draw_func)(struct Target_State*);
		};
	#endif
	class Context {
		friend class Game;
	public:
		static Context *createContext(u32 w, u32 h, std::string title);
	private:
		static void OGLDefaults(u32 w, u32 h);
	protected:
		#ifdef __EGL__
		TargetState *t;
		#else
		GLFWwindow *w;
		#endif
		~Context(){
			#ifdef __EGL__
			delete t;
			#else
			glfwDestroyWindow(w);
			#endif
		}
	};
}