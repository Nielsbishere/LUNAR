#pragma once
#include "../Math/MathDefines.h"
#include "../Shapes/AABB.h"
#include "../Shapes/Triangle.h"
namespace osomi{
	class ProceduralMeshFragment {
	public:
		ProceduralMeshFragment(osomi::AABB bo, osomi::Triangle *tri, u32 count): box(bo), triangles(tri), cnt(count){}
		ProceduralMeshFragment() {}
		osomi::Triangle *getTris(unsigned int &i) {
			i = cnt;
			return triangles;
		}
		~ProceduralMeshFragment() {
			if(triangles != nullptr)delete[] triangles;
		}
		osomi::AABB getAABB(){ return box; }
		void setAABB(osomi::AABB bo){box = bo;}
	private:
		osomi::Triangle *triangles;
		u32 cnt;
		osomi::AABB box;
	};
}