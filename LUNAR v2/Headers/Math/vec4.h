#pragma once
namespace osomi {
	class vec2;
	class vec3;
	class vec4 {
	public:
		float x, y, z, w;
		vec4();
		vec4(float v0, float v1, float v2, float v3);
		vec4(vec2 v, float v3, float v4);
		vec4(vec3 v, float v4);

		//@return reference to the element at i
		//@behaviour automatically loops (element 4 = 0, element 7 = 3, element 8 = 0)
		float &operator[](const unsigned int i);
		vec4 operator*(const float f);
		vec4 operator/(const float f);
		vec4 operator/(vec4 v);
		vec4 operator+(vec4 v);
		vec4 operator-(vec4 v);
		vec4 operator+(float f);
		vec4 operator-(float f);
		vec4 operator-() const;
		float dot(vec4 v);
		//Dot product
		float operator*(vec4 v);
		void operator+=(vec4 v);
		void operator-=(vec4 v);
		void operator*=(float f);
		//Assign multiply (Only way to multiply vector * vector without getting dot product)
		void operator*=(vec4 v);
		void operator/=(float f);
		float squareLength();
		float length();
		float magnitude();
		vec4 normalize();
		bool operator==(vec4 oth);
		bool operator!=(vec4 oth);
		
		bool operator>=(float f);
		bool operator<=(float f);
		bool operator>(float f);
		bool operator<(float f);

		//Print vector contents
		vec4 scale(vec4 v);

		void print();

		//Invert copy of this vector
		vec4 inverse();

		//@return null vector
		static vec4 zero();
	};
}
