#pragma once
#include "../Math/MathDefines.h"
#include "../Resource/Texture.h"
#include "../Render/RenderObject.h"
#include "../Render/AnimationState.h"
#include "../Render/Light.h"
#include <vector>
#include "Camera.h"
namespace osomi{
	class CChunk;
	struct CPackage;
	class Triangle;
	class World;
	class Behavior;
	class InputHandler;
	class GameObject {
		friend class World;
		friend class LinkedObject;
	private:
		World *w;
		std::string name;
		
		RenderObject *ro;
		Texture *t, *spec;
		vec3 rotation, position, scale, velocity;
		vec2 uvScale;
		
		bool dead;
		bool hidden, ignoresCollision;
		
		FrameState *fs;
		float animationTime;
		bool animationSwitched;
		
		//Collision
		bool defiesGravity, isStatic;
		float friction;	
		bool onGround;
		
		std::vector<Behavior*> behaviors;
		
		void collision(Triangle t, CPackage &cp, vec3 d);
		void collision(CChunk *chunk, GameObject *obj, AABB bounding, u32 &count, CPackage &cp, vec3 d);
		
		bool doCollision(double delta);
		
	protected:
		RenderObject *getRO(){ return ro; }
		Texture *getT(){ return t; }
		Texture *getSpecular() { return spec; }
		
		void nobindRender(Shader *s, Camera *c, std::vector<Light> l);
		
		GameObject(World *w, std::string name, RenderObject *render, Texture *texture, vec3 position, vec3 rotation, vec3 scale);
		
		~GameObject();
	public:
		vec3 getPosition(){ return position; }
		vec3 getScale(){ return scale; }
		vec3 getRotation(){ return rotation; }
		vec3 getForward();
		vec3 getVelocity(){ return velocity; }
		bool isOnGround(){ return onGround; }
		
		bool isDead(){ return dead; }
		bool isHidden(){ return hidden; }
		bool usesCollision(){ return !ignoresCollision; }
		
		void setHidden(bool isHidden){ hidden = isHidden; }
		void ignoreCollision(bool ignore){ ignoresCollision = ignore; }
		void setGravity(bool applies){ defiesGravity = !applies; }
		void setStatic(){ isStatic = true; }
		void setFriction(float fric){ friction = math::clamp(fric, 0.f, 1.f); }
		
		bool hasGravity(){ return !defiesGravity; }
		bool canMove(){ return !isStatic; }
		float getFriction(){ return friction; }
		
		bool isStuck();
		
		AABB getAABB(){ return ro->getAABB() * getModel(); }
		std::string getName(){ return name; }
		
		void move(vec3 v){ if(!isStatic) position = v; }
		void applyMovement(vec3 v){ if(!isStatic) position += v; }
		void setScale(vec3 size){ scale = size; }
		void setUvScale(vec2 uv){ uvScale = uv; }
		void setVelocity(vec3 vel){ velocity = vel; }
		void applyForce(vec3 dvelocity){ velocity += dvelocity; }
		void setRotation(vec3 rot){ rotation = rot; }
		void rotate(vec3 drot){ rotation += drot; }
		void kill(){ dead = true; }
		
		void setAnimationState(std::string name);

		void attachSpecular(Texture *t) { if (spec == nullptr) spec = t; }
		
		virtual void update(double delta, InputHandler *ih);
		void render(Shader *s, Camera *c, std::vector<Light> l);
		
		World *getWorld(){ return w; }
		
		void applyBehavior(Behavior *b);
		bool hasBehavior(std::string name);
		Behavior *getBehavior(std::string name);
		
		mat4 getModel(){ return mat4::getModelDeg(position, rotation, scale); }
	};
}