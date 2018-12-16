#include "../../Headers/Math/MathDefines.h"
#include <iostream>
#include <cmath>

using namespace osomi;

mat4::mat4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33) {
	m[0][0] = m00; m[1][0] = m01; m[2][0] = m02; m[3][0] = m03;
	m[0][1] = m10; m[1][1] = m11; m[2][1] = m12; m[3][1] = m13;
	m[0][2] = m20; m[1][2] = m21; m[2][2] = m22; m[3][2] = m23;
	m[0][3] = m30; m[1][3] = m31; m[2][3] = m32; m[3][3] = m33;
}
mat4::mat4(vec4 v0, vec4 v1, vec4 v2, vec4 v3) : mat4(v0.x, v0.y, v0.z, v0.w, v1.x, v1.y, v1.z, v1.w, v2.x, v2.y, v2.z, v2.w, v3.x, v3.y, v3.z, v3.w) {}
mat4::mat4() : mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) {}

mat4::arr &mat4::operator[](const unsigned int i) {
	return m[i];
}
void mat4::operator=(mat4 m) {
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			this->m[i][j] = m[i][j];
}
mat4 mat4::operator*(mat4 m) {
	mat4 mm;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			mm[i][j] = (*this).horizontal(j) * m.vertical(i);
	return mm;
}
mat4 mat4::operator*(float f) {
	mat4 mm;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			mm[i][j] = (*this)[i][j] * f;
	return mm;
}
mat4 &mat4::operator*=(float f) {
	(*this) = (*this) * f;
	return *this;
}
vec4 mat4::operator*(vec4 v) {
	vec4 res;
	for (int i = 0; i < 4; i++)
		res[i] = (*this).horizontal(i) * v;
	return res;
}
mat4 mat4::operator+(mat4 m) {
	mat4 returned = mat4::zero();
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			returned.m[i][j] = this->m[i][j] + m.m[i][j];
	return returned;
}
mat4 mat4::operator-(mat4 m) {
	mat4 returned = mat4::zero();
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			returned.m[i][j] = this->m[i][j] - m.m[i][j];
	return returned;
}

vec4 mat4::horizontal(const unsigned int j) {
	return vec4(m[0][j], m[1][j], m[2][j], m[3][j]);
} 
vec4 mat4::vertical(const unsigned int i) {
	return vec4(m[i][0], m[i][1], m[i][2], m[i][3]);
}

