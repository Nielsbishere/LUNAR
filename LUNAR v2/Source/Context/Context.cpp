#include "../../Headers/Context/Context.h"

using namespace osomi;

#ifdef __RASPBERRY__

Context *Context::createContext(u32 w, u32 h, std::string title){
	 const EGLint attribute_list[] = {
		EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 5,
		
		EGL_STENCIL_SIZE, 8,
		EGL_SAMPLE_BUFFERS, 1,
		EGL_SAMPLES, 4,
		
		EGL_SURFACE_TYPE,
		EGL_WINDOW_BIT,
		EGL_NONE
	};

	const EGLint context_attributes[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	
	TargetState *stptr = new TargetState();
	TargetState &state = *stptr;
	memset(stptr, 0, sizeof(TargetState));
	
	EGLBoolean result;
	EGLint num_config;

	bcm_host_init(); //RPI needs this
	//RPI setup is a little different to normal EGL
	DISPMANX_ELEMENT_HANDLE_T DispmanElementH;
	DISPMANX_DISPLAY_HANDLE_T DispmanDisplayH;
	DISPMANX_UPDATE_HANDLE_T DispmanUpdateH;
	VC_RECT_T dest_rect;
	VC_RECT_T src_rect;
	EGLConfig config;

	//Init display
	state.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	result = eglInitialize(state.display, NULL, NULL);
	result = eglChooseConfig(state.display, attribute_list, &config, 1, &num_config);
	assert(EGL_FALSE != result);

	//Init API
	result = eglBindAPI(EGL_OPENGL_ES_API);
	assert(EGL_FALSE != result);

	//Create context
	state.context = eglCreateContext(state.display, config, EGL_NO_CONTEXT, context_attributes);
	assert(state.context != EGL_NO_CONTEXT);

	//Create screen
	state.width = w;
	state.height = h;

	dest_rect.x = 0;
	dest_rect.y = 0;
	dest_rect.width = state.width;
	dest_rect.height = state.height;

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width = state.width;
	src_rect.height = state.height;

	DispmanDisplayH = vc_dispmanx_display_open(0);
	DispmanUpdateH = vc_dispmanx_update_start(0);

	DispmanElementH = 	vc_dispmanx_element_add(DispmanUpdateH, DispmanDisplayH, 0, &dest_rect, 0, &src_rect, DISPMANX_PROTECTION_NONE,  0 , 0, (DISPMANX_TRANSFORM_T) 0);

	state.nativewindow.element = DispmanElementH;
	state.nativewindow.width = state.width;
	state.nativewindow.height = state.height;
	vc_dispmanx_update_submit_sync(DispmanUpdateH);

	state.surface = eglCreateWindowSurface(state.display, config, &(state.nativewindow), NULL);
	assert(state.surface != EGL_NO_SURFACE);

	result = eglMakeCurrent(state.display, state.surface, state.surface, state.context);
	assert(EGL_FALSE != result);

	eglSwapInterval(state.display, 1);
	
	Context::OGLDefaults(w, h);
	
	Context *c = new Context();
	c->t = stptr;
	
	return c;
}

#else
void err(int error, const char *description) {
	printf("GLFW error %i: %s\n", error, description);
}
Context *Context::createContext(u32 wi, u32 he, std::string title) {
	glfwSetErrorCallback(err);
	if (!glfwInit()) {
		printf("Couldn't create window! Error %u, couldn't init glfw!\n", 0);
		return nullptr;
	}
	GLFWmonitor *mon = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(mon);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);

	GLFWwindow *w;

	if (wi != mode->width || he != mode->height || wi == 0 || he == 0) {
		printf("Tried to create screen 1920x1080 but the monitor doesn't support it! Changing to different res!\n");
		w = glfwCreateWindow(wi = mode->width, he = mode->height, title.c_str(), NULL, NULL);
	}
	else {
		w = glfwCreateWindow(wi, he, title.c_str(), NULL, NULL);
	}

	if (w == nullptr) {
		printf("Couldn't create window! Error %u, glfwCreateWindow failed!\n", 1);
		return nullptr;
	}
	glfwSetWindowPos(w, 0, 30);
	glfwMakeContextCurrent(w);

	/*GLFWcursor *c = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
	glfwSetCursor(w, c);*/
	glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	if (glewInit()) {
		printf("Couldn't create window! Error %u, couldn't create glew!\n", 2);
		//glfwDestroyCursor(c);
		return nullptr;
	}

	/*Texture *t = Texture::load(path, true);
	if (t == nullptr) {
		printf("Couldn't create window! Error %i, couldn't find the window icon!\n", 3);
		glfwSetWindowShouldClose(w, true);
		return nullptr;
	}
	GLFWimage img;
	img.width = t->getWidth();
	img.height = t->getHeight();
	img.pixels = t->getData();
	glfwSetWindowIcon(w, 1, &img);

	delete t;*/

	printf("OpenGL version supported %s\n", glGetString(GL_VERSION));
	glfwSwapInterval(1);

	Context::OGLDefaults(wi, he);
	Context *c = new Context();
	c->w = w;
	return c;
}
#endif


void Context::OGLDefaults(u32 width, u32 height) {
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glViewport(0, 0, width, height);
}