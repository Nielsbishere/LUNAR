#pragma once
namespace osomi {
	class vec2;
	class vec4;
	class vec3 {
	public:
		float x, y, z;
		vec3();
		vec3(float v0, float v1, float v2);
		vec3(vec2 v, float v3);
		vec3(vec4 v);


		//@return reference to the element at i
		//@behaviour automatically loops (element 3 = 0, element 5 = 2, element 6 = 0)
		float &operator[](const unsigned int i);
		vec3 operator*(const float f);
		vec3 operator/(const float f);
		vec3 operator/(vec3 v);
		vec3 operator+(vec3 v);
		vec3 operator-(vec3 v);
		vec3 operator+(float f);
		vec3 operator-(float f);
		vec3 operator-() const;
		float dot(vec3 v);
		//Dot product
		float operator*(vec3 v);
		void operator+=(vec3 v);
		void operator-=(vec3 v);
		void operator*=(float f);
		//Assign multiply (Only way to multiply vector * vector without getting dot product)
		void operator*=(vec3 v);
		void operator/=(float f);
		float squareLength();
		float length();
		float magnitude();
		vec3 normalize();
		bool operator==(vec3 oth);
		bool operator!=(vec3 oth);
		
		bool operator>=(float f);
		bool operator<=(float f);
		bool operator>(float f);
		bool operator<(float f);

		//Print vector contents
		void print();

		vec3 cross(vec3 v);
		vec3 abs();

		vec3 scale(vec3 v);

		//Invert copy of this vector
		vec3 inverse();

		//@return null vector
		static vec3 zero();
		
		//@return maximum vector (x,y,z are FLOAT_MAX)
		static vec3 max();
		
		//@return minimum vector (x,y,z are FLOAT_MIN)
		static vec3 min();
		
		//@return maximum vector; it picks the highest values from two vectors
		static vec3 max(vec3 a, vec3 b);
		
		//@return minimum vector; it picks the lowest values from two vectors
		static vec3 min(vec3 a, vec3 b);
	};
}
