#include "../../Headers/UI/Billboard.h"
#include "../../Headers/Render/Shader.h"
#include "../../Headers/World/Camera.h"
#include "../../Headers/Resource/Resources.h"
#include "../../Headers/World/GameObject.h"

using namespace osomi;

void Billboard::render(Shader *s, mat4 v, mat4 p, Frustum fr) {
	t->bind(GL_TEXTURE_2D);
	
	vec3 posi = go != nullptr ? position + go->getAABB().getCenter() + vec3(0, go->getAABB().getSize().y, 0)/2 : position;
	
	mat4 m = mat4();
	mat4 acm = mat4::getModelDeg(posi, rotation, vec3(1,1,1));
	
	m.setOrientation(v.getOrientation().transpose());
	m = (acm * m) * mat4::scale(scale);
	
	if (inWorldSpace) {
		s->set("MVP", p * v * m);
	} else{
		p = mat4();
		p.setScale(vec3(fr.getAspect(), 1, 1));
		s->set("MVP", p * getMatrix());
	}
	s->set("ambient", colorizer);
	s->set("uvBounds", uv);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

BillboardHelper::~BillboardHelper() {
	for (u32 i = 0; i < bills.size(); i++)
		delete bills[i];
	bills.clear();
	
	glDeleteBuffers(1, &vbo);
}
BillboardHelper::BillboardHelper(){
	GLuint vboSize = 6 * 2 * sizeof(GLfloat);
	
	GLfloat vert[] = { -1,-1,  1,-1,  1,1,  1,1,  -1,1,  -1,-1};
	
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vboSize, vert, GL_STATIC_DRAW);

	#ifndef __EGL2__
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (char*)0);

	glBindVertexArray(0);
	#endif
}
void BillboardHelper::render(Shader *s, Camera *c){
	if (bills.size() == 0)return;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	glEnable(GL_BLEND);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	#ifdef __EGL2__
	GLuint s0 = s->getLocation("position");
	glEnableVertexAttribArray(s0);
	glVertexAttribPointer(s0, 2, GL_FLOAT, GL_FALSE, NULL, (char*)0);
	#else
	glBindVertexArray(vao);
	#endif
	
	std::vector<Billboard*> bill2d, bill3d;
	
	for (u32 i = 0; i < bills.size(); i++){
		if (bills[i]->isInWorldSpace())bill3d.push_back(bills[i]);
		else bill2d.push_back(bills[i]);
	}
	
	if (bill2d.size() != 0) {
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
		for (u32 i = 0; i < bill2d.size(); i++)
			bill2d[i]->render(s, c->getView(), c->getProjection(), c->toFrustum());
	}
	
	if (bill2d.size() != 0) {
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}
	for (u32 i = 0; i < bill3d.size(); i++)
		bill3d[i]->render(s, c->getView(), c->getProjection(), c->toFrustum());
	
	#ifdef __EGL2__
	glDisableVertexAttribArray(s0);
	#else
	glBindVertexArray(0);
	#endif
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glDisable(GL_BLEND);
}


Billboard *BillboardHelper::add(std::string name, Texture *t, vec2 _position, vec3 _scale, vec3 _rotation, vec4 uv, vec3 _colorizer) {
	for (u32 i = 0; i < bills.size(); i++)
		if (bills[i]->getName() == name)
			return bills[i];
	Billboard *bb;
	bills.push_back(bb = new Billboard(name, t, _position, _scale, _rotation, uv, _colorizer));
	return bb;
}
Billboard *BillboardHelper::add(std::string name, Texture *t, vec3 _position, vec3 _scale, vec3 _rotation, vec4 uv, vec3 _colorizer) {
	for (u32 i = 0; i < bills.size(); i++)
		if (bills[i]->getName() == name)
			return bills[i];
	Billboard *bb;
	bills.push_back(bb = new Billboard(name, t, _position, _scale, _rotation, uv, _colorizer));
	return bb;
}
void BillboardHelper::remove(std::string name){
	for (u32 i = 0; i < bills.size(); i++)
		if (bills[i]->getName() == name){
			delete bills[i];
			bills.erase(bills.begin() + i);
			return;
		}
}