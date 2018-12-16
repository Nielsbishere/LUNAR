uniform sampler2D t;
uniform sampler2D spec;
uniform vec3 ambient;
uniform vec3 eye;
uniform mat4 MVP;

ShaderOld
	varying vec3 n;
	varying vec3 uv;
	varying vec3 wpos;
	varying vec3 cpos;
EndOld
ShaderNew
	in vec3 n;
	in vec3 uv;
	in vec3 wpos;
	in vec3 cpos;
	RenderingForward
		out layout(location=0) vec4 color;								//RGB
	EndForward
	RenderingDeferred
		out layout(location=0) vec4 color;			//RGB A=specular
		out layout(location=1) vec4 normal;			//normal
	EndDeferred
EndNew

RenderingAlbedo
EndAlbedo
RenderingLighting
	#define LIGHTS 16
	struct Light {
		vec3 position;
		vec3 color;
		float power;
	};
	uniform Light lights[LIGHTS];
	uniform int actualLights;
EndLighting

void main(){
	vec2 spos = uv.xy + vec2(uv.z, 0);
	RenderingForward
		RenderingAlbedo
			//Albedo lighting (No shadows / light)
			ShaderOld
				//Embedded systems EGL2.0 
				  gl_FragColor = vec4(texture2D(t, spos).rgb, 1);
			EndOld
			ShaderNew
				//Embedded systems EGL>=3.0 and modern OGL
				  color = vec4(texture(t, spos).rgb, 1);
			EndNew
		EndAlbedo
		RenderingLighting
			//Lighting; lights
			 vec3 computedLight = vec3(0,0,0);
			 int lightAm = actualLights < LIGHTS ? actualLights : LIGHTS;
			 
			 float ref = .1;
			 if(textureSize( spec, 0).x > 0){
				vec3 col = vec3(.1, 0, 0);
				ShaderOld
				col = texture2D(spec, spos).rgb;
				EndOld
				ShaderNew
				col = texture(spec, spos).rgb;
				EndNew
				ref = col.r;
			 }

			 if(lightAm >= 1){
				 for(int i = 0; i < LIGHTS; i++){
					if(i >= lightAm)break;
					float id = 1.0 / length(lights[i].position - wpos);
					if(lights[i].power * id * id < 1.0 / 256.0)continue;
					vec3 ldir = normalize(lights[i].position - wpos);
					float cosTheta = clamp(dot(ldir, normalize(n)), 0.0, 1.0);
					float reflected = ref * pow(clamp(dot(reflect(-ldir, n), normalize(eye - wpos)), 0.0, 1.0), 10.0);
					computedLight = computedLight + cosTheta * lights[i].color * lights[i].power * id * id + reflected * lights[i].power * lights[i].color * id;
				 }
				 computedLight = computedLight / float(lightAm);
			 }
			ShaderOld
				//Embedded systems EGL2.0 
				vec3 col = texture2D(t, spos).rgb;
				gl_FragColor = vec4(computedLight * col, 1);
			EndOld
			ShaderNew
				//Embedded systems EGL>=3.0 and modern OGL
				vec3 col = texture(t, spos).rgb;
				color = vec4(computedLight * col + ambient * col, 1);
			EndNew
		EndLighting
	EndForward
	RenderingDeferred
		ShaderOld
			ERROR! Without multiple rendering targets, deferred rendering is not possible.
		EndOld
		ShaderNew
			float ref = .1;

			 if(textureSize( spec, 0).x > 0){
				vec3 col = vec3(.1, 0, 0);
				ShaderOld
				col = texture2D(spec, spos).rgb;
				EndOld
				ShaderNew
				col = texture(spec, spos).rgb;
				EndNew
				ref = col.r;
			 }
			vec3 col = texture(t, spos).rgb;
			color = vec4(col, ref);
			normal = vec4(n/2.0+0.5, 0);
		EndNew
	EndDeferred
}