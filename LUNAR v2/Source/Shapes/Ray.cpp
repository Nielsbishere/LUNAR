#include "../../Headers/Shapes/Ray.h"
#include <iostream>

using namespace osomi;

vec3 Ray::pointOnRay(float T) {
	return p0 + d * T;
}
void Ray::print() {
	printf("Ray starting at %f,%f,%f heading towards %f,%f,%f\n", p0.x, p0.y, p0.z, d.x, d.y, d.z);
}