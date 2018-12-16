#include "../../Headers/Render/Shader.h"
#include "../../Headers/Math/MathDefines.h"

#include <string>

#include "../../Headers/Resource/Resources.h"
#include "../../Headers/Helper/StringUtils.h"

using namespace osomi;
using namespace std;

GLuint Shader::shader(GLenum type, const char *shaderSrc) {
	GLuint sh = glCreateShader(type);
	
	if (sh == 0) return GL_FALSE;
	
	glShaderSource(sh, 1, &shaderSrc, nullptr);
	glCompileShader(sh);
	
	GLint hasErrors;
	
	glGetShaderiv(sh, GL_COMPILE_STATUS, &hasErrors);
	if (!hasErrors) {
		GLint RetinfoLen = 0;
		glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &RetinfoLen);
		if (RetinfoLen > 1) {
			char* infoLog = (char*) malloc(sizeof(char) * RetinfoLen);
			glGetShaderInfoLog(sh, RetinfoLen, NULL, infoLog);
			fprintf(stderr, "Error compiling this shader:\n%s\n%s\n", infoLog, shaderSrc);
			free(infoLog);
		}
		glDeleteShader(sh);
		return GL_FALSE;
	}
	return sh;
}

void Shader::set(string pos, float f) {
	GLuint l = glGetUniformLocation(s, pos.c_str());
	glUniform1f(l, f);
}
void Shader::set(string pos, int i) {
	GLuint l = glGetUniformLocation(s, pos.c_str());
	glUniform1i(l, i);
}
void Shader::set(string pos, bool b) {
	GLuint l = glGetUniformLocation(s, pos.c_str());
	glUniform1i(l, b);
}
void Shader::set(string pos, mat4 m) {
	GLuint l = glGetUniformLocation(s, pos.c_str());
	glUniformMatrix4fv(l, 1, GL_FALSE, &m[0][0]);
}
void Shader::set(string pos, vec2 v) {
	GLuint l = glGetUniformLocation(s, pos.c_str());
	glUniform2fv(l, 1, &v.x);
}
void Shader::set(string pos, vec3 v) {
	GLuint l = glGetUniformLocation(s, pos.c_str());
	glUniform3fv(l, 1, &v.x);
}
void Shader::set(string pos, vec4 v) {
	GLuint l = glGetUniformLocation(s, pos.c_str());
	glUniform4fv(l, 1, &v.x);
}
void Shader::set(std::string pos, vec3 *va, u32 len) {
	GLuint l = glGetUniformLocation(s, pos.c_str());
	glUniform3fv(l, len, (f32*)va);
}
void Shader::set(std::string pos, vec4 *va, u32 len) {
	GLuint l = glGetUniformLocation(s, pos.c_str());
	glUniform4fv(l, len, (f32*)va);
}

string parseFunc(vector<u32> vlocationsn, vector<string> vlines, string s, string s1){
	u32 len = vlocationsn.size() / 2;
	for (u32 i = len - 1; i != 0xFFFFFFFF; i--) {
		u32 start = vlocationsn[i * 2];
		u32 end = vlocationsn[i * 2 + 1];
		for (u32 j = end; j >= start && j != 0xFFFFFFFF; j--)
			vlines.erase(vlines.begin() + j);
	}
	vector<string> toErase;
	toErase.push_back(s);
	toErase.push_back(s1);
	
	vlines = StringUtils::erase(vlines, toErase, true);

	ostringstream ss;
	ss << std::endl;
	string lineend = ss.str();

	string vert = "";
	for (u32 i = 0; i < vlines.size(); i++)
		vert = vert + vlines[i] + lineend; 
	
	return vert;
}

void parse(string &vert, string &frag, string s1, string s2, string s3, string s4){
	ostringstream ss;
	ss << std::endl;
	string lineend = ss.str();
	vector<string> vlines = StringUtils::split(vert, lineend);
	vector<string> flines = StringUtils::split(frag, lineend);
	
	vector<u32> vlocationsn;
	StringUtils::find(vlines, s3, s4, vlocationsn, true);
	
	vector<u32> flocationsn;
	StringUtils::find(flines, s3, s4, flocationsn, true);
	
	if(vlocationsn.size() != 0)
		vert = parseFunc(vlocationsn, vlines, s1, s2);
	
	if (flocationsn.size() != 0)
		frag = parseFunc(flocationsn, flines, s1, s2);
}

