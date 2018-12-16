#include "../../../Headers/MarchingCubes/Render/ProceduralMeshPart.h"
#include "../../../Headers/Render/Shader.h"
#include "../../../Headers/Helper/StringUtils.h"
#include "../../../Headers/Render/Light.h"

using namespace osomi;

ProceduralMeshPart::ProceduralMeshPart(std::string _name, GLuint _vbo, GLuint _ibo, unsigned int _M, unsigned int _N, osomi::vec3 _translate, osomi::vec3 _scale, ProceduralMeshFragment **cf, u32 cfcount, osomi::mat4 _model): name(_name), aabb(osomi::AABB(_translate - _scale/2, _translate + _scale/2) * _model), vbo(_vbo), indices(_ibo), M(_M), N(_N), translate(_translate), scale(_scale), cavefrags(cf), frags(cfcount), model(_model) {
	#ifndef __EGL2__
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	GLsizei posl = sizeof(GLfloat) * 3;
	GLsizei normall = sizeof(GLfloat) * 3;
	GLsizei stride = posl + normall;

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (char*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (char*)posl);
	
	glBindVertexArray(0);
	#endif
}

void ProceduralMeshPart::append(std::vector<ProceduralMeshFragment*> &frag, osomi::AABB box) {
	for (u32 i = 0; i < frags; i++)
		if ((cavefrags[i]->getAABB()).intersect(box))
			frag.push_back(cavefrags[i]);
}
ProceduralMeshPart::~ProceduralMeshPart() {
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &indices);
	for (u32 i = 0; i < frags; i++)delete cavefrags[i];
	delete[] cavefrags;
}
void ProceduralMeshPart::render(Shader *s, GLuint s0, GLuint s1) {
	mat4 m = getModel();

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
	
	#ifdef __EGL2__
	GLsizei posl = sizeof(GLfloat) * 3;
	GLsizei normall = sizeof(GLfloat) * 3;
	GLsizei stride = posl + normall;

	glEnableVertexAttribArray(s0);
	glVertexAttribPointer(s0, 3, GL_FLOAT, GL_FALSE, stride, (char*)0);
	glEnableVertexAttribArray(s1);
	glVertexAttribPointer(s1, 3, GL_FLOAT, GL_FALSE, stride, (char*)posl);
	#else
	glBindVertexArray(vao);
	#endif

	s->set("M", m);
	
	glDrawElements(GL_TRIANGLES, N, GL_UNSIGNED_SHORT, 0);
}