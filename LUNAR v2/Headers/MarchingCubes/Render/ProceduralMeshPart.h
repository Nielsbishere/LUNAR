#pragma once

#include "../../Math/MathDefines.h"
#include "../../Render/RenderObject.h"
#include <vector>

#include "../ProceduralMeshFragment.h"

namespace osomi {
	class Shader;
	class ProceduralMeshPart {
		friend class MarchingCubes;
		friend class ProceduralMesh;
	private:
		GLuint vbo, indices;
		#ifndef __EGL2__
		GLuint vao;
		#endif
		unsigned int M, N;
		vec3 translate, scale;
		ProceduralMeshFragment **cavefrags;
		u32 frags;
		mat4 model;
		
		std::string name;
		AABB aabb;
	protected:
		ProceduralMeshPart(std::string name, GLuint _vbo, GLuint _ibo, unsigned int _M, unsigned int _N, vec3 _translate, vec3 _scale, ProceduralMeshFragment **cf, u32 cfcount, mat4 _model);
		void render(Shader *s, GLuint s0, GLuint s1);
	public:
		~ProceduralMeshPart();
		void setTranslate(vec3 tr) { translate = tr; }
		void setScale(vec3 sc) { scale = sc; }
		vec3 getTranslate() { return translate; }
		vec3 getScale() { return scale; }
		
		mat4 getModel(){ return mat4::getModel(translate, vec3(), scale); }
		
		AABB getAABB(){ return aabb; }
		std::string getName(){ return name; }
	
		void append(std::vector<ProceduralMeshFragment*> &frag, AABB box);
	};	
}