Shader *Shader::create(string vert, string frag, string name, u8 flags) {
	//Deferred rendering
	if (flags & 0x2) {
		parse(vert, frag, "RenderingDeferred", "EndDeferred", "RenderingForward", "EndForward");
		//Lighting on
		if(flags & 0x1){
			parse(vert, frag, "RenderingLighting", "EndLighting", "RenderingAlbedo", "EndAlbedo");
			#ifdef __EGL2__
			parse(vert, frag, "ShaderOld", "EndOld", "ShaderNew", "EndNew");
			#else
			parse(vert, frag, "ShaderNew", "EndNew", "ShaderOld", "EndOld");
			#endif
		}else{
			parse(vert, frag, "RenderingAlbedo", "EndAlbedo", "RenderingLighting", "EndLighting");
			#ifdef __EGL2__
			parse(vert, frag, "ShaderOld", "EndOld", "ShaderNew", "EndNew");
			#else
			parse(vert, frag, "ShaderNew", "EndNew", "ShaderOld", "EndOld");
			#endif
		}
	}
	else{
		parse(vert, frag, "RenderingForward", "EndForward", "RenderingDeferred", "EndDeferred");
		//Lighting on
		if(flags & 0x1){
			parse(vert, frag, "RenderingLighting", "EndLighting", "RenderingAlbedo", "EndAlbedo");
			#ifdef __EGL2__
			parse(vert, frag, "ShaderOld", "EndOld", "ShaderNew", "EndNew");
			#else
			parse(vert, frag, "ShaderNew", "EndNew", "ShaderOld", "EndOld");
			#endif
		}else{
			parse(vert, frag, "RenderingAlbedo", "EndAlbedo", "RenderingLighting", "EndLighting");
			#ifdef __EGL2__
			parse(vert, frag, "ShaderOld", "EndOld", "ShaderNew", "EndNew");
			#else
			parse(vert, frag, "ShaderNew", "EndNew", "ShaderOld", "EndOld");
			#endif
		}
	}

	ostringstream ss;
	ss << std::endl;
	string lineend = ss.str();

	#ifndef __EGL2__
	vert = "#version 420 core" + lineend + vert;
	frag = "#version 420 core" + lineend + frag;
	#endif

	const char *vcs = vert.c_str();
	const char *fcs = frag.c_str();
	GLuint vertexShader = shader(GL_VERTEX_SHADER, vcs);
	if (vertexShader == 0) return nullptr;
	GLuint fragmentShader = shader(GL_FRAGMENT_SHADER, fcs);
	if (fragmentShader == 0) {
		glDeleteShader(vertexShader);
		return nullptr;
	}
	
	GLuint programObject = glCreateProgram();
	if (programObject == 0) return 0;
	
	glAttachShader(programObject, vertexShader);
	glAttachShader(programObject, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	#ifdef __EGL2__
	glBindAttribLocation(programObject, 0, "position");
	glBindAttribLocation(programObject, 1, "texCoord");
	glBindAttribLocation(programObject, 2, "normal");
	#endif
	
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
			
			vector<string> lines = StringUtils::split(vert, "\r\n");
			printf("Vertex shader:\n");
			for (u32 i = 0; i < lines.size(); i++)
				printf("%u\t\t%s\n", i+1, lines[i].c_str());
			printf("\nFragment shader:\n");
			
			lines = StringUtils::split(frag, "\r\n");
			for (u32 i = 0; i < lines.size(); i++)
				printf("%u\t\t%s\n", i+1, lines[i].c_str());
				
			free(infoLog);
		}
		glDeleteProgram(programObject);
		return nullptr;
	}
	
	return new Shader(name, programObject);
}