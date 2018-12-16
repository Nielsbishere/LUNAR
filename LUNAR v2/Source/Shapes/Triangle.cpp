#include "../../Headers/Shapes/Triangle.h"
#include <iostream>

using namespace osomi;

Triangle::Triangle(vec3 a, vec3 b, vec3 c, vec3 _n): p0(a), p1(b), p2(c), n(_n) {}
Triangle::Triangle(): Triangle(vec3(), vec3(), vec3(), vec3()) {}
Triangle::Triangle(vec3 a, vec3 b, vec3 c): Triangle(a, b, c, (b - a).cross(c - a).normalize()) {}

Triangle Triangle::operator*(mat4 m){
	return Triangle(m * vec4(p0, 1), m * vec4(p1, 1), m * vec4(p2, 1), n);
}

bool Triangle::intersects(Ray r) {
	vec3 i = vec3();
	if (!asPlane().intersects(r, i)) return false;

	vec3 ab = (p1 - p0);
	vec3 cb = (p2 - p0);
	vec3 ia = (i - p0);

	float d00 = ab * ab;
	float d01 = ab * cb;
	float d11 = cb * cb;
	float d20 = ia * ab;
	float d21 = ia * cb;

	float invdenom = 1 / (d00 * d11 - d01 * d01);
	float a = (d11 * d20 - d01 * d21) * invdenom;
	float b = (d00 * d21 - d01 * d20) * invdenom;

	return a >= 0 && b >= 0 && a <= 1 && b <= 1;
}
bool Triangle::intersects(vec3 point) {
	vec3 e10 = p1 - p0;
	vec3 e20 = p2 - p0;
	float a = e10 * e10;
	float b = e10 * e20;
	float c = e20 * e20;
	float ac_bb = (a*c) - (b*b);
	vec3 vp(point.x - p0.x, point.y - p0.y, point.z - p0.z);
	float d = vp * e10;
	float e = vp * e20;
	float x = (d*c) - (e*b);
	float y = (e*a) - (d*b);
	float z = x + y - ac_bb;
	return z < 0 && !(x < 0 || y < 0);
}
vec3 Triangle::getCenter(){ return p0 / 3 + p1 / 3 + p2 / 3; }
Triangle Triangle::recalc() {
	return Triangle(p0, p1, p2);
}
void Triangle::print() {
	printf("Triangle with coords %f,%f,%f, %f,%f,%f and %f,%f,%f. With plane attachment: ", p0.x, p0.y, p0.z, p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
	asPlane().print();
}

Plane Triangle::asPlane(){ return Plane(n, getCenter()); }