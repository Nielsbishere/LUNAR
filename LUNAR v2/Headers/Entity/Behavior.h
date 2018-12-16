#pragma once

#include "../Math/MathDefines.h"
#include "../UI/Billboard.h"
#include <vector>
#include <string>

namespace osomi{
	class GameObject;
	class InputHandler;
	class Billboard;
	class BillboardHelper;
	class Behavior{
		friend class GameObject;
	private:
		std::string name;
		BillboardHelper *helper;
		std::vector<Billboard*> bill;
	protected:
		Behavior(BillboardHelper *bh, std::string n): helper(bh), name(n){}
		virtual ~Behavior();
	public:
		std::string getName(){ return name; }
		Billboard *addBillboard(GameObject *go, std::string texName, vec3 position, vec3 scale, vec4 uv, vec3 colorizer);
		
		virtual void onUpdate(GameObject *go, double delta) = 0;
		virtual void onUpdate(GameObject *go, double delta, InputHandler *ih){}
		virtual void init(GameObject *go){}
	};
}