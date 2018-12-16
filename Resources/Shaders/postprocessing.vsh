ShaderOld
DEFERRED RENDERING NOT SUPPORTED IN ANCIENT OPENGL
EndOld
in layout(location = 0) vec2 position;
out vec2 uv;

void main() {
	gl_Position = vec4(position * 2.0 - 1.0, 0.0, 1.0);
	uv = vec2(position.x, position.y);
}