#pragma once
#include "../Render/RenderObject.h"
#include "Render/ProceduralMeshPart.h"
#include "../World/Grid3D.h"
namespace osomi {
	class Texture;
	class ProceduralMesh : public RenderObject {
	private:
		Texture *t;
		std::vector<ProceduralMeshPart*> p;

		void push(ProceduralMeshPart *cp) { if (cp != nullptr) p.push_back(cp); }

		ProceduralMesh(Texture *t, std::string name, AABB aabb);
		
	protected:
		void nobindRender(Shader *s, mat4 m, mat4 v, mat4 p, vec3 eye, std::vector<Light> lights, Frustum f) override;
		void unbind(Shader *s) override;
		void bind(Shader *s) override;
	public:
		~ProceduralMesh();

		void render(Shader *s, mat4 m, mat4 v, mat4 p, vec3 eye, std::vector<Light> lights, Frustum f) override;
	
		std::vector<ProceduralMeshFragment*> getParts(AABB e);
	
		static ProceduralMesh *create(std::string name, Texture *t, Grid3Df *g, float size, int chunkX, int chunkY, int chunkZ);
		
		static ProceduralMesh *create3D(std::string name, Texture *t, u8 chunks, float size = 2, vec3 noiseScale = vec3(.15, .15, .15), u8 chunkSize = 8);
	};
}