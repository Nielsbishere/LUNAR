#pragma once
#pragma once
#include "../../Entity/Behavior/LivingBehavior.h"
#include "../../World/GameObject.h"
#include "../../Math/MathDefines.h"
#include "../Exp/Level.h"
namespace osomi{
	namespace ss {
	class LivingBehaviorSS : public LivingBehavior {
	private:
		ELevel level;
		float experience;
//		ESkill skill;
//		EAbillity ability;
//		EGroup group;
//		ESpecial spec;
	public:
		LivingBehaviorSS(Billboard *bh, float _maxHealth, float _damage, ELevel lvl) : LivingBehavior(bh, _maxHealth, _damage), level(lvl){}
		ELevel getLevel(){ return level; }
		void onUpdate(GameObject *go, double delta) override {
			LivingBehavior::onUpdate(go, delta);
			if(level < ELevel::L100ZM){
				float exp = 0;
				if (experience >= (exp = Level::getExperience(level))){
					experience -= exp;
					level = (ELevel)(level + 1);
				}
			}
		}
	};
	}
}