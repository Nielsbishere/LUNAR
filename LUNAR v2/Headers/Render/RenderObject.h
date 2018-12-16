#pragma once
#include <vector>
#include <string>
#include "../Shapes/AABB.h"
#include "../Shapes/Frustum.h"

namespace osomi {
	class mat4;
	class vec3;
	class Shader;
	class Light;
	class RenderObject {
		friend class Shader;
		friend class Resources;
		friend class World;
	private:
		std::string name;
		AABB aabb;
	protected:
		RenderObject(std::string _name, AABB _aabb): name(_name), aabb(_aabb){}
		virtual ~RenderObject(){}
		
		virtual void bind(Shader *s) = 0;
		virtual void unbind(Shader *s) = 0;
		virtual void nobindRender(Shader *s, mat4 m, mat4 v, mat4 p, vec3 eye, std::vector<Light> lights, Frustum fr) = 0;
	public:
		virtual void render(Shader *s, mat4 m, mat4 v, mat4 p, vec3 eye, std::vector<Light> lights, Frustum fr) = 0;
		std::string getName() { return name; }
		AABB getAABB(){ return aabb; }
	};
}