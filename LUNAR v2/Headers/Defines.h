#pragma once

namespace osomi{
	class RenderObject;
	class Defines {
	public:
		template<class T, class K> static bool instanceof(K *v) {
			return dynamic_cast<T*>(v) != nullptr;
		}
	
	};
}

#ifdef __arm__
#define __RASPBERRY__
#define __EGL__
#define __EGL2__
#endif

#ifdef _WIN32
#define __OGL__
#endif


#if defined(__linux__) || (defined(__APPLE__) && defined(TARGET_OS_MAC))
#define __UNIX__
#define slp(x) sleep(x)
#else
#define slp(x) Sleep(x)
#endif

#ifdef __EGL2__
#include <GLES2/gl2.h>
#endif
#ifdef __OGL__
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif