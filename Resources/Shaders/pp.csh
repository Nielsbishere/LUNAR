#version 430
#extension GL_ARB_shader_storage_buffer_object : enable

#define MAX_WORK_GROUP_SIZE 32
#define MAX_LIGHTS_PER_QUAD 48

//Inputs
uniform sampler2D albedoSpecular;
uniform sampler2D normals;
uniform sampler2D depth;

//Output
layout(rgba8) writeonly uniform image2D outCol;

//Variables
uniform mat4 V;
uniform mat4 P;

uniform vec4 eyeNear;
uniform vec4 dirFar;
uniform vec3 ambient;

//Lights
struct PointLight { 
	vec4 posRadius;
	vec4 colPow;
};
layout(std430, binding = 3) buffer PointLights {
	PointLight lights[];
};
uniform int actualLights;

//Size of screen quads
layout (local_size_x = MAX_WORK_GROUP_SIZE, local_size_y = MAX_WORK_GROUP_SIZE, local_size_z = 1) in;

//Per screen-quad variables
shared uint minDepth;
shared uint maxDepth;

shared uint lightCount;
shared uint lightIndices[MAX_LIGHTS_PER_QUAD];

//Takes in uv coords + depth [0, 1] & returns world coordinates
//Optimizations taken from http://dev-room.blogspot.nl/2015/02/reconstructing-position-vector-from.html
//Optimization: sending inverse view instead of view
vec3 unproject(vec2 fuv, float fdepth){
	float zv = -P[3][2] / ((fdepth * 2.0 - 1.0) + P[2][2]);

	vec3 cpos;
	cpos.x = -zv * (2.0 * fuv.x - 1.0) / P[0][0];
    cpos.y = -zv * (2.0 * fuv.y - 1.0) / P[1][1];
    cpos.z = zv;

	return vec3(inverse(V) * vec4(cpos, 1));
}

uint toUint(float f) {
	return uint(f * 0xffffffffu);
}
float toFloat(uint i) {
	return float(i) / float(0xffffffffu);
}

mat4 invProj;
//Unproject NDC to view coords
vec3 toView(vec4 ndc) {
	vec4 res = invProj * ndc;
	res /= res.w;
	return res.xyz;
}

