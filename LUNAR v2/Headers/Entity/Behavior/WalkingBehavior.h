#pragma once
#include "../Behavior.h"
#include "../../World/GameObject.h"
namespace osomi{
	class WalkingBehavior : public Behavior{
	private:
		bool isWalking;
		float speed;
		float maxSpeed;
	public:
		WalkingBehavior(BillboardHelper *bh) : Behavior(bh, "WalkingBehavior"), isWalking(true), speed(1), maxSpeed(5){}
		void onUpdate(GameObject *go, double delta) override {
			if (!isWalking)return;
			vec3 fwd = go->getForward();
			vec3 d = fwd * speed * delta;
			go->applyForce(d);
		}
	};
}