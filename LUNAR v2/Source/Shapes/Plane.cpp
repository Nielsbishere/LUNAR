#include "../../Headers/Shapes/Plane.h"
#include <iostream>
using namespace osomi;

Plane::Plane(vec3 _n, vec3 _p0): n(_n), p0(_p0), D(-n * p0) {}
Plane::Plane(vec3 _p0, vec3 _p1, vec3 _p2, bool inverseNormal): p0(_p0) {
	n = (_p1 - p0).cross(_p2 - p0).normalize();
	if (inverseNormal)n = -n;
	D = -n * p0;
}
Plane::Plane(): Plane(vec3(), vec3()) {}

void Plane::print() {
	printf("Plane at %f,%f,%f with normal %f,%f,%f and D %f\n", p0.x, p0.y, p0.z, n.x, n.y, n.z, D);
}
float Plane::distance(vec3 p0) {
	return (n * p0) + D;
}
bool Plane::intersects(Ray r, vec3 &i) {
	vec3 w = r.getOrigin() - p0;
	float k = (w * n) / (r.getDirection() * n);
	if (k < 0 || k > 1)return false;
	i = r.pointOnRay(k);
	return true;
}