#pragma once
#pragma once
#include "../Math/MathDefines.h"
#include "Plane.h"

namespace osomi{
	class Triangle {
	private:
		vec3 p0, p1, p2;
		vec3 n;
	public:
		Triangle(vec3 a, vec3 b, vec3 c, vec3 _n);
		Triangle();
		Triangle(vec3 a, vec3 b, vec3 c);

		vec3 a() { return p0; }
		vec3 b() { return p1; }
		vec3 c() { return p2; }
		Plane asPlane();
		
		Triangle operator*(mat4 m);
		
		bool intersects(Ray r);
		bool intersects(vec3 point);
		vec3 getCenter();
		Triangle recalc();
		void print();
	};
}