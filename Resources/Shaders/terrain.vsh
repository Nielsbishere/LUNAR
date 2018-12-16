ShaderOld
//Embedded systems EGL2.0 
attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;
varying vec3 n;
varying vec3 uv;
varying vec3 wpos;
varying vec3 cpos;
EndOld

ShaderNew
//Embedded systems EGL>=3.0 and modern OGL
in layout(location = 0) vec3 position;
in layout(location = 1) vec2 texCoord;
in layout(location = 2) vec3 normal;
out vec3 n;
out vec3 uv;
out vec3 wpos;
out vec3 cpos;
EndNew

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
void main() {
	wpos = vec3(M * vec4(position, 1));
	cpos = vec3(V * vec4(wpos, 1));
	gl_Position = P * vec4(cpos, 1);
	if(normal.x == 0.0 && normal.z == 0.0){
		n = vec3(0, 1, 0);
		uv = vec3(texCoord.x, normal.y, 0);
	}
	else{
		n = normalize(vec3(M * vec4(normalize(vec3(normal.x, 0, normal.z)), 0)));
		uv = vec3(texCoord.x, texCoord.y, normal.y);
	}
}