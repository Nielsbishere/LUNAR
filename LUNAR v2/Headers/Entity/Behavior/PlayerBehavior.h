#pragma once
#include "../Behavior/LivingBehavior.h"
#include "../../World/GameObject.h"
#include "../../Input/InputHandler.h"
namespace osomi{
	class PlayerBehavior : public LivingBehavior{
	private:
		float forwardSpeed, rightSpeed, upSpeed, backwardSpeed;
	public:
		PlayerBehavior(BillboardHelper *bh, float maxHealth, float damage, float speedForward, float speedBackward, float speedHorizontal, float jumpSpeed): LivingBehavior(bh, maxHealth, damage), forwardSpeed(speedForward), backwardSpeed(speedBackward), rightSpeed(speedHorizontal), upSpeed(jumpSpeed){}
		virtual void onUpdate(GameObject *go, double delta, InputHandler *ih) override{
			if (ih->isKeyDown(CODE_UP))
				go->applyForce(vec3(0, 0, -1) * delta * forwardSpeed);
			if (ih->isKeyDown(CODE_DOWN))
				go->applyForce(vec3(0, 0, 1) * delta * backwardSpeed);
			if (ih->isKeyDown(CODE_LEFT))
				go->applyForce(vec3(-1, 0, 0) * delta * rightSpeed);
			if (ih->isKeyDown(CODE_RIGHT))
				go->applyForce(vec3(1, 0, 0) * delta * rightSpeed);
			if (go->isOnGround() && ih->isKeyPressed(CODE_SPACE))
				go->applyForce(vec3(0, 1, 0) * upSpeed);
			
			if (ih->isKeyDown(CODE_UP) || ih->isKeyDown(CODE_DOWN) || ih->isKeyDown(CODE_LEFT) || ih->isKeyDown(CODE_RIGHT))
				go->setAnimationState("run");
			else
				go->setAnimationState("stand");
		}
	};
}