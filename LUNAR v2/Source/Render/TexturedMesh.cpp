#include "../../Headers/Render/TexturedMesh.h"
#include "../../Headers/Render/Shader.h"
#include "../../Headers/Helper/StringUtils.h"
#include "../../Headers/Render/Light.h"

#include "../../Headers/Defines.h"

#include "../../Headers/Resource/Resources.h"

using namespace osomi;
using namespace std;

TexturedMesh *TexturedMesh::create(string name, GLfloat *vert, u32 vertices, vector<tinyobj::material_t> materials, vector<u32> offsets, AABB aabb) {
	if (vert == nullptr || materials.size() == 0 || vertices == 0 || offsets.size() == 0)return nullptr;
	
	GLuint vbo, indices;
	
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices * 8 * sizeof(GLfloat), vert, GL_STATIC_DRAW);
	
	vector<Texture*> texs(materials.size());
	for (u32 i = 0; i < materials.size(); i++)
		texs[i] = Resources::loadTexture("../Resources/Textures/" + materials[i].diffuse_texname, StringUtils::grabFileNoExt(materials[i].diffuse_texname));
	
	TexturedMesh *m = new TexturedMesh(name, vbo, vert, vertices, texs, offsets, aabb);
	
	#ifndef __EGL2__
	
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

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
	
	return m;
}
void TexturedMesh::nobindRender(Shader *s, mat4 m, mat4 v, mat4 p, vec3 eye, vector<Light> lights, Frustum f){
	s->set("MVP", p * v * m);
	s->set("M", m);
	for (u32 i = 0; i < textures.size(); i++) {
		textures[i]->bind(GL_TEXTURE_2D);
		glDrawArrays(GL_TRIANGLES, offsets[i] * 3, (offsets[i + 1] - offsets[i]) * 3);
	}
}
void TexturedMesh::bind(Shader *s){
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
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
	#else
	glBindVertexArray(vao);
	#endif
}
void TexturedMesh::unbind(Shader *s){
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
void TexturedMesh::render(Shader *s, mat4 m, mat4 v, mat4 p, vec3 eye, vector<Light> lights, Frustum f) {
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
TexturedMesh::~TexturedMesh() {
	delete[] vertexBuffer;
	glDeleteBuffers(1, &vbo);
	#ifndef __EGL2__
	glDeleteVertexArrays(1, &vao);
	#endif
}