float mat4::det33() {
	float a = m[0][0], b = m[1][0]/*, c = m[2][0]*/, d = m[0][1], e = m[1][1], f = m[2][1], g = m[0][2], h = m[1][2], i = m[2][2];
	float m1 = e * i - f * h;
	float m2 = d * i - f * g;
	float m3 = d * h - e * g;
	return a * m1 - b * m2 + b * m3;
}
float mat4::determinant() {
	float a = m[0][0]; float b = m[1][0]; float c = m[2][0]; float d = m[3][0];
	float e = m[0][1]; float f = m[1][1]; float g = m[2][1]; float h = m[3][1];
	float i = m[0][2]; float j = m[1][2]; float k = m[2][2]; float l = m[3][2];
	float n = m[1][3]; float o = m[2][3]; float p = m[3][3]; float m = this->m[0][3];

	float m00 = k * p - o * l;
	float m01 = j * p - n * l;
	float m02 = j * o - n * k;

	float m10 = k * p - o * l;
	float m11 = i * p - m * l;
	float m12 = i * o - m * k;

	float m20 = j * p - n * l;
	float m21 = i * p - m * l;
	float m22 = i * n - m * j;

	float m30 = j * o - n * k;
	float m31 = i * o - m * k;
	float m32 = i * n - m * j;

	float m0 = f * m00 - g * m01 + h * m02;
	float m1 = e * m10 - g * m11 + h * m12;
	float m2 = e * m20 - f * m21 + h * m22;
	float m3 = e * m30 - f * m31 + g * m32;

	return a * m0 - b * m1 + c * m2 - d * m3;
}
mat4 mat4::getCofactor() {
	mat4 m00 = mat4(m[1][1], m[2][1], m[3][1], 0, m[1][2], m[2][2], m[3][2], 0, m[1][3], m[2][3], m[3][3], 0, 0, 0, 0, 0);
	mat4 m01 = mat4(m[1][0], m[2][0], m[3][0], 0, m[1][2], m[2][2], m[3][2], 0, m[1][3], m[2][3], m[3][3], 0, 0, 0, 0, 0);
	mat4 m02 = mat4(m[1][0], m[2][0], m[3][0], 0, m[1][1], m[2][1], m[3][1], 0, m[1][3], m[2][3], m[3][3], 0, 0, 0, 0, 0);
	mat4 m03 = mat4(m[1][0], m[2][0], m[3][0], 0, m[1][1], m[2][1], m[3][1], 0, m[1][2], m[2][2], m[3][2], 0, 0, 0, 0, 0);

	mat4 m10 = mat4(m[0][1], m[2][1], m[3][1], 0, m[0][2], m[2][2], m[3][2], 0, m[0][3], m[2][3], m[3][3], 0, 0, 0, 0, 0);
	mat4 m11 = mat4(m[0][0], m[2][0], m[3][0], 0, m[0][2], m[2][2], m[3][2], 0, m[0][3], m[2][3], m[3][3], 0, 0, 0, 0, 0);
	mat4 m12 = mat4(m[0][0], m[2][0], m[3][0], 0, m[0][1], m[2][1], m[3][1], 0, m[0][3], m[2][3], m[3][3], 0, 0, 0, 0, 0);
	mat4 m13 = mat4(m[0][0], m[2][0], m[3][0], 0, m[0][1], m[2][1], m[3][1], 0, m[0][2], m[2][2], m[3][2], 0, 0, 0, 0, 0);

	mat4 m20 = mat4(m[0][1], m[1][1], m[3][1], 0, m[0][2], m[1][2], m[3][2], 0, m[0][3], m[1][3], m[3][3], 0, 0, 0, 0, 0);
	mat4 m21 = mat4(m[0][0], m[1][0], m[3][0], 0, m[0][2], m[1][2], m[3][2], 0, m[0][3], m[1][3], m[3][3], 0, 0, 0, 0, 0);
	mat4 m22 = mat4(m[0][0], m[1][0], m[3][0], 0, m[0][1], m[1][1], m[3][1], 0, m[0][3], m[1][3], m[3][3], 0, 0, 0, 0, 0);
	mat4 m23 = mat4(m[0][0], m[1][0], m[3][0], 0, m[0][1], m[1][1], m[3][1], 0, m[0][2], m[1][2], m[3][2], 0, 0, 0, 0, 0);

	mat4 m30 = mat4(m[0][1], m[1][1], m[2][1], 0, m[0][2], m[1][2], m[2][2], 0, m[0][3], m[1][3], m[2][3], 0, 0, 0, 0, 0);
	mat4 m31 = mat4(m[0][0], m[1][0], m[2][0], 0, m[0][2], m[1][2], m[2][2], 0, m[0][3], m[1][3], m[2][3], 0, 0, 0, 0, 0);
	mat4 m32 = mat4(m[0][0], m[1][0], m[2][0], 0, m[0][1], m[1][1], m[2][1], 0, m[0][3], m[1][3], m[2][3], 0, 0, 0, 0, 0);
	mat4 m33 = mat4(m[0][0], m[1][0], m[2][0], 0, m[0][1], m[1][1], m[2][1], 0, m[0][2], m[1][2], m[2][2], 0, 0, 0, 0, 0);
	return mat4(m00.det33(), m01.det33(), m02.det33(), m03.det33(), m10.det33(), m11.det33(), m12.det33(), m13.det33(), m20.det33(), m21.det33(), m22.det33(), m23.det33(), m30.det33(), m31.det33(), m32.det33(), m33.det33());
}
bool mat4::inverse() {
	float det;
	if ((det = determinant()) == 0)return false;
	mat4 cofactor = getCofactor();
	mat4 adjoint = cofactor.transpose();
	(*this) = adjoint * (1 / det);
	return true;
}
mat4 mat4::forceInverse() {
	mat4 copy = *this;
	if (copy.inverse())return copy;
	return mat4::zero();
}

mat4 mat4::identity() {
	return mat4();
}
mat4 mat4::zero() {
	return mat4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}
