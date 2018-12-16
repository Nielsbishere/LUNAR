#pragma once

#include "../Math/Types.h"

#include <string>

namespace osomi {
	class mat4;
	class vec2;
	class vec3;
	class vec4;
	class Texture;
	class Shader {
		friend class Resources;
	private:
		std::string name;
		GLuint s;
	protected:
		//ONLY FOR INHERITANCE!
		Shader(std::string n, GLuint id) : s(id), name(n) {}
		static GLuint shader(GLenum type, const char *shaderSrc);

		//For loading shaders!
		static Shader *create(std::string vert, std::string frag, std::string name, u8 flags);
		
		~Shader() {
			glDeleteProgram(s);
		}
		std::string getName(){ return name; }
	public:
		void use() {
			glUseProgram(s);
		}
		void set(std::string pos, float f);
		void set(std::string pos, int i);
		void set(std::string pos, bool b);
		void set(std::string pos, mat4 m);
		void set(std::string pos, vec2 v);
		void set(std::string pos, vec3 m);
		void set(std::string pos, vec4 m);
		void set(std::string pos, vec3 *va, u32 len);
		void set(std::string pos, vec4 *va, u32 len);
		
		#ifdef __EGL2__
		int getLocation(std::string st) { return glGetAttribLocation(s, st.c_str()); }
		#endif
	};
}