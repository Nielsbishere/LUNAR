ShaderOld
//Embedded systems EGL2.0 
attribute vec3 position;
varying vec2 uv;
EndOld

ShaderNew
//Embedded systems EGL>=3.0 and modern OGL
in layout(location = 0) vec3 position;
out vec2 uv;
EndNew

uniform mat4 MVP;
uniform vec4 uvBounds;
void main() {
	gl_Position = MVP * vec4(position, 1);
	uv = position.xy * 0.5 + 0.5;
	uv = vec2(uv.x, 1.0 - uv.y);
	uv = (uvBounds.zw - uvBounds.xy) * uv + uvBounds.xy;
}