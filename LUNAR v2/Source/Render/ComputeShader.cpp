#include "../../Headers/Render/ComputeShader.h"
#include "../../Headers/Helper/StringUtils.h"
using namespace osomi;
using namespace std;

ComputeShader *ComputeShader::create(std::string name, std::string source) {
	GLuint programObject = glCreateProgram();
	if (programObject == 0) {
		printf("Couldn't create program!\n");
		return nullptr;
	}
	const char *str = source.c_str();
	GLuint cs = shader(GL_COMPUTE_SHADER, str);
	if (cs == 0) return nullptr;

	glAttachShader(programObject, cs);
	glDeleteShader(cs);

	glLinkProgram(programObject);

	GLint linked;
	glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLint RetinfoLen = 0;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &RetinfoLen);
		if (RetinfoLen > 1) {
			GLchar* infoLog = (GLchar*)malloc(sizeof(char) * RetinfoLen);
			glGetProgramInfoLog(programObject, RetinfoLen, nullptr, infoLog);
			fprintf(stderr, "Error linking program:\n%s\n", infoLog);

			vector<string> lines = StringUtils::split(source, "\r\n");
			printf("Compute shader:\n");
			for (u32 i = 0; i < lines.size(); i++)
				printf("%u\t\t%s\n", i + 1, lines[i].c_str());

			free(infoLog);
		}
		glDeleteProgram(programObject);
		return nullptr;
	}
	return new ComputeShader(name, programObject);
}

void ComputeShader::execute(u32 x, u32 y, u32 z) {
	glDispatchCompute(x, y, z);
}