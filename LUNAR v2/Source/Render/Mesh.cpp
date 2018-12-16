#include "../../Headers/Render/Mesh.h"
#include "../../Headers/Render/Shader.h"
#include "../../Headers/Helper/StringUtils.h"
#include "../../Headers/Render/Light.h"

using namespace osomi;
using namespace std;

Mesh *Mesh::create(string name, float *vert, u32 vertBytes, u8 *ind, u32 bytePerIndex, u32 indBytes) {
	if (vert == nullptr || ind == nullptr || vertBytes == 0 || bytePerIndex == 0 || indBytes == 0)return nullptr;
	
	vec3 min = vec3::max(), max = vec3::min();
	for (u32 i = 0; i < vertBytes / 8 / sizeof(GLfloat); i++){
		vec3 pos = *(vec3*)&vert[i * 8];
		if (pos.x < min.x)min.x = pos.x;
		if (pos.y < min.y)min.y = pos.y;
		if (pos.z < min.z)min.z = pos.z;
		if (pos.x > max.x)max.x = pos.x;
		if (pos.y > max.y)max.y = pos.y;
		if (pos.z > max.z)max.z = pos.z;
	}
	if (min == vec3::max())min = vec3();
	if (max == vec3::min())max = vec3();
	
	GLuint vbo, indices;
	
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertBytes, vert, GL_STATIC_DRAW);
		
	glGenBuffers(1, &indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indBytes, ind, GL_STATIC_DRAW);
	
	Mesh *m = new Mesh(name, vbo, indices, vertBytes, bytePerIndex, indBytes / bytePerIndex, AABB(min, max));
	m->vertexBuffer = vert;
	m->indexBuffer = ind;
	
	#ifndef __EGL2__
	
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLsizei posl = sizeof(GLfloat) * 3;
	GLsizei coll = sizeof(GLfloat) * 2;
	GLsizei normall = sizeof(GLfloat) * 3;
	GLsizei stride = posl + coll + normall;

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (char*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (char*)posl);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (char*)(posl + coll));
	
	m->vao = vao;
	glBindVertexArray(0);
	
	#endif
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	return m;
}
void Mesh::nobindRender(Shader *s, mat4 m, mat4 v, mat4 p, vec3 eye, vector<Light> lights, Frustum f){
	s->set("MVP", p * v * m);
	s->set("M", m);
	
	if (indexPtrLength == 1)glDrawElements(GL_TRIANGLES, vertices, GL_UNSIGNED_BYTE, 0);
	else if (indexPtrLength == 2)glDrawElements(GL_TRIANGLES, vertices, GL_UNSIGNED_SHORT, 0);
	else if (indexPtrLength == 4)glDrawElements(GL_TRIANGLES, vertices, GL_UNSIGNED_INT, 0);
}
void Mesh::unbind(Shader *s){
	#ifdef __EGL2__
	GLuint s0 = s->getLocation("position");
	GLuint s1 = s->getLocation("texCoord");
	GLuint s2 = s->getLocation("normal");
	glDisableVertexAttribArray(s0);
	glDisableVertexAttribArray(s1);
	glDisableVertexAttribArray(s2);
	#else
	glBindVertexArray(0);
	#endif
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void Mesh::bind(Shader *s){
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	#ifndef __EGL2__
	glBindVertexArray(vao);
	#endif
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	
	#ifdef __EGL2__
	GLsizei posl = sizeof(GLfloat) * 3;
	GLsizei coll = sizeof(GLfloat) * 2;
	GLsizei normall = sizeof(GLfloat) * 3;
	GLsizei stride = posl + coll + normall;
	
	GLuint s0 = s->getLocation("position");
	GLuint s1 = s->getLocation("texCoord");
	GLuint s2 = s->getLocation("normal");

	glEnableVertexAttribArray(s0);
	glVertexAttribPointer(s0, 3, GL_FLOAT, GL_FALSE, stride, (char*)0);
	glEnableVertexAttribArray(s1);
	glVertexAttribPointer(s1, 2, GL_FLOAT, GL_FALSE, stride, (char*)posl);
	glEnableVertexAttribArray(s2);
	glVertexAttribPointer(s2, 3, GL_FLOAT, GL_FALSE, stride, (char*)(posl + coll));
	#endif
}
void Mesh::render(Shader *s, mat4 m, mat4 v, mat4 p, vec3 eye, vector<Light> lights, Frustum f){
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
Mesh::~Mesh(){
	if(chunks != nullptr)delete chunks;
	if(vertexBuffer != nullptr) delete[] vertexBuffer;
	if(vertexBuffer != nullptr) delete[] indexBuffer;
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	#ifndef __EGL2__
	glDeleteVertexArrays(1, &vao);
	#endif
}