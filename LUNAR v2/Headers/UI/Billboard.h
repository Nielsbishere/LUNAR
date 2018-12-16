#pragma once

#include "../Math/MathDefines.h"
#include "../Shapes/Frustum.h"
#include <string>
#include <vector>

namespace osomi{
	class Shader;
	class Camera;
	class Texture;
	class GameObject;
	class Billboard{
		friend class BillboardHelper;
		friend class Behavior;
	private:
		vec3 position;
		vec3 scale;
		vec3 rotation;
		
		vec3 colorizer;
		
		bool inWorldSpace;
		
		std::string n;
		vec4 uv;
		
		Texture *t;
		
		GameObject *go;
	protected:
		void render(Shader *s, mat4 v, mat4 p, Frustum fr);
		
		Billboard(std::string name, Texture *_t, vec2 _position, vec3 _scale, vec3 _rotation, vec4 _uv=vec4(0,0,1,1), vec3 _colorizer = vec3(1, 1, 1)): n(name), position(vec3(_position, 0)), scale(_scale), rotation(_rotation), colorizer(_colorizer), inWorldSpace(false), uv(_uv), t(_t), go(nullptr){}
		Billboard(std::string name, Texture *_t, vec3 _position, vec3 _scale, vec3 _rotation, vec4 _uv=vec4(0,0,1,1), vec3 _colorizer = vec3(1,1,1)): n(name),  position(_position), scale(_scale), rotation(_rotation), colorizer(_colorizer), inWorldSpace(true), uv(_uv), t(_t), go(nullptr){}
		
		void link(GameObject *_go){ if (go == nullptr) go = _go; }
	public:
		
		vec3 getPosition(){ return position; }
		vec3 getScale(){ return scale; }
		vec3 getRotation(){ return rotation; }
		vec3 getColorizer(){ return colorizer; }
		
		GameObject *getLinked(){ return go; }
		
		bool isInWorldSpace(){ return inWorldSpace; }
		
		mat4 getMatrix(){ return mat4::getModelDeg(position, rotation, scale); }
		
		std::string getName(){ return n; }
	};
	class BillboardHelper{
	private:
		std::vector<Billboard*> bills;
		GLuint vbo;
		#ifndef __EGL2__
		GLuint vao;
		#endif
	public:
		BillboardHelper();
		~BillboardHelper();
		
		Billboard *add(std::string name, Texture *t, vec2 _position, vec3 _scale, vec3 _rotation, vec4 _uv = vec4(0, 0, 1, 1), vec3 _colorizer = vec3(1, 1, 1));
		Billboard *add(std::string name, Texture *t, vec3 _position, vec3 _scale, vec3 _rotation, vec4 _uv = vec4(0, 0, 1, 1), vec3 _colorizer = vec3(1, 1, 1));
		
		void remove(std::string name);
		
		void render(Shader *s, Camera *c);
	};
}