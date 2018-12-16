uniform float factor;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

ShaderOld
	attribute vec3 position;
	attribute vec2 texCoord;
	attribute vec3 normal;

	attribute vec3 nposition;
	attribute vec3 nnormal;

	varying vec2 uv;
	varying vec3 n;
	varying vec3 wpos;
	varying vec3 cpos;
EndOld
ShaderNew
	in layout(location = 0) vec3 position;
	in layout(location = 1) vec2 texCoord;
	in layout(location = 2) vec3 normal;
	in layout(location = 3) vec3 nposition;
	in layout(location = 4) vec3 nnormal;

	out vec2 uv;
	out vec3 n;
	out vec3 wpos;
	out vec3 cpos;
EndNew

void main(){
 vec3 pos = position + factor * (nposition - position);
 vec3 norm = normal + factor * (nnormal - normal);
 wpos = vec3(M * vec4(pos, 1));
 cpos = vec3(V * vec4(wpos, 1));
 gl_Position = P * vec4(cpos, 1);
 uv = texCoord;
 n = normalize(vec3(M * vec4(norm, 0)));
}