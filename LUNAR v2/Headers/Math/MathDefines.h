#pragma once

#define M_PI 3.1415926535f

#include <cmath>

class math {
public:
	static bool getLowestRoot(float a, float b, float c, float maxR, float* root);
	template<class T> static T fraction(T x){ return x - floor(x); }
	template<class T> static T absf(T x){ return x < 0 ? -x : x; }
	template<class T> static T max(T i0, T i1){ return i0 < i1 ? i1 : i0; }
	template<class T> static T ceil(T i0){ return floor(i0) == i0 ? i0 : floor(i0) + 1; }
	template<class T> static T min(T i0, T i1){ return i0 > i1 ? i1 : i0; }
	template<class T> static T clamp(T i0, T mi, T ma){ return i0 > ma ? ma : (i0 < mi ? mi : i0); }
	template<class T> static bool base2(T t){ return fraction(log(t) / log(2)) == 0; }
	template<class T> static T deg2rad(T t){ return t * M_PI / 180;}
	template<class T> static T rad2deg(T t){ return t / M_PI * 180;}
};

#include "../../Headers/Math/vec2.h"
#include "../../Headers/Math/vec3.h"
#include "../../Headers/Math/vec4.h"
#include "../../Headers/Math/mat4.h"

#include "../../Headers/Math/Types.h"