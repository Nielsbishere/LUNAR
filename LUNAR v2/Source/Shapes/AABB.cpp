#include "../../Headers/Shapes/AABB.h"
#include <iostream>
using namespace osomi;
void AABB::print() {
	printf("AABB from %f,%f,%f to %f,%f,%f\n", min.x, min.y, min.z, max.x, max.y, max.z);
}
		
		
AABB AABB::encapsulate(AABB a0, AABB a1) {
	vec3 min = vec3::min(a0.getMin(), a1.getMin());
	vec3 max = vec3::max(a0.getMax(), a1.getMax());
	return AABB(min, max);
}
AABB AABB::operator*(mat4 m){
	return AABB(m * vec4(min, 1), m * vec4(max, 1));
}
AABB AABB::operator+(vec3 v){
	return AABB(min + v, max + v);
}
		
bool AABB::intersect(AABB &b) {
	vec3 dist = (b.center - center).abs();
	if (dist.x >= b.hsize.x + hsize.x)return false;
	if (dist.y >= b.hsize.y + hsize.y)return false;
	if (dist.z >= b.hsize.z + hsize.z)return false;
	return true;
}
bool AABB::intersect(vec3 point) {
	return !(point.x < min.x || point.y < min.y || point.z < min.z || point.x > max.x || point.y > max.y || point.z > max.z);
}
		
AABB AABB::within(AABB b) {
	vec3 size = b.max - b.min;
	return AABB(b.min + size * min, b.min + size * max);
}
AABB::AABB(vec3 p0, vec3 p1) {
	min = vec3();
	max = vec3();
	min.x = p0.x < p1.x ? p0.x : p1.x;
	min.y = p0.y < p1.y ? p0.y : p1.y;
	min.z = p0.z < p1.z ? p0.z : p1.z;
	max.x = p0.x >= p1.x ? p0.x : p1.x;
	max.y = p0.y >= p1.y ? p0.y : p1.y;
	max.z = p0.z >= p1.z ? p0.z : p1.z;
	
	if (min.x == max.x){
		min.x = -0.001f;
		max.x = -min.x;
	}
	if (min.y == max.y) {
		min.y = -0.001f;
		max.y = -min.y;
	}
	if (min.z == max.z) {
		min.z = -0.001f;
		max.z = -min.z;
	}
	
	hsize = (max - min) / 2;
	center = min + hsize;
}
AABB::AABB(): AABB(vec3(), vec3()) {}
vec3 AABB::operator[](unsigned int i) {
	switch (i) {
	case 1:
		return max;
	default:
		return min;
	}
}