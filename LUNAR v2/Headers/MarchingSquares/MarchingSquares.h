#pragma once

#include "../Math/MathDefines.h"
#include "../World/Grid2D.h"
#include <string>
namespace osomi{
	class Mesh;
	struct MSCell{
		vec4 neighbors;
		vec2 normal[4];
		vec2 position[4];
		u8 flag;
	};
	struct MSStrip {
		vec2 n[2];
		vec2 p[2];
	};
	struct MSTriangle{
		vec3 p[3];
		vec3 n[3];
		MSTriangle(vec3 a, vec3 b, vec3 c, vec3 na, vec3 nb, vec3 nc){
			p[0] = a;
			p[1] = b;
			p[2] = c;
			n[0] = na;
			n[1] = nb;
			n[2] = nc;
		}
		MSTriangle(vec3 a, vec3 b, vec3 c, vec2 na, vec2 nb, vec2 nc) {
			p[0] = a;
			p[1] = b;
			p[2] = c;
			n[0] = vec3(na.x, 0, na.y);
			n[1] = vec3(nb.x, 0, nb.y);
			n[2] = vec3(nc.x, 0, nc.y);
		}
		MSTriangle(): MSTriangle(vec3(), vec3(), vec3(), vec3(), vec3(), vec3()){}
	};
	class MarchingSquares {
	public:
		static Mesh *create(std::string name, Grid2Df *g, vec3 scale=vec3(1,1,1), vec3 translate=vec3(), vec3 uvLoop=vec3(10, 10, 10), double isoLevel =.5);
		static Mesh *generate(std::string name, u32 nodesX, u32 sizeY, vec2 noiseScale, vec2 noiseOffset, vec3 meshUvLoop);
	};
}