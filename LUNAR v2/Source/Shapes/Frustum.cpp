#include "../../Headers/Shapes/Frustum.h"

using namespace osomi;

Frustum::Frustum(float pov, float spct, float f, float n, vec3 eye, vec3 cnter, vec3 u): fov(pov), aspect(spct), far(f), near(n), up(u.normalize()), center(eye), direction((cnter - eye).normalize()) {
	vec3 right = up.cross(direction);
	float nearH = near * 2 * tan(fov / 2), nearW = nearH * aspect;
	float farH = far * 2 * tan(fov / 2), farW = farH * aspect;

	vec3 nc = center + direction * near;
	vec3 ntl = nc + (up * nearH / 2) - (right * nearW / 2);
	vec3 ntr = ntl + right * nearW;
	vec3 nbl = ntl - up * nearH;
	vec3 nbr = nbl + right * nearW;

	vec3 fc = center + direction * far;
	vec3 ftl = fc + (up * farH / 2) - (right * farW / 2);
	vec3 ftr = ftl + right * farW;
	vec3 fbl = ftl - up * farH;
	vec3 fbr = fbl + right * farW;

	planes[0] = Plane(ftl, ftr, fbr);
	planes[1] = Plane(ntl, nbl, nbr);
	planes[2] = Plane(ftl, fbl, nbl);
	planes[3] = Plane(ftr, ntr, nbr);
	planes[4] = Plane(ftr, ftl, ntl);
	planes[5] = Plane(fbr, nbr, nbl);
}
AABB Frustum::toAABB() {
	vec3 right = up.cross(direction);
	float nearH = near * 2 * tan(fov / 2), nearW = nearH * aspect;
	float farH = far * 2 * tan(fov / 2), farW = farH * aspect;

	vec3 nc = center + direction * near;
	vec3 ntl = nc + (up * nearH / 2) - (right * nearW / 2);
	vec3 ntr = ntl + right * nearW;
	vec3 nbl = ntl - up * nearH;
	vec3 nbr = nbl + right * nearW;

	vec3 fc = center + direction * far;
	vec3 ftl = fc + (up * farH / 2) - (right * farW / 2);
	vec3 ftr = ftl + right * farW;
	vec3 fbl = ftl - up * farH;
	vec3 fbr = fbl + right * farW;

	vec3 ma = vec3::max(ftr, nbl);
	vec3 mi = ma == ftr ? nbl : ftr;

	return AABB(mi, ma);
}
void Frustum::print() {
	printf("Frustum with fov %f, aspect %f, far and near plane %f,%f, eye position %f,%f,%f, direction %f,%f,%f, up %f,%f,%f and planes:\n", fov, aspect, far, near, center.x, center.y, center.z, direction.x, direction.y, direction.z, up.x, up.y, up.z);
	for (int i = 0; i < 6; i++)
		planes[i].print();
}
bool Frustum::intersection(AABB box, float delta) {
	for (int i = 0; i < 6; i++) {
		float dp = (planes[i].getNormal().x * box[planes[i].getNormal().x > 0].x) + (planes[i].getNormal().y * box[planes[i].getNormal().y > 0].y) + (planes[i].getNormal().z * box[planes[i].getNormal().z > 0].z);
		if (dp < -(planes[i].getD() + delta))
			return false;
	}
	return true;
}
bool Frustum::intersectionSphere(vec3 center, float r) {
	for (int i = 0; i < 6; i++) 
		if (planes[i].distance(center) < -r)return false;
	return true;
}
bool Frustum::intersectionSphere(AABB box, float r) {
	return intersectionSphere(box.getCenter(), box.maxRadius() + r);
}