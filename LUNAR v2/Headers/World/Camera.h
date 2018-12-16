#pragma once
#include "../Math/MathDefines.h"
#include "../Shapes/Frustum.h"
namespace osomi{
	class InputHandler;
	class Camera{
	private:
		float n, f, aspect, fov;
		vec3 eye, center, up;
		
		float pitch, yaw;
		
		Camera(vec3 eye, vec3 up, float n, float f, float aspect, float fov);
	public:
		float getNear(){ return n; }
		float getFar(){ return f; }
		float getAspect(){ return aspect; }
		float getFOV(){ return fov; }
		float getPitch(){ return pitch; }
		float getYaw(){ return yaw; }
		vec3 getPosition(){ return eye; }
		vec3 getCenter(){ return center; }
		vec3 getDirection(){ return (center - eye).normalize(); }
		vec3 getUp(){ return up; }
		
		void setDir(float yaw, float pitch);
		void move(vec3 position);
		void applyMovement(vec3 movement);
		void rotate(float dp, float dy, bool clamp=false);
		void updateAspect(float aspect);
		
		void update(InputHandler *ih, u8 forward, u8 backward, u8 right, u8 left, u8 up, u8 down, float xspeed, float yspeed, float moveX, float moveY, float moveZ, float delta);
		
		void relativeMovement(vec3 rmovement);
		
		Frustum toFrustum();
		mat4 getProjection();
		mat4 getView();
		
		static Camera *create(vec3 eye, float fov=45.f, float aspect = 16 / 9.f, vec3 up = vec3(0, 1, 0), float n =.1f, float f = 100.f);
	};
}