#include "../../Headers/Math/MathDefines.h"
#include <cmath>
#include <iostream>

using namespace osomi;

vec2::vec2() : vec2(0, 0) {}
vec2::vec2(float v0, float v1) {
	x = v0;
	y = v1;
}
vec2::vec2(vec3 v): vec2(v.x, v.y){}
vec2::vec2(vec4 v): vec2(v.x, v.y){}

float &vec2::operator[](const unsigned int j) {
	const unsigned int i = j % 2;
	switch (i) {
	case 1:
		return y;
	default:
		return x;
	}
}
vec2 vec2::operator*(const float f) {
	vec2 vec;
	for (int i = 0; i < 2; i++)
		vec[i] = f * (*this)[i];
	return vec;
}
vec2 vec2::operator/(const float f) {
	vec2 vec;
	for (int i = 0; i < 2; i++)
		vec[i] = (*this)[i] / f;
	return vec;
}
vec2 vec2::operator/(vec2 v) {
	vec2 vec;
	for (int i = 0; i < 2; i++)
		vec[i] = (*this)[i] / v[i];
	return vec;
}
vec2 vec2::operator+(vec2 v) {
	vec2 vec;
	for (int i = 0; i < 2; i++)
		vec[i] = v[i] + (*this)[i];
	return vec;
}
vec2 vec2::operator-(vec2 v) {
	return (*this) + -v;
}
vec2 vec2::operator+(float f) {
	vec2 vec;
	for (int i = 0; i < 2; i++)
		vec[i] = f + (*this)[i];
	return vec;
}
vec2 vec2::operator-(float f) {
	return (*this) + -f;
}
vec2 vec2::operator-() const{
	return vec2(-x, -y);
}
float vec2::dot(vec2 v) {
	float r = 0;
	for (int i = 0; i < 2; i++)
		r += v[i] * (*this)[i];
	return r;
}
float vec2::operator*(vec2 v) {
	return dot(v);
}
void vec2::operator+=(vec2 v) {
	for (int i = 0; i < 2; i++)
		(*this)[i] += v[i];
}
void vec2::operator-=(vec2 v) {
	for (int i = 0; i < 2; i++)
		(*this)[i] -= v[i];
}
void vec2::operator*=(float f) {
	for (int i = 0; i < 2; i++)
		(*this)[i] *= f;
}
void vec2::operator*=(vec2 v) {
	for (int i = 0; i < 2; i++)
		(*this)[i] *= v[i];
}
void vec2::operator/=(float f) {
	for (int i = 0; i < 2; i++)
		(*this)[i] /= f;
}
float vec2::squareLength() {
	float f = 0;
	for (int i = 0; i < 2; i++)
		f += (*this)[i] * (*this)[i];
	return f;
} 
float vec2::length(){
	return sqrt(squareLength());
} 
float vec2::magnitude(){
	return sqrt(squareLength());
} 
vec2 vec2::normalize(){
	vec2 cpy = *this;
	cpy /= length();
	return cpy;
}
vec2 vec2::scale(vec2 v) {
	return vec2(x * v.x, y * v.y);
}
bool vec2::operator==(vec2 oth) {
	for (int i = 0; i < 2; i++)
		if ((*this)[i] != oth[i])return false;
	return true;
}
bool vec2::operator!=(vec2 oth){
	return !(*this == oth);
} 
void vec2::print(){
	printf("%f, %f\n", x, y);
}
vec2 vec2::swap() {
	float x = y;
	float y = this->x;
	return vec2(x, y);
}
vec2 vec2::inverse() {
	return vec2(1 / x, 1 / y);
}
vec2 vec2::zero() {
	return vec2();
}

bool vec2::operator>=(float f) {
	return x >= f && y >= f;
}
bool vec2::operator<=(float f) {
	return x <= f && y <= f;
}
bool vec2::operator>(float f) {
	return x > f && y > f;
}
bool vec2::operator<(float f) {
	return x < f && y < f;
}