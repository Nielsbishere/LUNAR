#pragma once
#include "../Math/MathDefines.h"
#include "Plane.h"
#include "AABB.h"
#include <iostream>
namespace osomi{
	class Frustum {
	private:
		float fov, aspect, far, near;
		vec3 center, direction, up;
		Plane planes[6];
	public:
		Frustum(float pov, float spct, float f, float n, vec3 eye, vec3 cnter, vec3 u);
		
		float getFOV(){ return fov; }
		float getAspect(){ return aspect; }
		float getFar(){ return far; }
		float getNear(){ return near; }
		vec3 getEye(){ return center; }
		vec3 getCenter(){ return direction + center; }
		vec3 getUp(){ return up; }
		Plane operator[](unsigned int i){ return planes[i]; }
		
		mat4 getProjection(){ return mat4::perspective(fov / M_PI * 180, aspect, near, far); }
		mat4 getView(){ return mat4::lookat(center, center + direction, up); }
		
		void print();
		bool intersection(AABB box, float delta = 0);
		bool intersectionSphere(vec3 center, float r);
		bool intersectionSphere(AABB box, float delta=0);

		AABB toAABB();
	};
}