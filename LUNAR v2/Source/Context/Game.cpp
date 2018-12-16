#include "../../Headers/Context/Game.h"
#include "../../Headers/Render/ComputeShader.h"
#include "../../Headers/Context/Context.h"
#include "../../Headers/Input/InputHandler.h"
#include "../../Headers/Resource/Resources.h"
#include "../../Headers/Helper/StringUtils.h"
#include "../../Headers/Render/Light.h"
#include "../../Headers/World/Camera.h"
#include "../../Headers/Context/OGLHelper.h"
#include "../../Headers/API/stbi/write.h"

using namespace osomi;
using namespace std;

Game *Game::create(u32 w, u32 h, std::string title, void(*update)(double), void (*render)(), void (*renderForward)(), void (*postRender)(), Camera *(*getCamera)(), std::vector<Light>(*getLights)(), InputHandler **ihp) {
	if (update == nullptr || render == nullptr || postRender == nullptr || renderForward == nullptr || getCamera == nullptr || getLights == nullptr || w == 0 || h == 0 || title == ""){
		printf("Couln't create state; invalid size, title, render, renderForward, postRender, update, getCamera or getLights function!\n");
		return nullptr;
	}
	Context *c = Context::createContext(w, h, title);
	if (c == nullptr){
		printf("Couldn't create window; invalid context!\n");
		return nullptr;
	}
	
	Game *g = new Game();
	g->c = c;
	g->prevW = w;
	g->prevH = h;
	g->update = update;
	g->render = render;
	g->renderForward = renderForward;
	g->postRender = postRender;
	g->cam = getCamera;
	g->light = getLights;
	g->hasFBOS = false;

	glfwSetWindowUserPointer(c->w, g);

	#ifdef __OGL__
	*ihp = InputHandler::init(c->w);
	#else 
	*ihp = InputHandler::init();
	#endif

	g->ih = *ihp;
	
	return g;
}

void Game::recreateFBOS() {
	if (hasFBOS) {
		glDeleteTextures(1, &color);
		glDeleteTextures(1, &normal);
		glDeleteTextures(1, &depth);
		glDeleteTextures(1, &output);
		glDeleteFramebuffers(1, &fbo);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else hasFBOS = true;

	u32 buffers[] = { 0, 1 };

	color = OGLHelper::createTexture(newW, newH, true);
	normal = OGLHelper::createTexture(newW, newH);
	depth = OGLHelper::createDepthTexture(newW, newH);

	fbo = OGLHelper::createFBO();
	OGLHelper::attachment(fbo, DEPTH_TEXTURE, depth);
	OGLHelper::attachment(fbo, 0, color);
	OGLHelper::attachment(fbo, 1, normal);
	OGLHelper::attachments(fbo, buffers);

	if (!OGLHelper::checkFBO(fbo)) {
		printf("Couldn't render FBO!\n");
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &color);
		glDeleteTextures(1, &normal);
		glDeleteTextures(1, &depth);
		hasFBOS = false;
		return;
	}
	output = OGLHelper::createTexture(newW, newH, true);
}

