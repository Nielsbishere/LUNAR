#include "../../Headers/MarchingCubes/ProceduralMesh.h"
#include "../../Headers/MarchingCubes/MarchingCubes.h"
#include "../../Headers/Helper/StringUtils.h"
#include "../../Headers/Resource/Resources.h"
#include "../../Headers/Render/Light.h"
#include "../../Headers/Noise/SimplexNoise.h"

using namespace osomi;

ProceduralMesh::ProceduralMesh(Texture *_t, std::string name, AABB aabb): RenderObject(name, aabb) {
	t = _t;
}

ProceduralMesh *ProceduralMesh::create(std::string name, Texture *t, Grid3Df *g, float size, int chunkX, int chunkY, int chunkZ) {
	int w, h, l;
	w = g->getWidth();
	h = g->getHeight();
	l = g->getLength();
	if (chunkX == 0 || chunkY == 0 || chunkZ == 0 || w < 2 || l < 2 || h < 2)return nullptr;
	
	ProceduralMesh *c = new ProceduralMesh(t, name, AABB(vec3(), vec3(20 * (chunkX - 1), 20 * (chunkY - 1), 20 * (chunkZ - 1)) * size));
		
	int nchunkW = math::ceil((w - 2) / chunkX);
	int nchunkH = math::ceil((h - 2) / chunkY);
	int nchunkL = math::ceil((l - 2) / chunkZ);
	
	//Push subchunks
	for (int i = 0; i < nchunkW; i++)
		for (int j = 0; j < nchunkH; j++)
			for (int k = 0; k < nchunkL; k++) {
				//(chunkX-1) per chunk -1
				unsigned int sX = i * (chunkX - 1);
				unsigned int sY = j * (chunkY - 1);
				unsigned int sZ = k * (chunkZ - 1);
				unsigned int eX = sX + chunkX + 2;
				unsigned int eY = sY + chunkY + 2;
				unsigned int eZ = sZ + chunkZ + 2;
					
				Grid3Df *gc = g->copy(sX, sY, sZ, eX, eY, eZ);
	
				vec3 siz = vec3(20, 20, 20) * size;
				vec3 tr = vec3(siz.x * i, siz.y * j, siz.z * k);
				ProceduralMeshPart *cp = MarchingCubes::meshFromGrid(StringUtils::numstring(k * nchunkH * nchunkW + j * nchunkW + i, name + "- #", ""), gc, vec3(1, 1, 1), mat4::getModel(tr, vec3(), siz), .2);
				cp->setTranslate(tr);
				cp->setScale(siz);
	
				c->push(cp);
					
				delete gc;
			}
	
	return c;
}
std::vector<ProceduralMeshFragment*> ProceduralMesh::getParts(AABB e) {
	std::vector<ProceduralMeshFragment*> parts;
	for (unsigned int i = 0; i < p.size(); i++)
		if (p[i]->getAABB().intersect(e))
			p[i]->append(parts, e);
	return parts;
}

ProceduralMesh::~ProceduralMesh() {
	for (unsigned int i = 0; i < p.size(); i++)
		delete p[i];
}
template<class T, class L> bool put(std::vector<T*> &objects, std::vector<L> &distances, T *obj, L distance) {
	if (objects.size() != distances.size())return false;
	for (unsigned int ind = 0; ind < objects.size(); ind++) {
		if (distances[ind] > distance) {
			objects.insert(objects.begin() + ind, obj);
			distances.insert(distances.begin() + ind, distance);
			return true;
		}
	}
	objects.push_back(obj);
	distances.push_back(distance);
}

void ProceduralMesh::unbind(Shader *s){
	#ifdef __EGL2__
	GLsizei s0 = s->getLocation("position"), s1 = s->getLocation("normal");
	glDisableVertexAttribArray(s0);
	glDisableVertexAttribArray(s1);
	#else
	glBindVertexArray(0);
	#endif
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void ProceduralMesh::bind(Shader *s){}
void ProceduralMesh::nobindRender(Shader *s, mat4 m, mat4 v, mat4 p, vec3 eye, std::vector<Light> lights, Frustum f) {
	t->bind(GL_TEXTURE_2D);

	s->set("ambient", vec3(.8, .8, .8));
	s->set("V", v);
	s->set("P", p);
	s->set("eye", eye);
	
	s->set("actualLights", (i32)lights.size());
	for (u32 i = 0; i < lights.size(); i++) {
		s->set(StringUtils::numstring(i, "lights[", "].position"), lights[i].getPosition());
		s->set(StringUtils::numstring(i, "lights[", "].color"), lights[i].getColor());
		s->set(StringUtils::numstring(i, "lights[", "].power"), lights[i].getPower());
	}

	#ifdef __EGL2__
	GLsizei s0 = s->getLocation("position"), s1 = s->getLocation("normal");
	#else
	GLsizei s0 = 0, s1 = 0;
	#endif

	std::vector<ProceduralMeshPart*> rend;
	std::vector<float> distances;

	for (unsigned int i = 0; i < this->p.size(); i++) {
		vec3 dis = (this->p[i]->getTranslate() - eye);
		float dist = dis.length();
		if (dist >= f.getFar() + this->p[i]->getScale().x)continue;

		vec3 mi = this->p[i]->getTranslate() - this->p[i]->getScale();
		vec3 ma = this->p[i]->getTranslate() + this->p[i]->getScale();
		
		if (!f.intersection(AABB(mi, ma), 5))continue;
		
		put(rend, distances, this->p[i], dist);
	}
	
	for (unsigned int i = 0; i < rend.size(); i++) {
		s->set("MVP", p * v * rend[i]->getModel());
		rend[i]->render(s, s0, s1);
	}
}

void ProceduralMesh::render(Shader *s, mat4 m, mat4 v, mat4 p, vec3 eye, std::vector<Light> lights, Frustum f){
	nobindRender(s, m, v, p, eye, lights, f);

	unbind(s);
}

ProceduralMesh *ProceduralMesh::create3D(std::string name, Texture *t, u8 chunks, float size, vec3 noiseScale, u8 chunkSize) {
	SimplexNoise *sn = new SimplexNoise();
	
	noiseScale *= size;
	
	Grid3Df *gr = Grid3Df::create(chunks * chunkSize + 2, chunks * chunkSize + 2, chunks * chunkSize + 2);
	
	for (int i = 0; i < chunks * chunkSize + 2; i++)
		for (int j = 0; j < chunks * chunkSize + 2; j++)
			for (int k = 0; k < chunks * chunkSize + 2; k++) 
				gr->at(i, j, k) = sn->noise(i * noiseScale.x, j * noiseScale.y, k * noiseScale.z);
	
	ProceduralMesh *cav = ProceduralMesh::create(name, t, gr, size, chunkSize, chunkSize, chunkSize);
	
	delete gr;
	delete sn;
	
	return cav;
}