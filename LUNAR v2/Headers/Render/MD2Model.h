#pragma once
#include "../Math/Types.h"
#include "../Math/MathDefines.h"

#include <iostream>

#include "AnimationState.h"
#include "RenderObject.h"
namespace osomi {
	class MD2Model : public RenderObject {
		friend class Resources;
	private:
		MD2Model(std::string name, GLuint _vbo, u32 _vertices, u32 _frames, f32 *buf, std::vector<FrameState> _states, AABB aabb): RenderObject(name, aabb), vbo(_vbo), vertices(_vertices), frames(_frames), buffer(buf), states(_states), tcurrent(nullptr), tanimationTime(0) {}
		
		u32 vertices, frames;
		f32 *buffer;
		std::vector<FrameState> states;
		
		GLuint vbo;
		#ifndef __EGL2__
		GLuint vao;
		#endif
		
		//Temporary
		FrameState *tcurrent;
		float tanimationTime;
		
	protected:
		static MD2Model *create(std::string name, float *buf, u32 verts, u32 frames, std::vector<FrameState> states, AABB aabb);
		~MD2Model();
		void nobindRender(osomi::Shader *s, osomi::mat4 m, osomi::mat4 v, osomi::mat4 p, osomi::vec3 eye, std::vector<osomi::Light> light, Frustum f) override;
		void unbind(Shader *s) override;
		void bind(Shader *s) override;
	public:
		void render(osomi::Shader *s, osomi::mat4 m, osomi::mat4 v, osomi::mat4 p, osomi::vec3 eye, std::vector<osomi::Light> light, Frustum f) override;
		FrameState *getAnimationState(std::string name);
		void setTemporary(FrameState *current, float animationTime);
	};
}