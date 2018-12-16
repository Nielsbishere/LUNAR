#pragma once
#include "Shader.h"
namespace osomi {
	class ComputeShader : public Shader {
		friend class Resources;
	private:
		ComputeShader(std::string n, GLuint id) : Shader(n, id) {}
	protected:
		static ComputeShader *create(std::string name, std::string source);
	public:

		void execute(u32 x, u32 y, u32 z);
	};
}