#pragma once

#include "../../Headers/Math/MathDefines.h"

namespace osomi{
	class Ray {
	private:
		vec3 p0, d;
	public:
		Ray(vec3 a, vec3 delta): p0(a), d(delta) {}
		
		vec3 getOrigin(){ return p0; }
		vec3 getDirection(){ return d; }
		
		void print();
		vec3 pointOnRay(float T);
	};
}