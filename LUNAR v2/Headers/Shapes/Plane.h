#pragma once

#include "../Math/MathDefines.h"
#include "Ray.h"

namespace osomi{
	class Plane {
	private:
		vec3 n, p0;
		float D;
	public:
		Plane(vec3 _n, vec3 _p0);
		Plane(vec3 _p0, vec3 _p1, vec3 _p2, bool inverseNormal = false);
		Plane();
		vec3 getNormal() { return n; }
		vec3 getPoint() { return p0; }
		float getD() { return D; }
		
		void print();
		float distance(vec3 p0);
		bool intersects(Ray r, vec3 &i);
	};
}