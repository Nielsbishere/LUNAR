ShaderOld
//Embedded systems EGL2.0 
attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;
varying vec2 uv;
varying vec3 n;
varying vec3 wpos;
varying vec3 cpos;
EndOld

ShaderNew
//Embedded systems EGL>=3.0 and modern OGL
in layout(location = 0) vec3 position;
in layout(location = 1) vec2 texCoord;
in layout(location = 2) vec3 normal;
out vec2 uv;
out vec3 n;
out vec3 wpos;
out vec3 cpos;
EndNew

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec2 uvScale;
void main() {
	wpos = vec3(M * vec4(position, 1));
	cpos = vec3(V * vec4(wpos, 1));
	gl_Position = P * vec4(cpos, 1);
	uv = vec2(texCoord.x, 1.0 - texCoord.y) * uvScale;
	n = normalize(vec3(M * vec4(normalize(normal), 0)));
}