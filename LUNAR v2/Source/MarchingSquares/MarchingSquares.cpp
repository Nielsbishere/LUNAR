#include "../../Headers/MarchingSquares/MarchingSquares.h"
#include "../../Headers/Render/Mesh.h"
#include "../../Headers/Shapes/Triangle.h"
#include "../../Headers/Noise/SimplexNoise.h"
#include "../../Headers/World/Grid2D.h"

using namespace osomi;

//C++ 3D OpenGL MarchingSquares algorithm by Niels Brunekreef
//With walls and top.
//Using interpolation, normals and uv mapping
//Inspiration: http://paulbourke.net/geometry/polygonise/

//LUT for single layer marching squares (no walls
//int tri[16][12] = {
//	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
//	{4, 0, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1},
//	{1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
//	{5, 1, 3, 3, 4, 5, -1, -1, -1, -1, -1, -1},
//	
//	{1, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},
//	{4, 2, 3, 4, 6, 2, 4, 1, 6, 4, 0, 1},
//	{5, 6, 2, 2, 0, 5, -1, -1, -1, -1, -1, -1},
//	{3, 4, 2, 4, 5, 2, 5, 6, 2, -1, -1, -1},
//	
//	{3, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1},
//	{0, 2, 7, 7, 4, 0, -1, -1, -1, -1, -1, -1},
//	{1, 2, 7, 5, 1, 7, 7, 5, 3, 3, 5, 0},
//	{1, 2, 7, 5, 1, 7, 4, 5, 7, -1, -1, -1},
//	
//	{1, 6, 7, 7, 3, 1, -1, -1, -1, -1, -1, -1},
//	{4, 0, 7, 0, 6, 7, 0, 1, 6, -1, -1, -1},
//	{3, 0, 7, 0, 6, 7, 0, 5, 6, -1, -1, -1},
//	{5, 6, 7, 7, 4, 5, -1, -1, -1, -1, -1, -1},
//};

