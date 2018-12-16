#pragma once
#include "../Math/MathDefines.h"
#include <string>
#include <vector>
namespace osomi{
	class GameObject;
	class Texture;
	class Camera;
	class Light;
	class Shader;
	class InputHandler;
	struct LinkedObject{
		friend class World;
	protected:
		static Camera *c;
		
		Shader *s;
		GameObject *go;
		LinkedObject(GameObject *_go, Shader *_s): s(_s), go(_go){}

		float cdist(const LinkedObject *lo);
	public:
		LinkedObject() : s(nullptr), go(nullptr) {}
		bool operator<(const LinkedObject &lo);
	};
	class World{
	private:
		std::string name;
		std::vector<LinkedObject> gos;
	public:
		World(std::string n): name(n){}
		std::string getName(){ return name; }
		u32 getObjects(){ return gos.size(); }
		GameObject *at(u32 i){
			if (i >= getObjects())return nullptr;
			return gos[i].go;
		}
		GameObject *get(std::string name);
		
		~World();
		GameObject *add(std::string name, Shader *s, RenderObject *render, Texture *texture, vec3 position, vec3 rotation = vec3(), vec3 scale = vec3(1, 1, 1));
		bool remove(std::string objectName);
		
		void update(double delta, InputHandler *ih);
		void render(Camera *c, std::vector<Light> lights);
	};
}