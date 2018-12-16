#include "../../Headers/World/Camera.h"
#include "../../Headers/Input/InputHandler.h"
using namespace osomi;

Camera::Camera(vec3 _eye, vec3 _up, float _n, float _f, float _aspect, float _fov): eye(_eye), up(_up), n(_n), f(_f), aspect(_aspect), fov(_fov), pitch(0), yaw(90) {
	setDir(yaw, pitch);
}
		
void Camera::setDir(float ya, float pi){
	this->pitch = pi;
	this->yaw = ya;
	
	float yaw = this->yaw / 180 * M_PI;
	float pitch = this->pitch / 180 * M_PI;
	float xzLen = cos(pitch);
	float x = xzLen * cos(yaw);
	float y = sin(pitch);
	float z = xzLen * sin(-yaw);
	vec3 d = vec3(x, y, z).normalize();
	
	center = eye + d;
}
void Camera::move(vec3 position){
	eye = position;
	setDir(yaw, pitch);
}
void Camera::updateAspect(float asp){
	aspect = asp;
}
		
Frustum Camera::toFrustum(){
	return Frustum(fov, aspect, f, n, eye, center, up);	
}
		
Camera *Camera::create(vec3 eye, float fov, float aspect, vec3 up, float n, float f){
	return new Camera(eye, up, n, f, aspect, fov);
}


mat4 Camera::getProjection(){
	return mat4::perspective(fov, aspect, n, f);
}
mat4 Camera::getView(){
	return mat4::lookat(eye, center, up);
}

void Camera::applyMovement(vec3 movement){
	move(eye + movement);
}
void Camera::rotate(float dy, float dp, bool clamp){
	if (clamp){
		if (pitch + dp <= -60){
			dp = 0;
			pitch = -60;
		}
		if (pitch + dp >= 60) {
			dp = 0;
			pitch = 60;
		}
	}
	setDir(yaw + dy, pitch + dp);
}

void Camera::relativeMovement(vec3 rm){
	mat4 v = getView();
	
	vec3 fd = v.getForward().normalize();
	vec3 rd = v.getRight().normalize();
	vec3 ud = v.getUp().normalize();

	vec3 mov = fd * -rm.z + rd * rm.x + ud * rm.y;
	
	applyMovement(mov);
}
void Camera::update(InputHandler *ih, u8 forward, u8 backward, u8 right, u8 left, u8 up, u8 down, float xspeed, float yspeed, float moveSpeedX, float moveSpeedY, float moveSpeedZ, float delta){
	if (ih->isKeyDown(forward)) relativeMovement(vec3(0, 0, 1) * delta * moveSpeedZ);
	if (ih->isKeyDown(backward)) relativeMovement(vec3(0, 0, -1) * delta * moveSpeedZ);
	if (ih->isKeyDown(left)) relativeMovement(vec3(-1, 0, 0) * delta * moveSpeedX);
	if (ih->isKeyDown(right)) relativeMovement(vec3(1, 0, 0) * delta * moveSpeedX);
	if (ih->isKeyDown(up)) relativeMovement(vec3(0, 1, 0) * delta * moveSpeedY);
	if (ih->isKeyDown(down)) relativeMovement(vec3(0, -1, 0) * delta * moveSpeedY);
	
	float x, y;
	ih->getDelta(x, y);
	rotate(xspeed * -x * delta, yspeed * -y * delta, true);
}