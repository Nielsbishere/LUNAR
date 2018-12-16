#pragma once
#define DEPTH_TEXTURE 420
#include "../Math/MathDefines.h"
#include <string>
class OGLHelper {
public:
	static GLuint createFBO();
	static GLuint createTexture(u32 w, u32 h, bool linearFiltering=false);
	static GLuint createDepthTexture(u32 w, u32 h, bool linearFiltering = false);
	static GLuint createDepth(u32 w, u32 h);
	static void attachDepth(GLuint fbo, GLuint depth);
	//Attach a texture to an FBO. DEPTH_TEXTURE is for attaching depth texture
	static void attachment(GLuint fbo, u32 id, GLuint texture);
	template<u32 l> static void attachments(GLuint fbo, u32 (&attach)[l]) {
		GLenum draw[l];
		for (u32 i = 0; i < l; i++) draw[i] = attach[i] + GL_COLOR_ATTACHMENT0;
		glDrawBuffers(l, draw);
	}
	static bool checkFBO(GLuint fbo);
	static void bindFBO(GLuint fbo);
	static void bindRBO(GLuint rbo);
	static void destroyFBO(GLuint fbo);
	static void destroyTexture(GLuint t);
	static void destroyRBO(GLuint rbo);
	//Where u8 i 1 = depth, 2 = stencil or else color.
	static void copy(u8 i, u32 w, u32 h);

	static void shoutAny(std::string prefix = "");
};