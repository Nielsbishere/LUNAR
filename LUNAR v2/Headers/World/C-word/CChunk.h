#pragma once
#include "../../Shapes/AABB.h"
#include "../../Shapes/Triangle.h"
#include <vector>
#include <iostream>
namespace osomi{
	class GameObject;
	class CChunk {
	private:
		AABB box;
		
		CChunk **childs;
		u32 childCount;
		
		std::vector<Triangle> tris;
	public:
		CChunk(AABB _box, CChunk **_childs = nullptr, u32 _c=0): box(_box), tris(std::vector<Triangle>()), childs(_childs), childCount(_c) {}
		CChunk(AABB _box, std::vector<Triangle> _tris): box(_box), tris(_tris), childs(nullptr), childCount(0) {}
		AABB getAABB(){ return box; }
		CChunk **getChunks(u32 &count){ 
			count = childCount;
			return childs; 
		}
		std::vector<Triangle> getTriangles() { return tris; }
		void push(Triangle tr){ 
			tris.push_back(tr);
		}
		~CChunk(){
			if (childs != nullptr) {
				for (u32 i = 0; i < childCount; i++)delete childs[i];
				delete[] childs;
			}
		}
	};
	struct CPackage{
		bool hasCollision;
		double nearest;
		vec3 intersectionPoint;
		
		double t;
		vec3 norm;
		
		GameObject *closest;
		
		CPackage(): hasCollision(false), nearest(f32_MAX), intersectionPoint(vec3()), t(1), closest(nullptr), norm(vec3()){}
	};
}