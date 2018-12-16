#pragma once
#include "../Math/MathDefines.h"

namespace osomi {
	class Light{
	private:
		vec3 position;
		vec3 color;
		float power;
	public:
		Light(vec3 pos, vec3 col, float pow): position(pos), color(col), power(pow){}
		vec3 getPosition(){ return position; }
		vec3 getColor(){ return color; }
		float getPower(){ return power; }
		void setPosition(vec3 pos){ position = pos; }
		void setColor(vec3 col){ color = col; }
		void setPower(float pow){ power = pow; }
		void applyMovement(vec3 vec){ setPosition(getPosition() + vec); }
		float calculateRadius() {
			if (color == vec3())return 0;
			//Solve radius;
			//5 / 256.f = hicoli / (kc + kl*R + kq*R^2)
			//5 / (256.f * hicoli) = 1 / (kc + kl*R + kq*R^2)
			//(256.f * hicoli) / 5 = kc + kl * R + kq * r^2
			//kq * r^2 + kl * r + kc - 256/5 * hicoli/5
			//ABC(kq, kl, kc - 256/5 * hicoli/5) = r
			float hicoli = color.x > color.y && color.x > color.z ? color.x : (color.y > color.x && color.y > color.z ? color.y : color.z);
			float constant = 1.0;
			float linear = 0.7;
			float quadratic = 1.8;
			return (-linear + std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * hicoli * power))) / (2 * quadratic);
		}
	};
}