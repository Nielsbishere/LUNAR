#include "../../Headers/Math/MathDefines.h"
#include <cmath>
#include <iostream>

using namespace osomi;

vec3::vec3() : vec3(0, 0, 0) {}
vec3::vec3(float v0, float v1, float v2) {
	x = v0;
	y = v1;
	z = v2;
}
vec3::vec3(vec2 v, float v3) : vec3(v.x, v.y, v3) {}
vec3::vec3(vec4 v): vec3(v.x, v.y, v.z) {}

float &vec3::operator[](const unsigned int j) {
	const unsigned int i = j % 3;
	switch (i) {
	case 1:
		return y;
	case 2:
		return z;
	default:
		return x;
	}
}
vec3 vec3::operator*(const float f) {
	vec3 vec;
	for (int i = 0; i < 3; i++)
		vec[i] = f * (*this)[i];
	return vec;
}
vec3 vec3::operator/(const float f) {
	vec3 vec;
	for (int i = 0; i < 3; i++)
		vec[i] = (*this)[i] / f;
	return vec;
}
vec3 vec3::operator/(vec3 v) {
	vec3 vec;
	for (int i = 0; i < 3; i++)
		vec[i] = (*this)[i] / v[i];
	return vec;
}
vec3 vec3::operator+(vec3 v) {
	vec3 vec;
	for (int i = 0; i < 3; i++)
		vec[i] = v[i] + (*this)[i];
	return vec;
}
vec3 vec3::operator-(vec3 v) {
	return (*this) + -v;
}
vec3 vec3::operator+(float f) {
	vec3 vec;
	for (int i = 0; i < 3; i++)
		vec[i] = f + (*this)[i];
	return vec;
}
vec3 vec3::operator-(float f) {
	return (*this) + -f;
}
vec3 vec3::operator-() const {
	return vec3(-x, -y, -z);
}
float vec3::dot(vec3 v) {
	float r = 0;
	for (int i = 0; i < 3; i++)
		r += v[i] * (*this)[i];
	return r;
}
float vec3::operator*(vec3 v) {
	return dot(v);
}
void vec3::operator+=(vec3 v) {
	for (int i = 0; i < 3; i++)
		(*this)[i] += v[i];
}
void vec3::operator-=(vec3 v) {
	for (int i = 0; i < 3; i++)
		(*this)[i] -= v[i];
}
void vec3::operator*=(float f) {
	for (int i = 0; i < 3; i++)
		(*this)[i] *= f;
}
void vec3::operator*=(vec3 v) {
	for (int i = 0; i < 3; i++)
		(*this)[i] *= v[i];
}
void vec3::operator/=(float f) {
	for (int i = 0; i < 3; i++)
		(*this)[i] /= f;
}
float vec3::squareLength() {
	float f = 0;
	for (int i = 0; i < 3; i++)
		f += (*this)[i] * (*this)[i];
	return f;
}
float vec3::length() {
	return sqrt(squareLength());
}
float vec3::magnitude() {
	return sqrt(squareLength());
}
vec3 vec3::normalize() {
	vec3 cpy = *this;
	cpy /= length();
	return cpy;
}
vec3 vec3::scale(vec3 v) {
	return vec3(x * v.x, y * v.y, z * v.z);
}
bool vec3::operator==(vec3 oth) {
	for (int i = 0; i < 3; i++)
		if ((*this)[i] != oth[i])return false;
	return true;
}
bool vec3::operator!=(vec3 oth) {
	return !(*this == oth);
}
void vec3::print() {
	printf("%f, %f, %f\n", x, y, z);
}
vec3 vec3::cross(vec3 v) {
	return vec3(y*v.z - v.y*z, z*v.x - v.z*x, x*v.y - v.x*y);
}
vec3 vec3::abs(){
	return vec3(math::absf(x), math::absf(y), math::absf(z));
}
vec3 vec3::inverse() {
	return vec3(1 / x, 1 / y, 1 / z);
}
vec3 vec3::zero() {
	return vec3();
}
vec3 vec3::min(){
	unsigned int fmin = 0xff7fffff;
	float mi = *((float*)&fmin);
	return vec3(mi, mi, mi);
}
vec3 vec3::max() {
	unsigned int fmax = 0x7f7fffff;
	float ma = *((float*)&fmax);
	return vec3(ma, ma, ma);
}
vec3 vec3::min(vec3 a, vec3 b){
	return vec3(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z);
}
vec3 vec3::max(vec3 a, vec3 b) {
	return vec3(a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z);
}

bool vec3::operator>=(float f){
	return x >= f && y >= f && z >= f;
}
bool vec3::operator<=(float f){
	return x <= f && y <= f && z <= f;
}
bool vec3::operator>(float f){
	return x > f && y > f && z > f;
}
bool vec3::operator<(float f){
	return x < f && y < f && z < f;
}