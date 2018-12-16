#include "../../Headers/World/World.h"
#include "../../Headers/World/GameObject.h"
#include "../../Headers/Resource/Resources.h"
#include <algorithm>
#include <chrono>
#include "../../Headers/Helper/StringUtils.h"
#include "../../Headers/World/Camera.h"

using namespace osomi;

Camera *LinkedObject::c = nullptr;

GameObject *World::add(std::string name, Shader *s, RenderObject *render, Texture *texture, vec3 position, vec3 rotation, vec3 scale){
	if (scale == vec3() || render == nullptr)return nullptr;
	
	for (u32 i = 0; i < gos.size(); i++)
		if (gos[i].go->getName() == name){
			printf("GameObjects need unique names!\n");
			return nullptr;
		}
	
	GameObject *go = new GameObject(this, name, render, texture, position, rotation, scale);
	gos.push_back(LinkedObject(go, s));
	return go;
}
bool World::remove(std::string name){
	for (u32 i = 0; i < gos.size(); i++)
		if (gos[i].go->getName() == name) {
			delete gos[i].go;
			gos.erase(gos.begin() + i);
			return true;
		}
	return false;
}
GameObject *World::get(std::string name){
	for (u32 i = 0; i < gos.size(); i++)
		if (gos[i].go->getName() == name) 
			return gos[i].go;
	return nullptr;
}
World::~World(){
	for (u32 i = 0; i < gos.size(); i++)
		delete gos[i].go;
	gos.clear();
}

void World::update(double delta, InputHandler *ih){
	for (u32 i = gos.size() - 1; i != 0xFFFFFFFF; i--) {
		if (gos[i].go->isDead()) {
			delete gos[i].go;
			gos.erase(gos.begin() + i);
			continue;
		}
		gos[i].go->update(delta, ih);
	}
}
void World::render(Camera *c, std::vector<Light> lights){
	Frustum f = c->toFrustum();
	AABB enc = f.toAABB();

	LinkedObject::c = c;

	std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();

	std::vector<LinkedObject> temp;

	for (u32 i = 0; i < gos.size(); i++)
		if (!gos[i].go->isHidden() && f.intersectionSphere(gos[i].go->getAABB(), 5))
			temp.push_back(gos[i]);

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	std::sort(temp.begin(), temp.end());
	//printf("%fs for sorting! (%fs in pre-sort)\n", std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - t0).count(), std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0).count());

	Shader *prev = nullptr;
	RenderObject *pro = nullptr;
	Texture *t = nullptr, *tspec = nullptr;
	u32 rendered = 0, binds = 0, unbinds = 0, textureBinds = 0, shaderSwaps = 0;
	for (u32 i = 0; i < temp.size(); i++) {
		if (!f.intersection(temp[i].go->getAABB(), 15) || temp[i].go->isHidden())continue;
		if (prev != temp[i].s) {
			prev = temp[i].s;
			if(prev != nullptr){ 
				prev->use();
				prev->set("t", 0);			//Diffuse texture
				prev->set("spec", 1);		//Specular map
				prev->set("norm", 2);		//Normal map
				prev->set("V", c->getView());
				prev->set("P", c->getProjection());
				prev->set("eye", c->getPosition());
				prev->set("actualLights", (i32)lights.size());
				for (u32 i = 0; i < lights.size(); i++) {
					prev->set(StringUtils::numstring(i, "lights[", "].position"), lights[i].getPosition());
					prev->set(StringUtils::numstring(i, "lights[", "].color"), lights[i].getColor());
					prev->set(StringUtils::numstring(i, "lights[", "].power"), lights[i].getPower());
				}
				shaderSwaps++;
			}
		}
		if (pro != temp[i].go->ro){
			if (pro != nullptr) {
				pro->unbind(prev);
				unbinds++;
			}
			pro = temp[i].go->ro;
			if (pro != nullptr) {
				pro->bind(prev);
				binds++;
			}
		}
		if (t != temp[i].go->getT()) {
			t = temp[i].go->getT();
			glActiveTexture(GL_TEXTURE0);
			if (t != nullptr)t->bind(GL_TEXTURE_2D);
			else glBindTexture(GL_TEXTURE_2D, 0);
			textureBinds++;
		}
		if (tspec != temp[i].go->getSpecular()) {
			tspec = temp[i].go->getSpecular();
			glActiveTexture(GL_TEXTURE1);
			if (tspec != nullptr)tspec->bind(GL_TEXTURE_2D);
			else glBindTexture(GL_TEXTURE_2D, 0);
			textureBinds++;
			glActiveTexture(GL_TEXTURE0);
		}
		temp[i].go->nobindRender(prev, c, lights);
		rendered++;
	}
	if (pro != nullptr)pro->unbind(prev);
	if (t != nullptr)t->unbind(GL_TEXTURE_2D);
	//printf("%u rendered, %u bound, %u unbound, %u textures bound, %u shaders swapped\n", rendered, binds, unbinds+1, textureBinds+1, shaderSwaps);
}
//Camera distance
float LinkedObject::cdist(const LinkedObject *lo) {
	return (LinkedObject::c->getPosition() - lo->go->getPosition()).squareLength();
}

//Sorts on:
//#1 Shader
//#2 Model
//#3 Texture
//#4 Camera dist
bool LinkedObject::operator<(const LinkedObject &lo){
	bool eq0 = s == lo.s;
	bool eq1 = go->getRO() == lo.go->getRO();
	bool eq2 = go->getT() == lo.go->getT();
	return s < lo.s || 
		((eq0 && go->getRO() < lo.go->getRO()) 
			|| ((eq0 && eq1 && go->getT() < lo.go->getT()) || 
				(eq0 && eq1 && eq2 && cdist(this) > cdist(&lo))));
}
