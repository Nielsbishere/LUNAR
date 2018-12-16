#pragma once
namespace osomi {
	class vec3;
	class vec4;
	class vec2 {
	public:
		float x, y;

		vec2();
		vec2(float v0, float v1);
		vec2(vec3 v);
		vec2(vec4 v);


		//@return reference to the element at i
		//@behaviour automatically loops (element 2 = 0, element 3 = 1, element 4 = 0)
		float &operator[](const unsigned int i);

		vec2 operator*(const float f);
		vec2 operator/(const float f);
		vec2 operator/(vec2 v);
		vec2 operator+(vec2 v);
		vec2 operator-(vec2 v);
		vec2 operator+(float f);
		vec2 operator-(float f);
		vec2 operator-() const;
		float dot(vec2 v);
		//Dot product
		float operator*(vec2 v);
		void operator+=(vec2 v);
		void operator-=(vec2 v);
		void operator*=(float f);
		//Assign multiply (Only way to multiply vector * vector without getting dot product)
		void operator*=(vec2 v);
		void operator/=(float f);
		float squareLength();
		float length();
		float magnitude();
		vec2 normalize();
		bool operator==(vec2 oth);
		bool operator!=(vec2 oth);
		
		bool operator>=(float f);
		bool operator<=(float f);
		bool operator>(float f);
		bool operator<(float f);

		//Print vector contents
		void print();
		vec2 scale(vec2 v);

		//Swap x and y
		vec2 swap();

		//Invert copy of this vector
		vec2 inverse();

		//@return null vector
		static vec2 zero();
	};
}
