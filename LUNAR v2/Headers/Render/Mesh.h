#pragma once
#include "../Math/Types.h"
#include "../Math/MathDefines.h"
#include "../World/C-word/CChunk.h"

#include "RenderObject.h"
namespace osomi{
	class Mesh : public RenderObject{
		friend class Resources;
		friend class MarchingSquares;
	private:
		u8 indexPtrLength;
		u32 vertices, vertLength;
		
		f32 *vertexBuffer;
		u8 *indexBuffer;
		
		CChunk *chunks;
		
		GLuint vbo, ibo;
		#ifndef __EGL2__
		GLuint vao;
		#endif
	protected:
		Mesh(std::string name, GLuint _vbo, GLuint _ibo, u32 _vertLength, u8 _indexPtrLength, u32 _vertices, AABB aabb): RenderObject(name, aabb), vbo(_vbo), ibo(_ibo), indexPtrLength(_indexPtrLength), vertices(_vertices), vertLength(_vertLength), chunks(nullptr), vertexBuffer(nullptr), indexBuffer(nullptr){}
		
		static Mesh *create(std::string name, float *vert, u32 vertBytes, u8 *dat, u32 bytePerIndex, u32 indBytes);
		~Mesh();
		
		void setChunk(CChunk *c){
			if (chunks != nullptr)return;
			chunks = c;
		}
		void unbind(Shader *s) override;
		void bind(Shader *s) override;
		void nobindRender(osomi::Shader *s, osomi::mat4 m, osomi::mat4 v, osomi::mat4 p, osomi::vec3 eye, std::vector<osomi::Light> light, Frustum f) override;
	public:
		void render(osomi::Shader *s, osomi::mat4 m, osomi::mat4 v, osomi::mat4 p, osomi::vec3 eye, std::vector<osomi::Light> light, Frustum f) override;
		
		CChunk *getChunk(){ return chunks; }
	};
}