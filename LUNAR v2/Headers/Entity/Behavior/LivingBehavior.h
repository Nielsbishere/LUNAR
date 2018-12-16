#pragma once
#include "../Behavior.h"
#include "../../World/GameObject.h"
#include "../../Math/MathDefines.h"
namespace osomi{
	class LivingBehavior : public Behavior{
	private:
		float health, maxHealth;
		float damage;
	public:
		LivingBehavior(BillboardHelper *bh, float _maxHealth, float _damage) : Behavior(bh, "LivingBehavior"), health(_maxHealth), maxHealth(_maxHealth), damage(_damage){}
		virtual ~LivingBehavior(){}
		virtual void onUpdate(GameObject *go, double delta) override {
			if (health <= 0){
				go->kill();
			}
		}
		float getHealth(){ return health; }
		float getMaxHealth(){ return maxHealth; }
		float getDamage(){ return damage; }
		float getHealthPercentage(){ return math::clamp(health / maxHealth, 0.f, 1.f);}
		
		void setDamage(float f){ damage = f; }
		
		void hurt(float d){
			health -= d;
			if (health < 0)health = 0;
		}
		void heal(float d){
			health += d;
			if (health > maxHealth)health = maxHealth;
		}
		
		virtual void init(GameObject *go) override {
			vec3 size = go->getAABB().getSize();
			addBillboard(go, "Bars", vec3(0, size.y, 0), vec3(size.x / 2, size.y / 10, size.z / 2), vec4(0, 70 / 256.f, 214 / 256.f, 105 / 256.f), vec3(1, 1, 1));
			addBillboard(go, "Bars", vec3(0, size.y + size.y / 10 * 2, 0), vec3(size.x / 3, size.y / 13, size.z / 3), vec4(0, 37 / 256.f, 214 / 256.f, 70 / 256.f), vec3(1, 1, 1));
		}
	};
}