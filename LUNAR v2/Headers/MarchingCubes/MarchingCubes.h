#pragma once
#include "../Math/MathDefines.h"
#include "../World/Grid3D.h"
#include "Render/ProceduralMeshPart.h"
#include <vector>
#include <string>

namespace osomi {
	struct MCTriangle {
		osomi::vec3 p[3];
		osomi::vec3 n[3];
	};

	struct MCCell {
		osomi::vec3 p[8];
		osomi::vec3 n[8];
		float val[8];
	};
	class MarchingCubes {
	private:
		static osomi::vec3 vertexInterp(double isolevel, osomi::vec3 p1, osomi::vec3 p2, double valp1, double valp2);
		static MCTriangle *polygonise(MCCell grid, double isolevel, unsigned int &t);
		static osomi::vec3 norm(Grid3Df *g, int i, int j, int k);
	public:
		static ProceduralMeshPart *meshFromGrid(std::string name, Grid3Df *g, osomi::vec3 size, osomi::mat4 m, double isolevel = .5, u32 subdivisions = 4);
	};
}