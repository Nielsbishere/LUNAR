//uniform sampler2D albedoSpecular;
//uniform sampler2D normals;
//uniform sampler2D depth;
//
//uniform mat4 P;
//uniform mat4 V;
//uniform vec3 eye;
//uniform vec3 dir;
//
//uniform float near;
//uniform float far;
//
//uniform vec3 ambient;
//
//ShaderOld
//DEFERRED RENDERING NOT SUPPORTED IN ANCIENT OPENGL
//EndOld
//
//RenderingLighting
//	#define LIGHTS 128
//	uniform vec3 lightPositions[LIGHTS];
//	uniform vec4 lightColPow[LIGHTS];
//	uniform int actualLights;
//EndLighting
//
//in vec2 uv;
//out vec4 color;
//
////Takes in uv coords + depth & returns world coordinates
////Optimizations taken from http://dev-room.blogspot.nl/2015/02/reconstructing-position-vector-from.html
////Optimization: sending inverse view instead of view
//vec3 unproject(vec2 fuv, float fdepth){
//	float zv = -P[3][2] / ((fdepth * 2.0 - 1.0) + P[2][2]);
//
//	vec3 cpos;
//	cpos.x = -zv * (2.0 * fuv.x - 1.0) / P[0][0];
//    cpos.y = -zv * (2.0 * fuv.y - 1.0) / P[1][1];
//    cpos.z = zv;
//
//	return vec3(inverse(V) * vec4(cpos, 1));
//}
//
//void main(){
//	RenderingAlbedo
//		color = vec4(texture(albedoSpecular, uv).rgb, 1);
//	EndAlbedo
//	RenderingLighting
//		vec4 as = texture(albedoSpecular, uv);
//		vec4 nsamp = texture(normals, uv);
//		if(as == vec4(0.0, 0.0, 0.0, 0.0) && nsamp == vec4(0.0, 0.0, 0.0, 0.0))discard;
//		float mods = nsamp.a;		//0 = none, 1 = light, 2 = bloom, 3 = light & bloom
//
//		if((uint(mods) & 1) != 0){
//			//No lighting calculations (Like for a light; which has 100% ambient)
//			color = vec4(as.rgb, 1);
//			return;
//		}
//
//		float dist = texture(depth, uv).r;
//		float distl = (2.0 * near) / (far + near - dist * (far - near));
//		vec3 n = nsamp.xyz * 2.0 - 1.0;
//		vec3 albedo = as.rgb;
//		float ref = as.a;
//
//		vec3 wpos = unproject(uv, dist);
//		
//		vec3 outcolor = vec3(0.0, 0.0, 0.0);
//
//		vec3 computedLight = vec3(0,0,0);
//		int lightAm = actualLights < LIGHTS ? actualLights : LIGHTS;
//		if(lightAm >= 1){
//			int applied = 0;
//			 for(int i = 0; i < lightAm; i++){
//				float id = 1.0 / length(lightPositions[i] - wpos);
//				vec3 ldir = normalize(lightPositions[i] - wpos);
//				float cosTheta = clamp(dot(ldir, n), 0.0, 1.0);
//				float angle = clamp(dot(n, normalize(ldir + dir)), 0.0, 1.0);
//				float reflected = ref * pow(angle, 10.0);
//				vec3 lightfactor = lightColPow[i].xyz * lightColPow[i].w * id;
//				computedLight = computedLight + cosTheta * lightfactor + reflected * lightfactor;
//				applied++;
//			 }
//		}
//		outcolor = (computedLight + ambient) * albedo;
//
//		if((uint(mods) & 2) != 0){
//			//Gaussian blur
//		}
//
//		color = vec4(outcolor, 1);
//	EndLighting
//}

//Fragment shader is applied to the deferred rendering quad
uniform sampler2D t;
in vec2 uv;
out vec4 color;
void main(){
	color = vec4(texture(t, uv).rgb, 1);
}