#include "../../Headers/Render/MD2Model.h"
#include "../../Headers/Render/Shader.h"
#include "../../Headers/Helper/StringUtils.h"
#include "../../Headers/Render/Light.h"

using namespace osomi;
using namespace std;

MD2Model *MD2Model::create(string name, float *buf, u32 verts, u32 frames, std::vector<FrameState> states, AABB aabb) {
	if (buf == nullptr || verts == 0 || frames == 0 || states.size() == 0)return nullptr;
	
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verts * frames * 8 * sizeof(GLfloat), buf, GL_STATIC_DRAW);
	
	MD2Model *m = new MD2Model(name, vbo, verts, frames, buf, states, aabb);
	
#if !(defined(__EGL2__) || defined(__EGL1__))
	
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	GLsizei posl = sizeof(GLfloat) * 3;
	GLsizei coll = sizeof(GLfloat) * 2;
	GLsizei normall = sizeof(GLfloat) * 3;
	GLsizei stride = posl + coll + normall;
	GLsizei framestride = sizeof(GLfloat) * 8 * verts;

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (char*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (char*)posl);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (char*)(posl + coll));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (char*)framestride);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (char*)(posl + coll + framestride));
	
	m->vao = vao;
	glBindVertexArray(0);
	
#endif
	
	return m;
}
void MD2Model::bind(Shader *s){
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	#ifdef __EGL2__
	GLsizei posl = sizeof(GLfloat) * 3;
	GLsizei coll = sizeof(GLfloat) * 2;
	GLsizei normall = sizeof(GLfloat) * 3;
	GLsizei stride = posl + coll + normall;
	GLsizei framestride = sizeof(GLfloat) * 8 * vertices;
	
	GLuint s0 = s->getLocation("position");
	GLuint s1 = s->getLocation("texCoord");
	GLuint s2 = s->getLocation("normal");
	GLuint s3 = s->getLocation("nposition");
	GLuint s4 = s->getLocation("nnormal");

	glEnableVertexAttribArray(s0);
	glVertexAttribPointer(s0, 3, GL_FLOAT, GL_FALSE, stride, (char*)0);
	glEnableVertexAttribArray(s1);
	glVertexAttribPointer(s1, 2, GL_FLOAT, GL_FALSE, stride, (char*)posl);
	glEnableVertexAttribArray(s2);
	glVertexAttribPointer(s2, 3, GL_FLOAT, GL_FALSE, stride, (char*)(posl + coll));
	glEnableVertexAttribArray(s3);
	glVertexAttribPointer(s3, 3, GL_FLOAT, GL_FALSE, stride, (char*)framestride);
	glEnableVertexAttribArray(s4);
	glVertexAttribPointer(s4, 3, GL_FLOAT, GL_FALSE, stride, (char*)(posl + coll + framestride));
	#else
	glBindVertexArray(vao);
	#endif
}
void MD2Model::unbind(Shader *s){
	#ifdef __EGL2__
	GLuint s0 = s->getLocation("position");
	GLuint s1 = s->getLocation("texCoord");
	GLuint s2 = s->getLocation("normal");
	GLuint s3 = s->getLocation("nposition");
	GLuint s4 = s->getLocation("nnormal");
	glDisableVertexAttribArray(s0);
	glDisableVertexAttribArray(s1);
	glDisableVertexAttribArray(s2);
	glDisableVertexAttribArray(s3);
	glDisableVertexAttribArray(s4);
	#else
	glBindVertexArray(0);
	#endif
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	tcurrent = nullptr;
	tanimationTime = 0;
}
void MD2Model::nobindRender(Shader *s, mat4 m, mat4 v, mat4 p, vec3 eye, vector<Light> lights, Frustum f) {
	s->set("MVP", p * v * m);
	s->set("M", m);
	
	f32 perc = math::fraction(tanimationTime / tcurrent->getDuration()) * (f32)(tcurrent->getFrames() - 1);
	
	u32 offset = floor(perc) + tcurrent->getOffset();
	s->set("factor", math::fraction(perc));
	
	glDrawArrays(GL_TRIANGLES, offset * vertices, vertices);
}
void MD2Model::render(Shader *s, mat4 m, mat4 v, mat4 p, vec3 eye, vector<Light> lights, Frustum f) {
	if (tcurrent == nullptr) {
		printf("Warning! Object tried to render MD2Model without binding framestate!\n");
		return;
	}
	bind(s);
	
	s->set("V", v);
	s->set("P", p);
	s->set("eye", eye);
	
	s->set("actualLights", (i32)lights.size());
	for (u32 i = 0; i < lights.size(); i++) {
		s->set(StringUtils::numstring(i, "lights[", "].position"), lights[i].getPosition());
		s->set(StringUtils::numstring(i, "lights[", "].color"), lights[i].getColor());
		s->set(StringUtils::numstring(i, "lights[", "].power"), lights[i].getPower());
	}
	
	nobindRender(s, m, v, p, eye, lights, f);
	
	unbind(s);
}
MD2Model::~MD2Model() {
	delete[] buffer;
	glDeleteBuffers(1, &vbo);
	#ifndef __EGL2__
	glDeleteVertexArrays(1, &vao);
	#endif
}
FrameState *MD2Model::getAnimationState(std::string name){
	for (u32 i = 0; i < states.size(); i++)
		if (StringUtils::equalsIgnoreCase(states[i].getName(), name))
			return &states[i];
	return nullptr;
}
void MD2Model::setTemporary(FrameState *current, float animationTime){
	tcurrent = current;
	tanimationTime = animationTime;
}