void Game::renderScene(GLuint vbo, GLuint vao) {
	#ifdef __EGL2__
	render();
	return;
	#else

	if (!hasFBOS)return;

	Camera *c = cam();
	vector<Light> larr = light();

	//Cull lights
	Frustum f = c->toFrustum();
	vector<Light> lights;
	for (u32 i = 0; i < larr.size(); i++) {
		Light &l = larr[i];
		float r = l.calculateRadius();
		bool culled = false;
		for (u32 j = 0; j < 6; j++)
			if (f[j].distance(l.getPosition()) < -r) {
				culled = true;
				break;
			}
		if (culled)continue;
		lights.push_back(l);
	}

	//std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();
	
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	render();

	//std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

	//Copy depth bits from fbo to 0 (screen)
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, newW, newH, 0, 0, newW, newH, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Convert 3 buffers into 1 by using tiled-based deferred rendering
	Shader *s = Resources::fetchShader("PostProcessing-compute");
	if (s == nullptr) {
		printf("\"PostProcessing-compute\" compute shader not found!\n");
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &color);
		glDeleteTextures(1, &normal);
		glDeleteTextures(1, &depth);
		return;
	}
	ComputeShader *cs = (ComputeShader*)s;
	cs->use();
	cs->set("albedoSpecular", 0);
	cs->set("normals", 1);
	cs->set("depth", 2);
	cs->set("outCol", 3);
	cs->set("V", c->getView());
	cs->set("P", c->getProjection());
	cs->set("ambient", vec3(.1, .1, .1));
	cs->set("eyeNear", vec4(c->getPosition(), c->getNear()));
	cs->set("dirFar", vec4(c->getDirection(), c->getFar()));

	//Send lights
	/*vec4 *posi = new vec4[lights.size()];
	vec4 *coli = new vec4[lights.size()];*/
	f32 *dat = new f32[lights.size() * 8];
	for (u32 i = 0; i < lights.size(); i++) {
		*(vec4*)&dat[i * 8] = vec4(lights[i].getPosition(), lights[i].calculateRadius());
		*(vec4*)&dat[i * 8 + 4] = vec4(lights[i].getColor(), lights[i].getPower());
	}
	cs->set("actualLights", (i32)lights.size());

	GLuint ssbo;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 8 * sizeof(f32) * lights.size(), dat, GL_STATIC_READ);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);

	delete[] dat;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depth);
	glActiveTexture(GL_TEXTURE0);
	glBindImageTexture(3, output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	//std::chrono::high_resolution_clock::time_point t3 = std::chrono::high_resolution_clock::now();

	cs->execute(math::ceil(newW / 16.f), math::ceil(newH / 16.f), 1);

	//std::chrono::high_resolution_clock::time_point t4 = std::chrono::high_resolution_clock::now();

	//Get rid of SSBO
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glDeleteBuffers(1, &ssbo);

	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	//Draw quad with deferred rendered texture
	s = Resources::fetchShader("PostProcessing");
	if (s == nullptr) {
		printf("Couldn't post process; Shader \"PostProcessing\" is missing!\n");
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &color);
		glDeleteTextures(1, &normal);
		glDeleteTextures(1, &depth);
		glDeleteTextures(1, &output);
		return;
	}
	s->use();
	s->set("t", 0);
	glBindTexture(GL_TEXTURE_2D, output);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//std::chrono::high_resolution_clock::time_point t5 = std::chrono::high_resolution_clock::now();

	//Forward rendering
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	renderForward();

	//std::chrono::high_resolution_clock::time_point t6 = std::chrono::high_resolution_clock::now();

	/*std::chrono::high_resolution_clock::time_point t7 = std::chrono::high_resolution_clock::now();

	double i1 = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0).count();
	double i2 = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count();
	double i3 = std::chrono::duration_cast<std::chrono::duration<double>>(t3 - t2).count();
	double i4 = std::chrono::duration_cast<std::chrono::duration<double>>(t4 - t3).count();
	double i5 = std::chrono::duration_cast<std::chrono::duration<double>>(t5 - t4).count();
	double i6 = std::chrono::duration_cast<std::chrono::duration<double>>(t6 - t5).count();
	double i7 = std::chrono::duration_cast<std::chrono::duration<double>>(t7 - t6).count();
	double it = std::chrono::duration_cast<std::chrono::duration<double>>(t7 - t0).count();*/


	//printf("Setting up FBO %fs, render calls (deferred) %fs, setting up deferred shader %fs, executing deferred shader %fs, rendering deferred texture %fs, forward rendering %fs, deletion %fs. Total %fs.\n", i1, i2, i3, i4, i5, i6, i7, it);

	#endif
}

GLuint initSquad(GLuint &vao) {
	GLfloat arr[] = {
		0, 0,
		1, 0,
		1, 1,

		1, 1,
		0, 1,
		0, 0
	};
	u32 arrSize = 2 * 3 * 2;

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, arrSize * sizeof(GLfloat), arr, GL_STATIC_DRAW);

	#ifndef __EGL2__
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (char*)0);

	glBindVertexArray(0);
	#endif
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return vbo;
}

void Game::start(){
	GLuint vao;
	GLuint vbo = initSquad(vao);
	printf("Started game\n");
	t0 = chrono::high_resolution_clock::now();
	t = t0;
	float dt = 0;
	bool init = false;
	while (true){
		#ifdef __OGL__
		glfwGetWindowSize(c->w, &newW, &newH);
		#endif
		if ((newW != 0 && newH != 0 && newH != prevH && newW != prevW) || (!hasFBOS && newW != 0 && newH != 0)){
			glViewport(0, 0, newW, newH);
			recreateFBOS();
			prevW = newW;
			prevH = newH;
		}
		
		chrono::high_resolution_clock::time_point now = chrono::high_resolution_clock::now();
		dt = chrono::duration_cast<chrono::duration<double>>(now - t).count();
		
		if (!init) {
			init = true;
			continue;
		}
		
		update(dt);
		
		t = chrono::high_resolution_clock::now();
		
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderScene(vbo, vao);
		
		#ifdef __EGL__
		eglSwapBuffers(c->t->display, c->t->surface);
		#else
		glfwSwapBuffers(c->w);
		#endif
		
		postRender();
		#ifdef __OGL__
		ih->update();
		glfwPollEvents();
		#endif
		
		if(ih->isKeyDown(CODE_F10))printf("%f\n", 1 / dt);

		frames++;
		time += dt;

		#ifdef __OGL__
		if (glfwWindowShouldClose(c->w)) 
			break;
		#endif
	}
	glDeleteBuffers(1, &vbo);
	#ifndef __EGL2__
	glDeleteVertexArrays(1, &vao);
	#endif
}