//Construct a plane from 2 frustum corners
vec3 genPlane(vec3 a, vec3 b) {
	return cross(a, b);
}
//Get the distance from a plane to a point (requires plane and position to be in view space)
float signedDistance(vec3 plane, vec3 vpos) {
	return dot(plane, vpos);
}
void main() {
	//Get pixel position and resolution of image/screen (as image:screen = 1:1)
    ivec2 res = imageSize(outCol);
    ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);

	float dist;
	vec2 uv;

	if (storePos.x < res.x && storePos.y < res.y) {
		uv = vec2(storePos) / (vec2(res) - vec2(1, 1));
		dist = texture(depth, uv).r;

		//Calculate a min and max depth from all fragments in this quad
		uint depthInt = toUint(dist);
		atomicMin(minDepth, depthInt);
		atomicMax(maxDepth, depthInt);
	}
	lightCount = 0;
	barrier();	//Ensure that all fragments' depth are calculated

	//Get actual floating point values instead of uint
	float minDepthf = toFloat(minDepth);
	float maxDepthf = toFloat(maxDepth);

	//Get the pixel start and end
	uint minX = MAX_WORK_GROUP_SIZE * gl_WorkGroupID.x;
	uint minY = MAX_WORK_GROUP_SIZE * gl_WorkGroupID.y;
	uint maxX = min(MAX_WORK_GROUP_SIZE * (gl_WorkGroupID.x + 1), res.x);
	uint maxY = min(MAX_WORK_GROUP_SIZE * (gl_WorkGroupID.y + 1), res.y);

	//Save the inverse projection matrix (minor optimization)
	invProj = inverse(P);
	mat4 invView = inverse(V);

	//Convert these corners into NDC and then convert them to view space
	vec3 tileCorners[4];
	tileCorners[0] = toView(vec4((float(minX) / res.x) * 2.0f - 1.0f, (float(minY) / res.y) * 2.0f - 1.0f, 1.0f, 1.0f));
	tileCorners[1] = toView(vec4((float(maxX) / res.x) * 2.0f - 1.0f, (float(minY) / res.y) * 2.0f - 1.0f, 1.0f, 1.0f));
	tileCorners[2] = toView(vec4((float(maxX) / res.x) * 2.0f - 1.0f, (float(maxY) / res.y) * 2.0f - 1.0f, 1.0f, 1.0f));
	tileCorners[3] = toView(vec4((float(minX) / res.x) * 2.0f - 1.0f, (float(maxY) / res.y) * 2.0f - 1.0f, 1.0f, 1.0f));

	//Get the plane vectors
	vec3 frustum[4];
	frustum[0] = genPlane(tileCorners[0], tileCorners[1]);	//bottom
	frustum[1] = genPlane(tileCorners[1], tileCorners[2]);	//right
	frustum[2] = genPlane(tileCorners[2], tileCorners[3]);	//top
	frustum[3] = genPlane(tileCorners[3], tileCorners[0]);	//left

	//Efficient light frustum culling; (Example 16x16 pixel workgroup)
	//Every pixel invokes a frustum cull, so for maximum efficiency:
	//Every pixel calculates id + workgroupLength * x = y (where y < lights)
	//This means that for 1024 lights, the following would happen:
	//(0, 0) would check lights 0, 256, 512, 768
	//This means that every pixel performs 4 frustum culls instead of 1024.
	//Afterwards; we need to synchronize all of the threads so we can use the information we generated
	uint workgroupLength = MAX_WORK_GROUP_SIZE * MAX_WORK_GROUP_SIZE;
	for (uint i = 0; i < actualLights; i += workgroupLength) {
		uint j = gl_LocalInvocationIndex + i;
		if (j < actualLights) {
			//Get light and view position of light and radius
			PointLight l = lights[j];
			vec3 vpos = vec3(V * vec4(l.posRadius.xyz, 1.0f));
			float r = l.posRadius.w;

			//Oclussion culling
			if (vpos.z + minDepthf < r && vpos.z - maxDepthf < r) {
				//Frustum culling
				if (signedDistance(frustum[0], vpos) < r && signedDistance(frustum[1], vpos) < r && signedDistance(frustum[2], vpos) < r && signedDistance(frustum[3], vpos) < r) {
					uint id = atomicAdd(lightCount, 1);
					if(id < MAX_LIGHTS_PER_QUAD) lightIndices[id] = j;
				}
			}
		}
	}
	barrier();				//Ensure that all the lights have been culled

	//Render fragment
    if (storePos.x < res.x && storePos.y < res.y) {
		if (lightCount == 0) {
			imageStore(outCol, storePos, vec4(ambient * texture(albedoSpecular, uv).xyz, 1));
			return;
		}
		//float lightPerc = float(lightCount) / float(MAX_LIGHTS_PER_QUAD);
		//imageStore(outCol, storePos, vec4(lightPerc, lightPerc, lightPerc, 1));
		//return;

		//Get all variables
		vec4 as = texture(albedoSpecular, uv);
		vec4 nm = texture(normals, uv);

		if (as != vec4(0, 0, 0, 0)) {
			//Unpack the variables
			vec3 albedo = as.rgb;
			float ref = as.a;
			vec3 n = nm.rgb;
			float mods = nm.a;

			//Exception for light sources (so they seem fully lit)
			if ((uint(mods) & 1) != 0) {
				//No lighting calculations (Like for a light; which has 100% ambient)
				imageStore(outCol, storePos, vec4(albedo, 1));
				return;
			}

			//Calculate world position
			vec3 wpos = unproject(uv, dist);

			//Per Light calculations
			vec3 outColor = ambient;
			int applied = 0;
			for(int i = 0; i < lightCount; i++){
				PointLight l = lights[lightIndices[i]];
				float d = length(l.posRadius.xyz - wpos);
				if (d > l.posRadius.w)continue;
				float id = 1.0 / d;
				vec3 ldir = normalize(l.posRadius.xyz - wpos);
				float cosTheta = clamp(dot(ldir, n), 0.0, 1.0);
				float angle = clamp(dot(n, normalize(ldir + dirFar.xyz)), 0.0, 1.0);
				float reflected = ref * pow(angle, 10.0);
				vec3 lightfactor = l.colPow.xyz * l.colPow.w * id * id;
				outColor += cosTheta * lightfactor + reflected * lightfactor;
				applied++;
			}
			//Save output color
			imageStore(outCol, storePos, vec4(outColor * albedo, 1));
		}
		else imageStore(outCol, storePos, vec4(0, 0, 0, 0)); //Clear undefined pixels
    }
}