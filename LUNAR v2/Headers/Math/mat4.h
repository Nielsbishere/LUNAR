#pragma once

#ifndef M_PI
#define M_PI 3.141592f
#endif

namespace osomi {
	class vec3;
	class vec4;
	class vec2;
	class mat4 {
	public:
		//Main constructor of mat4
		//@param m01 item at [1][0] (0y1x)
		mat4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33);

		//Vec4 constructor of mat4
		//Takes in 4 rows
		mat4(vec4 v0, vec4 v1, vec4 v2, vec4 v3);

		//Identity matrix
		mat4();


		//A column in the matrix
		typedef float arr[4];

		//@param unsigned int i, col number
		//@return column in the matrix
		arr& operator[](const unsigned int i);

		//@param unsigned int j, the row number
		//@return row
		vec4 horizontal(const unsigned int j);

		//@param unsigned int i, col number
		//@return row in the matrix
		vec4 vertical(const unsigned int i);

		//@param mat4 m, matrix to copy
		void operator=(mat4 m);

		//@param mat4 m, matrix to multiply by
		mat4 operator*(mat4 m);

		//@param float f, scale fator
		mat4 operator*(float f);

		//@param float f, scale factor
		mat4 &operator*=(float f);

		//@param vec4 v, vector to multiply by
		vec4 operator*(vec4 v);

		//@param mat4 m, matrix to add
		mat4 operator+(mat4 m);

		//@param mat4 m, matrix to subtract
		mat4 operator-(mat4 m);


		//@return translate, offset of the matrix
		vec3 getTranslate();

		//@param vec3 v, translate
		//@return mat4 this
		mat4 &setTranslate(vec3 v);

		//@return vec3 x
		vec3 getXAxis();

		//@return vec3 y
		vec3 getYAxis();

		//@return vec3 z
		vec3 getZAxis();

		//@return vec3 forward
		vec3 getForward();

		//@return vec3 up
		vec3 getUp();

		//@return vec3 right
		vec3 getRight();
		
		mat4 getOrientation();
		
		vec3 getScale();
		
		//@return float determinant
		float determinant();

		//@return mat4 cofactors
		mat4 getCofactor();

		//Inverses self if possible
		//@return bool is inversable
		bool inverse();

		//Inverses self if possible
		//@return mat4::zero() if invalid
		mat4 forceInverse();

		mat4 &setOrientation(vec3 x, vec3 y, vec3 z);
		
		mat4 &setOrientation(mat4 m);
		
		mat4 &setScale(vec3 sc);

		//Returns copied transposed matrix
		//@return mat4 transposed
		mat4 transpose();

		//Prints the contents of this matrix
		void print();


		//@return mat4 null
		static mat4 zero();

		//@return mat4 identity
		static mat4 identity();

		//@return mat4 translation matrix
		static mat4 translate(vec3 v);

		//@return mat4 scale matrix
		static mat4 scale(vec3 v);

		//@return mat4 rotation matrix (in radians)
		static mat4 rotate(vec3 rot);

		//@return mat4 rotation matrix (in degrees)
		static mat4 rotateDeg(vec3 rot);

		//@param vec3 translate
		//@param vec3 rotation (radians)
		//@param vec3 scale
		//@return mat4 model matrix
		static mat4 getModel(vec3 t, vec3 rot, vec3 s);

		//@param vec3 translate
		//@param vec3 rotation (degrees)
		//@param vec3 scale
		//@return mat4 model matrix
		static mat4 getModelDeg(vec3 t, vec3 rot, vec3 s);

		//@return mat4 ortho matrix (projection)
		static mat4 ortho(float l, float r, float b, float t, float n, float f);

		//@return mat4 perspective matrix (projection)
		static mat4 perspective(float fov, float asp, float n, float f);

		//@return mat4 view matrix
		static mat4 lookat(vec3 eye, vec3 center, vec3 up);
	private:
		float m[4][4];
		float det33();

		static mat4 rotateX(float x);
		static mat4 rotateY(float y);
		static mat4 rotateZ(float z);
	};
}