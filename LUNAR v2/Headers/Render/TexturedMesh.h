#pragma once
#include "../Math/Types.h"
#include "../Math/MathDefines.h"
#include "../Resource/Texture.h"
#include "../API/tinyobjloader/tiny_obj_loader.h"

#include "RenderObject.h"
namespace osomi{
	//TODO: Resource leak; textures are loaded but never unloaded.
	//How to fix?
	class TexturedMesh : public RenderObject{
		friend class Resources;
	private:
		TexturedMesh(std::string name, GLuint _vbo, GLfloat *dat, u32 _vertices, std::vector<Texture*> texs, std::vector<u32> offs, AABB aabb): RenderObject(name, aabb), vbo(_vbo), vertexBuffer(dat), vertices(_vertices), textures(texs), offsets(offs){}
		
		u32 vertices;
		
		f32 *vertexBuffer;
		std::vector<u32> offsets;
		std::vector<Texture*> textures;
		
		GLuint vbo;
		#ifndef __EGL2__
		GLuint vao;
		#endif
	protected:
		static TexturedMesh *create(std::string name, GLfloat *vert, u32 vertices, std::vector<tinyobj::material_t> materials, std::vector<u32> offsets, AABB aabb);
		~TexturedMesh();
		u32 getTextures(){ return textures.size(); }
		u32 getVertices(){ return vertices; }
		f32 *getVertex(){ return vertexBuffer; }
		
		void nobindRender(osomi::Shader *s, osomi::mat4 m, osomi::mat4 v, osomi::mat4 p, osomi::vec3 eye, std::vector<osomi::Light> light, Frustum f) override;
		void bind(osomi::Shader *s) override;
		void unbind(osomi::Shader *s) override;
	public:
		void render(osomi::Shader *s, osomi::mat4 m, osomi::mat4 v, osomi::mat4 p, osomi::vec3 eye, std::vector<osomi::Light> light, Frustum f) override;
	};
}