//LUT for 3D mesh from marching squares (walls)
int tri[16][24] = {
	//positions
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 0, 3, 0, 8, 11, 11, 3, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 5, 1, 9, 8, 0, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{5, 1, 3, 3, 4, 5, 9, 11, 3, 3, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	
	{1, 6, 2, 10, 9, 1, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 2, 3, 4, 6, 2, 4, 1, 6, 4, 0, 1, 8, 9, 1, 1, 0, 8, 10, 11, 3, 3, 2, 10},
	{5, 6, 2, 2, 0, 5, 10, 8, 0, 0, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 4, 2, 4, 5, 2, 5, 6, 2, 10, 11, 3, 3, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	
	{3, 2, 7, 11, 10, 2, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 2, 7, 7, 4, 0, 8, 10, 2, 2, 0, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 7, 5, 1, 7, 3, 5, 7, 0, 5, 3, 11, 8, 0, 0, 3, 11, 9, 10, 2, 2, 1, 9},
	{1, 2, 7, 5, 1, 7, 4, 5, 7, 9, 10, 2, 2, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	
	{1, 6, 7, 7, 3, 1, 11, 9, 1, 1, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 0, 7, 0, 6, 7, 0, 1, 6, 8, 9, 1, 1, 0, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 7, 0, 6, 7, 0, 5, 6, 11, 8, 0, 0, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{5, 6, 7, 7, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

vec2 norm(Grid2Df *g, u32 i, u32 j){
	if (i < 1 || j < 1 || i >= g->getWidth() - 1 || j >= g->getHeight() - 1)return vec2();
	vec2 delta = vec2();
	delta.x = g->at(i + 1, j) - g->at(i - 1, j);
	delta.y = g->at(i, j + 1) - g->at(i, j - 1);
	return delta.normalize();
}
//vec2 interp(double isolevel, vec2 p1, vec2 p2, double valp1, double valp2) {
//	double mu;
//	vec3 p;
//
//	if (math::absf(isolevel - valp1) < 0.00001)
//		return p1;
//	if (math::absf(isolevel - valp2) < 0.00001)
//		return p2;
//	if (math::absf(valp1 - valp2) < 0.00001)
//		return p1;
//	mu = math::clamp((isolevel - valp1) / (valp2 - valp1), 0.0, 1.0);
//	p.x = p1.x + mu * (p2.x - p1.x);
//	p.y = p1.y + mu * (p2.y - p1.y);
//
//	return p;
//}

vec3 gridSpace2MSSpace(float i, float j, u32 wi, u32 he){
	if (wi <= 2 || he <= 2)return vec3();
	float iperc = (i - 1) / (wi - 2);
	float jperc = (j - 1) / (he - 2);
	return vec3(iperc, 1, jperc);
}

vec3 interp(vec3 p0, vec3 p1, float v0, float v1, float isolvl){
	if (v0 > 1 || v0 < 0 || v1 > 1 || v1 < 0)return vec3();
	bool b0 = v0 > isolvl;
	bool b1 = v1 > isolvl;
	if (b0 != b1){
		float big = v0 > v1 ? v0 : v1;
		float small = v0 < v1 ? v1 : v0;
		float perc = 1 - (big + small) / 2;
		vec3 bp = big == v0 ? p0 : p1;
		vec3 sp = small == v0 ? p0 : p1;
		return bp * perc + sp * (1 - perc);
	}
	return vec3();
}

//Splits the AABB into sub chunks; as long as the width >= subX && height >= subY
CChunk *subdivide(AABB start, u32 x, u32 y, u32 subX, u32 subY, u32 &total){
	if (x < subX || y < subY)return nullptr;
	std::vector<CChunk*> divisions;
	for (u32 i = 1; i <= 2; i++)
		for (u32 j = 1; j <= 2; j++){
			AABB New = AABB(start.getMin() + start.getSize() * vec3(i - 1, 0, j - 1), start.getMin() + start.getSize().scale(vec3(i, 2, j)));
			CChunk *c = subdivide(New, x / 2, y / 2, subX, subY, total);
			if (c == nullptr)continue;
			divisions.push_back(c);
		}
	CChunk *self;
	if (divisions.size() != 0){
		CChunk **divarr = new CChunk*[divisions.size()];
		for (u32 i = 0; i < divisions.size(); i++)
			divarr[i] = divisions[i];
		self = new CChunk(start, divarr, divisions.size());
	}else
		self = new CChunk(start, nullptr, 0);
	total++;
	return self;
}

bool intersect(AABB box, MSTriangle tri, vec3 sc, vec3 tr){
	vec3 a = tri.p[0].scale(sc) - sc / 2 + tr;
	vec3 b = tri.p[1].scale(sc) - sc / 2 + tr;
	vec3 c = tri.p[2].scale(sc) - sc / 2 + tr;
	vec3 p0 = vec3(a.x, 0, a.z);
	vec3 p1 = vec3(b.x, 0, b.z);
	vec3 p2 = vec3(c.x, 0, c.z);
	return box.intersect(p0) && box.intersect(p1) && box.intersect(p2);
}

//Puts a triangle into a chunk
void putInChunk(CChunk *c, MSTriangle *tr, u32 count, vec3 sc, vec3 t){
	std::vector<MSTriangle> inChunk;
	for (u32 i = 0; i < count; i++){
		if (!intersect(c->getAABB(), tr[i], sc, t))continue;
		inChunk.push_back(tr[i]);
	}
	
	u32 cnt;
	CChunk **chunks = c->getChunks(cnt);
	if (chunks == nullptr){
		for (u32 i = 0; i < inChunk.size(); i++){
			MSTriangle triangle = inChunk[i];
			c->push(Triangle(triangle.p[0].scale(sc) - sc / 2 + t, triangle.p[1].scale(sc) - sc / 2 + t, triangle.p[2].scale(sc) - sc / 2 + t));
		}
		return;
	}
	for (u32 j = 0; j < inChunk.size(); j++) {
		MSTriangle triangle = inChunk[j];
		bool contains = false;
		for (u32 i = 0; i < cnt; i++) {	
			CChunk *sub = chunks[i];
			if (sub == nullptr)continue;
			if (intersect(sub->getAABB(), triangle, sc, t)){
				putInChunk(sub, &triangle, 1, sc, t);
				contains = true;
				break;
			}
		}
		if (!contains) 
			c->push(Triangle(triangle.p[0].scale(sc) - sc / 2 + t, triangle.p[1].scale(sc) - sc / 2 + t, triangle.p[2].scale(sc) - sc / 2 + t));
	}
}

void traverse(CChunk *chunks){
	if (chunks == nullptr)return;
	u32 cnt = 0;
	CChunk **c = chunks->getChunks(cnt);
	for (u32 i = 0; i < cnt && c != nullptr; i++){
		if (c[i] == nullptr)continue;
		traverse(c[i]);
	}
	printf("Chunk %p has %u childs (%p) and is located at ", chunks, cnt, c);
	chunks->getAABB().print();
	std::vector<Triangle> trvec = chunks->getTriangles();
	for (u32 i = 0; i < trvec.size(); i++){
		(trvec[i] * mat4::getModel(vec3(0, 12, 0), vec3(), vec3(50, 4, 50))).print();
	}
}

Mesh *MarchingSquares::create(std::string name, Grid2Df *g, vec3 sc, vec3 tr, vec3 uvl, double isoLevel) {
	if (g->getWidth() <= 2 || g->getHeight() <= 2) {
		printf("Couldn't create MS mesh; Grid size <= 2\n");
		return nullptr;
	}
	if (!math::base2(g->getWidth()-2) || !math::base2(g->getHeight()-2)) {
		printf("Grid size invalid! Should be x+2,y+2, where x,y are both base2\n");
		return nullptr;
	}
	sc *= 2;
	vec2 scale = vec2(sc.x, sc.z);
	vec2 translation = vec2(tr.x, tr.z);
	f32 yscale = sc.y;
	f32 ytranslate = tr.y;
	
	std::vector<MSTriangle*> alltris((g->getHeight() - 2)*(g->getWidth() - 2));
	std::vector<u32> count((g->getHeight() - 2)*(g->getWidth() - 2));
	u32 allTriangles = 0;
	u32 troff = 0;
	
	u32 chunkTotal = 0;
	CChunk *chunks = subdivide(AABB(tr - sc, tr + sc), g->getWidth() - 2, g->getHeight() - 2, 1, 1, chunkTotal);
	
	for (u32 i = 1; i < g->getWidth() - 1; i++)
		for (u32 j = 1; j < g->getHeight() - 1; j++) {
			MSCell cell = MSCell();
			
			u32 trcount = 0;
			
			float vals[4];
			vals[0] = g->at(i, j);
			vals[1] = g->at(i + 1, j);
			vals[2] = g->at(i + 1, j + 1);
			vals[3] = g->at(i, j + 1);
			
			u32 flag = (vals[0] >= isoLevel ? 0b1000 : 0b0) | (vals[1] >= isoLevel ? 0b100 : 0b0) | (vals[2] >= isoLevel ? 0b10 : 0b0) | (vals[3] >= isoLevel ? 0b1 : 0b0);
			
			if (flag == 0)continue;
			
			vec3 positions[16];
			positions[4] = gridSpace2MSSpace(i, j + 1, g->getWidth(), g->getHeight());
			positions[5] = gridSpace2MSSpace(i + 1, j + 1, g->getWidth(), g->getHeight());
			positions[6] = gridSpace2MSSpace(i + 1, j, g->getWidth(), g->getHeight());
			positions[7] = gridSpace2MSSpace(i, j, g->getWidth(), g->getHeight());
			positions[0] = positions[4] * 0.5 + positions[5] * 0.5;
			positions[1] = positions[5] * 0.5 + positions[6] * 0.5;
			positions[2] = positions[6] * 0.5 + positions[7] * 0.5;
			positions[3] = positions[7] * 0.5 + positions[4] * 0.5;
			
			for (u32 p2 = 0; p2 < 8; p2++)
				positions[p2 + 8] = positions[p2] - vec3(0, 2, 0);
			
			for (u32 k = 0; k < 23 && tri[flag][k] != -1; k += 3)
				trcount++;
			
			if (trcount == 0) continue;
			
			MSTriangle *tris = new MSTriangle[trcount];
			for (u32 k = 0; k < trcount; k++)
				tris[k] = MSTriangle(positions[tri[flag][k * 3]], positions[tri[flag][k * 3 + 1]], positions[tri[flag][k * 3 + 2]], vec3(), vec3(), vec3());
			
			putInChunk(chunks, tris, trcount, sc, tr);
			
			alltris[troff] = tris;
			count[troff] = trcount;
			troff++;
			allTriangles += trcount;
		}
	
//	traverse(chunks);
	
	//Normals and vertices
	std::vector<GLuint> indices(allTriangles * 3);

	//These are 99% of the time way too big; but resizing vectors is more intense than allocating a block.
	std::vector<vec3> points(allTriangles * 3);
	std::vector<vec3> norms(allTriangles * 3);

	unsigned int vertices = 0, trioff = 0;

	for (unsigned int i = 0; i < troff; i++)
		for (unsigned int j = 0; j < count[i]; j++) {
			MSTriangle t = alltris[i][j];
			vec3 norm = (t.p[1] - t.p[0]).cross(t.p[2] - t.p[0]);
			for (unsigned int k = 0; k < 3; k++) {
				vec3 p = t.p[k].scale(sc) - sc / 2 + tr;	//Convert from MS space to model space
				bool contains = false;
				GLuint ind = 0;
				for (unsigned int l = 0; l < vertices; l++)
					if (points[l] == p) {
						contains = true;
						ind = l;
						break;
					}
				if (contains) {
					indices[trioff * 3 + k] = ind;
					norms[ind] += norm;
				}else {
					points[vertices] = p;
					norms[vertices] = norm;
					indices[trioff * 3 + k] = vertices;
					vertices++;
				}
			}
			trioff++;
		}
	//Generate vertex data
	GLfloat *triangle = new GLfloat[vertices * 8];
	for (int i = 0; i < vertices; i++) {
		*(vec3*)(&triangle[i * 8]) = points[i];
		*(vec2*)(&triangle[i * 8 + 3]) = vec2(uvl.x * points[i].x, uvl.y * points[i].y);
		*(vec3*)(&triangle[i * 8 + 5]) = vec3(norms[i].x, uvl.z * points[i].z, norms[i].z);
	}

	//Generate indices
	u32 indexPtrLength = vertices <= 256 ? 1 : (vertices <= 65536 ? 2 : 4);
	
	u8 *in = new u8[indices.size() * indexPtrLength];
	for (unsigned int i = 0; i < indices.size(); i++){
		if (indexPtrLength == 1)
			in[i] = indices[i];
		else if (indexPtrLength == 2)
			*(u16*)&in[2*i] = indices[i];
		else if (indexPtrLength == 4)
			*(u32*)&in[4*i] = indices[i];
	}
	
	for (unsigned int i = 0; i < troff; i++) 
		delete[] alltris[i];

	printf("Creating model with %u vertices and %u tris!\n", vertices, indices.size() / 3);
	
	Mesh *m = Mesh::create(name, triangle, 8 * vertices * sizeof(GLfloat), in, indexPtrLength, indices.size() * indexPtrLength);
	
	m->setChunk(chunks);
	
	return m;
}

Mesh *MarchingSquares::generate(std::string name, u32 nodesX, u32 nodesY, vec2 noiseScale, vec2 noiseOffset, vec3 uvLoop) {
	SimplexNoise *sn = new SimplexNoise();
	Grid2Df *gr = Grid2Df::create(nodesX+2, nodesY+2);
	
	for (int i = 0; i < nodesX + 2; i++) {
		for (int k = 0; k < nodesY + 2; k++) {
			gr->at(i, k) = (sn->noise(i * noiseScale.x + noiseOffset.x, k * noiseScale.y + noiseOffset.y) + 1) / 2;
			printf("%c", gr->at(i, k) > .5 ? '#' : ' ');
		}
		printf("\n");
	}
	
	Mesh *cav = MarchingSquares::create(name, gr, vec3(1,1,1), vec3(), uvLoop, .5);
	
	delete gr;
	delete sn;
	
	return cav;
}