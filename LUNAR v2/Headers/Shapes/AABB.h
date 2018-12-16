#pragma once
#include "../Math/MathDefines.h"
namespace osomi{
	class AABB {
	private:
		vec3 min, max;
		vec3 hsize, center;
	public:
		AABB(vec3 p0, vec3 p1);
		AABB();
		vec3 operator[](unsigned int i);
		
		vec3 getMin() { return min; }
		vec3 getMax() { return max; }
		//@return half size (NOT FULL SIZE)
		vec3 getSize(){ return hsize; }
		float maxRadius() { return hsize.x > hsize.y && hsize.x > hsize.z ? hsize.x : (hsize.y > hsize.x && hsize.y > hsize.z ? hsize.y : hsize.z); }
		vec3 getCenter(){ return center; }
		
		void print();
		
		AABB operator*(mat4 m);
		AABB operator+(vec3 v);
		
		static AABB encapsulate(AABB a0, AABB a1);
		
		bool intersect(AABB &b);
		bool intersect(vec3 point);
		
		AABB within(AABB b);
	};
}