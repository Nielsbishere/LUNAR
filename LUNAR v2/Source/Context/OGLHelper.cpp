#include "../../Headers/Context/OGLHelper.h"
#include <iostream>
#ifndef __EGL2__
GLuint OGLHelper::createFBO() {
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	return fbo;
}
GLuint OGLHelper::createTexture(u32 w, u32 h, bool linearFiltering) {
	GLuint t;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linearFiltering ? GL_LINEAR : GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linearFiltering ? GL_LINEAR : GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return t;
}
GLuint OGLHelper::createDepthTexture(u32 w, u32 h, bool linearFiltering) {
	GLuint t;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linearFiltering ? GL_LINEAR : GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linearFiltering ? GL_LINEAR : GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return t;
}
GLuint OGLHelper::createDepth(u32 w, u32 h) {
	GLuint RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, w, h);
	return RBO;
}
void OGLHelper::attachDepth(GLuint fbo, GLuint depth) {
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
}
void OGLHelper::attachment(GLuint fbo, u32 id, GLuint texture) {
	glFramebufferTexture(GL_FRAMEBUFFER, id == DEPTH_TEXTURE ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0 + id, texture, 0);
}
void OGLHelper::shoutAny(std::string prefix) {
	GLenum ge = glGetError();
	switch (ge) {
	case GL_INVALID_ENUM:
		printf("%s: Invalid enum!\n", prefix.c_str());
		break;
	case GL_INVALID_VALUE:
		printf("%s: Invalid value!\n", prefix.c_str());
		break;
	case GL_INVALID_OPERATION:
		printf("%s: Invalid operation!\n", prefix.c_str());
		break;
	case GL_STACK_OVERFLOW:
		printf("%s: Stack overflow!\n", prefix.c_str());
		break;
	case GL_OUT_OF_MEMORY:
		printf("%s: Out of memory!\n", prefix.c_str());
		break;
	case GL_TABLE_TOO_LARGE:
		printf("%s: Table to large!\n", prefix.c_str());
		break;
	default:
		break;
	}
}
bool OGLHelper::checkFBO(GLuint fbo){
	GLenum err = GL_FRAMEBUFFER_COMPLETE;
	if ((err = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
		switch (err) {
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			printf("Framebuffer: Incomplete attachment!\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			printf("Framebuffer: Missing attachment!\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			printf("Framebuffer: Incomplete draw buffer!\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			printf("Framebuffer: Incomplete read buffer!\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			printf("Framebuffer: Incomplete dimensions!\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			printf("Framebuffer: Incomplete formats!\n");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			printf("Framebuffer: Not supported!\n");
			break;
		default:
			printf("Framebuffer: Unknown failure!\n");
			break;
		}
		if (err == GL_INVALID_ENUM)printf("Framebuffer: Invalid enum!\n");
		shoutAny("Framebuffer: ");
		return false;
	}
	return true;
}
void OGLHelper::bindFBO(GLuint fbo){
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}
void OGLHelper::bindRBO(GLuint rbo) {
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
}
void OGLHelper::destroyFBO(GLuint fbo){
	glDeleteFramebuffers(1, &fbo);
}
void OGLHelper::destroyTexture(GLuint t){
	glDeleteTextures(1, &t);
}
void OGLHelper::destroyRBO(GLuint rbo){
	glDeleteRenderbuffers(1, &rbo);
}
void OGLHelper::copy(u8 u, u32 w, u32 h) {
	GLbitfield bit = u == 1 ? GL_DEPTH_BITS : (u == 2 ? GL_STENCIL_BITS : GL_COLOR_BUFFER_BIT);
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, bit, GL_NEAREST);
}
#endif