mat4 mat4::translate(vec3 v) {
	return identity().setTranslate(v);
}
mat4 mat4::scale(vec3 v) {
	return mat4(v.x, 0, 0, 0, 0, v.y, 0, 0, 0, 0, v.z, 0, 0, 0, 0, 1);
}
mat4 mat4::rotateX(float x) {
	mat4 m;
	m[1][1] = m[2][2] = cos(x);
	m[2][1] = -sin(x);
	m[1][2] = sin(x);
	return m;
}
mat4 mat4::rotateY(float y) {
	mat4 m;
	m[0][0] = m[2][2] = cos(y);
	m[0][2] = -sin(y);
	m[2][0] = sin(y);
	return m;
}
mat4 mat4::rotateZ(float z) {
	mat4 m;
	m[0][0] = m[1][1] = cos(z);
	m[1][0] = -sin(z);
	m[0][1] = sin(z);
	return m;
}
mat4 mat4::rotate(vec3 rot) {
	return rotateX(rot.x) * rotateY(rot.y) * rotateZ(rot.z);
}
mat4 mat4::rotateDeg(vec3 rot) {
	return rotateX(rot.x / 180 * M_PI) * rotateY(rot.y / 180 * M_PI) * rotateZ(rot.z / 180 * M_PI);
}
mat4 mat4::getModel(vec3 t, vec3 rot, vec3 s) {
	return translate(t) * rotate(rot) * scale(s);
}
mat4 mat4::getModelDeg(vec3 t, vec3 rot, vec3 s) {
	return translate(t) * rotateDeg(rot) * scale(s);
}

mat4 mat4::ortho(float l, float r, float b, float t, float n, float f) {
	float w = r - l;
	float h = t - b;
	mat4 m;
	m[0][0] = 2 / w;
	m[1][1] = 2 / h;
	m[2][2] = 1 / (f - n);
	m[3][2] = -n / (f - n);
	return m;
}
mat4 mat4::perspective(float fov, float asp, float n, float f) {
	mat4 m;
	float scale = 1 / tan(fov * 0.5f / 180 * M_PI);
	m[0][0] = scale / asp;
	m[1][1] = scale;
	m[2][2] = -((f + n) / (f - n));
	m[3][3] = 0;
	m[2][3] = -1;
	m[3][2] = -(2 * f * n / (f - n));
	return m;
}
mat4 mat4::lookat(vec3 eye, vec3 center, vec3 up) {
	vec3 z = (eye - center).normalize();
	vec3 x = (up.cross(z)).normalize();
	vec3 y = (z.cross(x)).normalize();
	mat4 m = identity().setOrientation(x, y, z);
	return m * translate(-eye);
}
mat4 mat4::transpose() {
	mat4 r;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			r[j][i] = m[i][j];
	return r;
}
mat4 &mat4::setOrientation(vec3 x, vec3 y, vec3 z) {
	m[0][0] = x.x; m[1][0] = x.y; m[2][0] = x.z;
	m[0][1] = y.x; m[1][1] = y.y; m[2][1] = y.z;
	m[0][2] = z.x; m[1][2] = z.y; m[2][2] = z.z;
	return *this;
}
mat4 &mat4::setOrientation(mat4 _){
	m[0][0] = _[0][0]; m[0][1] = _[0][1]; m[0][2] = _[0][2];
	m[1][0] = _[1][0]; m[1][1] = _[1][1]; m[1][2] = _[1][2];
	m[2][0] = _[2][0]; m[2][1] = _[2][1]; m[2][2] = _[2][2];
	return *this;
}
mat4 mat4::getOrientation(){
	mat4 _;
	_[0][0] = m[0][0]; _[0][1] = m[0][1]; _[0][2] = m[0][2];
	_[1][0] = m[1][0]; _[1][1] = m[1][1]; _[1][2] = m[1][2];
	_[2][0] = m[2][0]; _[2][1] = m[2][1]; _[2][2] = m[2][2];
	return _;
}
vec3 mat4::getScale(){
	return vec3(m[0][0], m[1][1], m[2][2]);
}
mat4 &mat4::setScale(vec3 sc){
	m[0][0] = sc.x;
	m[1][1] = sc.y;
	m[2][2] = sc.z;
	return *this;
}
vec3 mat4::getTranslate() {
	return vec3(m[3][0], m[3][1], m[3][2]);
}
mat4 &mat4::setTranslate(vec3 v) {
	m[3][0] = v.x;
	m[3][1] = v.y;
	m[3][2] = v.z;
	return *this;
}
vec3 mat4::getXAxis() {
	return vec3(m[0][0], m[1][0], m[2][0]);
}
vec3 mat4::getYAxis() {
	return vec3(m[0][1], m[1][1], m[2][1]);
}
vec3 mat4::getZAxis() {
	return vec3(m[0][2], m[1][2], m[2][2]);
}
vec3 mat4::getForward() {
	return getZAxis();
}
vec3 mat4::getRight() {
	return getXAxis();
}
vec3 mat4::getUp() {
	return getYAxis();
}

void mat4::print() {
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) printf("%f\t", m[i][j]);
		printf("\n");
	}
	printf("\n");
}