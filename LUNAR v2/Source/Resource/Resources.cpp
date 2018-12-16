#include "../../Headers/Resource/Resources.h"
#include "../../Headers/Helper/StringUtils.h"
#include "../../Headers/Helper/BinaryHelper.h"
#include "../../Headers/API/stbi/load.h"

#include "../../Headers/Render/Mesh.h"
#include "../../Headers/Render/MD2Model.h"
#include "../../Headers/Render/TexturedMesh.h"
#include "../../Headers/Render/Shader.h"
#include "../../Headers/Render/ComputeShader.h"
#include "../../Headers/API/tinyobjloader/tiny_obj_loader.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>

using namespace osomi;
using namespace std;

vector<Texture*> Resources::textures = vector<Texture*>();
vector<RenderObject*> Resources::models = vector<RenderObject*>();
vector<Shader*> Resources::shaders = vector<Shader*>();

Texture *Resources::loadTexture(string path, string name) {
	for (unsigned int i = 0; i < textures.size(); i++)
		if (textures[i]->getName() == name) 
			return textures[i];
	
	int width, height, comp;
	unsigned char *data = stbi_load((StringUtils::combine(StringUtils::split(path, '\\'), "/")).c_str(), &width, &height, &comp, 4);

	if (data == nullptr) {
		printf("Couldn't load a texture with name %s\n", name.c_str());
		return nullptr;
	}

	GLuint textureId;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	#ifdef __EGL2__
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	#else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA_DXT5_S3TC, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	#endif

	if (glGetError() != GL_NO_ERROR) {
		stbi_image_free(data);
		return nullptr;
	}

	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	#ifndef __EGL2__
	float ani = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ani);
	if (ani > 1) {
		ani = math::max(4.f, ani);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ani);
	}
	#endif

	if (glGetError() == GL_NO_ERROR) {
		Texture *tex = new Texture(name, textureId);
		printf("Successfully loaded texture with name \"%s\" (%p:%u)\n", name.c_str(), tex, tex->getId());
		textures.push_back(tex);
		return tex;
	}
	return nullptr;
}

RenderObject *Resources::loadModel(string path, string name){
	for (unsigned int i = 0; i < models.size(); i++)
		if (models[i]->getName() == name) 
			return models[i];
	
	ifstream i(path, ios::in | ios::binary);
	if (!i.good()){
		printf("Couldn't open file \"%s\".\n", path.c_str());	
		return nullptr;
	}
	istream::pos_type start = i.tellg();
	i.seekg(0, ios::end);
	u32 len = i.tellg() - start;
	i.seekg(0, ios::beg);
	
	RenderObject *ro = nullptr;
	
	bool closed = false;
	
	if (len > 0) {
		char *block = new char[len];
		i.read(block, len);
		if (StringUtils::endsWith(path, ".oiRM")) ro = loadRawModel(name, block, len);
		else if (StringUtils::endsWith(path, ".oiEM")) printf("Failed to parse a model; .oiEM. Type not implemented yet\n");
		else if (StringUtils::endsWith(path, ".oiOM")) printf("Failed to parse a model; .oiOM. Type not implemented yet\n");
		else if (StringUtils::endsWith(path, ".oiFM")) printf("Failed to parse a model; .oiFM. Type not implemented yet\n");
		else if (StringUtils::endsWith(path, ".oiM")) printf("Failed to parse a model; .oiM. Type not implemented yet\n");
		else if (StringUtils::endsWith(path, ".obj")) {
			i.close();
			ro = loadObj(name, path);
			closed = true;
		}
		else if (StringUtils::endsWith(path, ".md2")) ro = loadMD2(name, block, len);
		else printf("Failed to parse a model from \"%s\". Supported model types: oiRM, oiEM, oiOM, oiFM, oiM, obj and md2\n", path.c_str());
		delete[] block;
	}
		
	if(!closed)i.close();
	if (ro != nullptr) {
		printf("Successfully loaded model with name \"%s\"\n", name.c_str());
		models.push_back(ro);
	}
	else printf("Couldn't load model with name \"%s\"\n", name.c_str());
	return ro;
}
RenderObject *Resources::loadRawModel(string name, char *block, u32 len){
	if (len > 12) {
		u32 val = BinaryHelper::getData(block, 0, 4);
		//Magic number; oiRM
		if (val == 0x6F69524D) {
			u32 vertexLength = BinaryHelper::getData(block, 4, 8);
			u32 indexLength = BinaryHelper::getData(block, 8, 12);
					
			u32 bytePerIndex = indexLength <= 256 ? 1 : (indexLength <= 65536 ? 2 : 4);
					
			u32 vertexDat = vertexLength * 8 * sizeof(GLfloat);
			u32 indexDat = indexLength * bytePerIndex;
			
			if (len == 12 + vertexDat + indexDat) {
				float *vertex = BinaryHelper::getFloats(block, 12, vertexDat /  sizeof(GLfloat));
				
				char *dat = (char*)(block + 12 + vertexDat);
				
				u8 *ind = new u8[indexDat];
				memcpy(ind, dat, indexDat);
				
				RenderObject *ro = loadModel(name, vertex, vertexDat, ind, bytePerIndex, indexDat);
				return ro;
			}
			else printf("Couldn't load raw model! Length was %u/%u\n", len, 12 + vertexDat + indexDat);
		}
		else printf("Couldn't load raw model! Magic number was %u\n", val);
	}
	else printf("Couldn't load raw model! Length was %u\n", len);
	return nullptr;
}
RenderObject *Resources::loadModel(string name, float *vert, u32 vertBytes, u8 *dat, u32 bytePerIndex, u32 indBytes) {
	Mesh *m = Mesh::create(name, vert, vertBytes, dat, bytePerIndex, indBytes);
	
	u32 trcount = indBytes / bytePerIndex / 3;
	
	vector<Triangle> tris(trcount);
	for (u32 i = 0; i < trcount; i++) {
		vec3 arr[3];
		for (u32 j = 0; j < 3; j++) {
			u8 *off = dat + i * 3 * bytePerIndex + j * bytePerIndex;
			u32 index = bytePerIndex == 1 ? (u32)(*off) : (bytePerIndex == 2 ? (u32)(*(u16*)off) : *(u32*)off);
			u32 offsets = index * 8;
			arr[j] = *(vec3*)&vert[offsets];
		}
		tris[i] = Triangle(arr[0], arr[1], arr[2]);
	}
	
	m->setChunk(new CChunk(m->getAABB(), tris));
	
	return m;
}
Shader *Resources::loadShader(string vert, string frag, string name, u8 flag) {
	#ifdef __RASPBERRY__
	if(flag & 0b10 || flag & 0x1){
		printf("WARNING: Tried to use lighting on the PI. The PI doen't support a lot of uniforms and doesn't allow for lights.\n");
		flag &= !(u8)0b11;
	}
	#endif
	#ifdef __EGL2__
	if (flag & 0b10) {
		printf("WARNING: EGL2.0 doesn't support multiple render targets; so deferred rendering is impossible!\n");
		flag &= !(u8)0b10;
	}
	#elif defined(__OGL__)
	if (flag & 0b1 && (flag & 0b10) == 0) {
		printf("WARNING: OGL tried to use lighting without enabling deferred rendering! Switching to deferred\n");
		flag &= !(u8)(0b1);
		flag |= (u8)0b10;
	}
	#endif
	for (unsigned int i = 0; i < shaders.size(); i++)
		if (shaders[i]->getName() == name) 
			return shaders[i];
	string vertstr = StringUtils::fromFile(vert);
	string fragstr = StringUtils::fromFile(frag);
	if (vertstr.size() == 0 || fragstr.size() == 0) {
		printf("Couldn't load shader! Vert or frag string were NULL\n");
		return nullptr;
	}
	Shader *s = Shader::create(vertstr, fragstr, name, flag);
	if (s != nullptr){
		shaders.push_back(s);
		printf("Successfully loaded shader with name \"%s\"\n", name.c_str());
		return s;
	}
	return nullptr;
}
ComputeShader *Resources::loadShader(std::string csPath, std::string name) {
	string cs = StringUtils::fromFile(csPath);
	if (cs.size() == 0) {
		printf("Couldn't load compute shader! Source doesn't exist!\n");
		return nullptr;
	}
	ComputeShader *s = ComputeShader::create(name, cs);
	if (s != nullptr) {
		shaders.push_back(s);
		printf("Successfully loaded compute shader with name \"%s\"\n", name.c_str());
		return s;
	}
	return nullptr;
}
bool Resources::writeModel(string path, string name){
	RenderObject *obj = nullptr;
	for (unsigned int i = 0; i < models.size(); i++)
		if (models[i]->getName() == name){
			obj = models[i];
			break;
		}	
	if (obj == nullptr){
		printf("Couldn't write that model! Please ensure that you have loaded it as a Mesh... The exporter only allows VBO + IBO outputs and not pure VBO outputs.\n");
		return false;
	}
	
	if (StringUtils::endsWith(path, ".oiRM")) {
		if (Defines::instanceof<Mesh>(obj))
			return writeRM(path, (Mesh*)obj);
		if (Defines::instanceof<TexturedMesh>(obj) && ((TexturedMesh*)obj)->getTextures() <= 1)
			return writeRM(path, (TexturedMesh*)obj);
		
		printf("Tried to write model \"%s\", but it can't be converted to .oiRM!\n", name.c_str());
		return false;
	}
	return false;
}
bool Resources::writeRM(std::string path, Mesh *m) {
	ofstream out(path, ios::out | ios::trunc | ios::binary);
	if (!out.good()){
		printf("Couldn't open file \"%s\" for output!\n", path.c_str());
		return false;
	}
	u32 l = 12 + m->vertLength + m->indexPtrLength * m->vertices;
	char *buffer = new char[l];
	buffer[0] = 'o';
	buffer[1] = 'i';
	buffer[2] = 'R';
	buffer[3] = 'M';
	
	BinaryHelper::writeData(buffer, 4, m->vertLength / sizeof(GLfloat) / 8);
	BinaryHelper::writeData(buffer, 8, m->vertices);
	
	BinaryHelper::writeData(buffer, 12, m->vertexBuffer, m->vertLength / sizeof(GLfloat));
	BinaryHelper::writeData(buffer, 12 + m->vertLength, m->indexBuffer, m->vertices * m->indexPtrLength);
	
	out.write(buffer, l);
	
	printf("Successfully wrote a oiRM file!\n");
	
	out.close();
	
	return true;
}
struct RMIndex_t {
	vec3 pos;
	vec2 uv;
	vec3 norm;
	RMIndex_t(vec3 p, vec2 u, vec3 n): pos(p), uv(u), norm(n){}
	RMIndex_t(): RMIndex_t(vec3(), vec2(), vec3()) {}
	bool operator==(RMIndex_t rit){ return pos == rit.pos && uv == rit.uv && norm == rit.norm; }
};
bool Resources::writeRM(std::string path, TexturedMesh *m) {
	printf("Converting TexturedMesh to oiRM format is not supported yet!\n");
	return false;
//	if (m->getTextures() > 1){
//		printf("Couldn't downcast TexturedMesh to Mesh; it uses more than 1 texture!\n");
//		return false;
//	}
//	ofstream out(path, ios::out | ios::trunc | ios::binary);
//	if (!out.good()) {
//		printf("Couldn't open file \"%s\" for output!\n", path.c_str());
//		return false;
//	}
//	u8 ptrLen = m->getVertices() <= 256 ? 1 : (m->getVertices() <= 65536 ? 2 : 4);
//		
//	vector<RMIndex_t> vert(m->getVertices());
//	vector<u32> indices(m->getVertices());
//	u32 k = 0, l = 0;
//	GLfloat *d = m->getVertex();
//	for (u32 i = 0; i < m->getVertices(); i++){
//		RMIndex_t v = RMIndex_t(*(vec3*)&d[8 * i], *(vec2*)&d[8 * i + 3], *(vec3*)&d[8 * i + 5]);
//		bool contains = false;
//		u32 n = 0;
//		for (u32 j = 0; j < k; j++)
//			if (vert[j] == v) {
//				n = j;
//				contains = true;
//				break;
//			}
//		if (!contains) {
//			vert[k] = v;
//			indices[l] = k;
//			k++;
//			l++;
//		}
//		else{
//			indices[l] = n;
//			l++;
//		}
//	}
//	
//	GLfloat *vertDat = new GLfloat[8 * k];
//	u8 *indDat = new u8[l * ptrLen];
//	
//	for (u32 i = 0; i < k; i++){
//		*(vec3*)&vertDat[8 * i] = vert[i].pos;
//		*(vec2*)&vertDat[8 * i + 3] = vert[i].uv;
//		*(vec3*)&vertDat[8 * i + 5] = vert[i].norm;
//	}
//	for (u32 i = 0; i < l; i++) {
//		if (ptrLen == 1)
//			indDat[i] = indices[i];
//		else if (ptrLen == 2){
//			indDat[i*2] = (indices[i] & 0xFF00) >> 8;
//			indDat[i*2+1] = indices[i] & 0xFF;
//		}else if (ptrLen == 4){
//			indDat[i * 4] = (indices[i] & 0xFF000000) >> 24;
//			indDat[i * 4 + 1] = (indices[i] & 0xFF0000) >> 16;
//			indDat[i * 4 + 2] = (indices[i] & 0xFF00) >> 8;
//			indDat[i * 4 + 3] = indices[i] & 0xFF;
//		}
//	}
//	
//	u32 len = 12 + k * 8 * sizeof(GLfloat) + ptrLen * l;
//	
//	char *buffer = new char[len];
//	for (u32 i = 0; i < len; i++)buffer[i] = 0;
//	
//	buffer[0] = 'o';
//	buffer[1] = 'i';
//	buffer[2] = 'R';
//	buffer[3] = 'M';
//	
//	BinaryHelper::writeData(buffer, 4, k);
//	BinaryHelper::writeData(buffer, 8, l);
//	
//	printf("%u\n", len);
//	
//	for (u32 i = 0; i < k * 8; i++)printf("%f\n", vertDat[i]);
//	BinaryHelper::writeData(buffer, 12, vertDat, k * 8);
//	BinaryHelper::writeData(buffer, 12 + k * 8 * sizeof(GLfloat), indDat, l * ptrLen);
//	
//	out.write(buffer, len);
//	
//	printf("Successfully wrote a oiRM file!\n");
//	
//	out.close();
//	
//	return true;
}

bool Resources::hasTexture(std::string name) {
	for (unsigned int i = 0; i < textures.size(); i++)
		if (textures[i]->getName() == name)
			return true;
	return false;
}
bool Resources::hasModel(std::string name) {
	for (unsigned int i = 0; i < models.size(); i++)
		if (models[i]->getName() == name)
			return true;
	return false;
}
bool Resources::hasShader(std::string name) {
	for (unsigned int i = 0; i < shaders.size(); i++)
		if (shaders[i]->getName() == name)
			return true;
	return false;
}
Texture *Resources::fetchTexture(std::string name) {
	for (unsigned int i = 0; i < textures.size(); i++)
		if (textures[i]->getName() == name)
			return textures[i];
	return nullptr;
}
RenderObject *Resources::fetchModel(std::string name) {
	for (unsigned int i = 0; i < models.size(); i++)
		if (models[i]->getName() == name)
			return models[i];
	return nullptr;
}
Shader *Resources::fetchShader(std::string name) {
	for (unsigned int i = 0; i < shaders.size(); i++)
		if (shaders[i]->getName() == name)
			return shaders[i];
	return nullptr;
}

bool Resources::unloadTexture(std::string name) {
	for (unsigned int i = 0; i < textures.size(); i++)
		if (textures[i]->getName() == name){
			delete textures[i];
			textures.erase(textures.begin() + i);
			printf("Unloading texture \"%s\"\n", name.c_str());
			return true;
		}
	return false;
}
bool Resources::unloadModel(std::string name) {
	for (unsigned int i = 0; i < models.size(); i++)
		if (models[i]->getName() == name) {
			delete models[i];
			models.erase(models.begin() + i);
			printf("Unloading model \"%s\"\n", name.c_str());
			return true;
		}
	return false;
}
bool Resources::unloadShader(std::string name) {
	for (unsigned int i = 0; i < shaders.size(); i++)
		if (shaders[i]->getName() == name) {
			delete shaders[i];
			shaders.erase(shaders.begin() + i);
			printf("Unloading shader \"%s\"\n", name.c_str());
			return true;
		}

	return false;
}
//Obj loading
RenderObject *Resources::loadObj(std::string name, std::string in){
	string mtlFile = StringUtils::combine(StringUtils::split(StringUtils::grabPath(in), '\\'), "/");
	
	tinyobj::attrib_t attrib;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;

	string err;
	bool ret = LoadObj(&attrib, &shapes, &materials, &err, in.c_str(), mtlFile.c_str(), true);

	if (!err.empty() || shapes.size() == 0) {
		printf("Couldn't open \"%s\"; error '%s'!\n", in.c_str(), err.c_str());
		return nullptr;
	}

	if (!ret) {
		printf("Couldn't open \"%s\"; the file result is NULL!\n", in.c_str());
		return nullptr;
	}
	
	u32 *off = new u32[shapes.size()];
	u32 tris = 0;
	for (u32 i = 0; i < shapes.size(); i++){
		off[i] = tris;
		tris += shapes[i].mesh.indices.size() / 3;	
	}
	u32 *mIdBuf = new u32[tris];
	vector<u32> startEnd;
	u32 *mcount = new u32[materials.size()];
	startEnd.push_back(shapes[0].mesh.material_ids[0]);
	mcount[shapes[0].mesh.material_ids[0]]++;
	startEnd.push_back(0);
	u32 prevMaterial = 0xFFFFFFFF;
	
	vector<u32> perShapeOffset;
	for (u32 i = 0; i < shapes.size(); i++)
		for (u32 j = 0; j < shapes[i].mesh.indices.size() / 3; j++){
			u32 k = off[i] + j;
			u32 m = mIdBuf[k] = shapes[i].mesh.material_ids[j];
			if (prevMaterial != 0xFFFFFFFF && prevMaterial != m){
				startEnd.push_back(k-1);
				mcount[m]++;
				startEnd.push_back(m);
				startEnd.push_back(k);
			}
			if (prevMaterial == 0xFFFFFFFF || prevMaterial != m)
				prevMaterial = m;
		}
	startEnd.push_back(tris - 1);
	
	GLfloat *buffer = new GLfloat[tris * 3 * 8];
	
	vec3 mi = vec3::max(), ma = vec3::min();
	
	u32 currtri = 0;
	vector<u32> offsets = vector<u32>(materials.size()+1);
	offsets[0] = 0;
	for (u32 i = 0; i < materials.size(); i++) {
		for (u32 j = 0; j < startEnd.size() / 3; j++){
			u32 m = startEnd[j*3];
			if (m != i)continue;
			
			u32 beg = startEnd[j*3 + 1];
			u32 end = startEnd[j*3 + 2];
			
			for (u32 k = beg; k <= end; k++){
				u32 sh = 0;
				u32 offs = 0;
				for (u32 l = 0; l < materials.size(); l++)
					if (k >= off[l]){
						sh = l;
						offs = k - off[l];
						break;
					}
				for (u32 l = 0; l < 3; l++){
					tinyobj::index_t idx = shapes[sh].mesh.indices[offs * 3 + l];
					float vx = attrib.vertices[3*idx.vertex_index + 0];
					float vy = attrib.vertices[3*idx.vertex_index + 1];
					float vz = attrib.vertices[3*idx.vertex_index + 2];
					float tx = attrib.texcoords[2*idx.texcoord_index + 0];
					float ty = attrib.texcoords[2*idx.texcoord_index + 1];
					float nx = attrib.normals[3*idx.normal_index + 0];
					float ny = attrib.normals[3*idx.normal_index + 1];
					float nz = attrib.normals[3*idx.normal_index + 2];
					*(vec3*)&buffer[(currtri * 3 + l) * 8] = vec3(vx, vy, vz);
					*(vec2*)&buffer[(currtri * 3 + l) * 8  + 3] = vec2(tx, ty);
					*(vec3*)&buffer[(currtri * 3 + l) * 8  + 5] = vec3(nx, ny, nz);
					if (vx < mi.x)mi.x = vx;
					if (vy < mi.y)mi.y = vy;
					if (vz < mi.z)mi.z = vz;
					if (vx > ma.x)ma.x = vx;
					if (vy > ma.y)ma.y = vy;
					if (vz > ma.z)ma.z = vz;
				}
				currtri++;
			}
		}
		offsets[i+1] = currtri;
	}
	
	delete[] off;
	delete[] mIdBuf;
	delete[] mcount;
	
	if (mi == vec3::max())mi = vec3();
	if (ma == vec3::min())ma = vec3();
		
	return TexturedMesh::create(name, buffer, tris * 3, materials, offsets, AABB(mi, ma));
}
//MD2 loading
float anorms_table[162][3] = {
	{ -0.525731f, 0.000000f, 0.850651f },
	{ -0.442863f, 0.238856f, 0.864188f },
	{ -0.295242f, 0.000000f, 0.955423f },
	{ -0.309017f, 0.500000f, 0.809017f },
	{ -0.162460f, 0.262866f, 0.951056f },
	{ 0.000000f, 0.000000f, 1.000000f },
	{ 0.000000f, 0.850651f, 0.525731f },
	{ -0.147621f, 0.716567f, 0.681718f },
	{ 0.147621f, 0.716567f, 0.681718f },
	{ 0.000000f, 0.525731f, 0.850651f },
	{ 0.309017f, 0.500000f, 0.809017f },
	{ 0.525731f, 0.000000f, 0.850651f },
	{ 0.295242f, 0.000000f, 0.955423f },
	{ 0.442863f, 0.238856f, 0.864188f },
	{ 0.162460f, 0.262866f, 0.951056f },
	{ -0.681718f, 0.147621f, 0.716567f },
	{ -0.809017f, 0.309017f, 0.500000f },
	{ -0.587785f, 0.425325f, 0.688191f },
	{ -0.850651f, 0.525731f, 0.000000f },
	{ -0.864188f, 0.442863f, 0.238856f },
	{ -0.716567f, 0.681718f, 0.147621f },
	{ -0.688191f, 0.587785f, 0.425325f },
	{ -0.500000f, 0.809017f, 0.309017f },
	{ -0.238856f, 0.864188f, 0.442863f },
	{ -0.425325f, 0.688191f, 0.587785f },
	{ -0.716567f, 0.681718f, -0.147621f },
	{ -0.500000f, 0.809017f, -0.309017f },
	{ -0.525731f, 0.850651f, 0.000000f },
	{ 0.000000f, 0.850651f, -0.525731f },
	{ -0.238856f, 0.864188f, -0.442863f },
	{ 0.000000f, 0.955423f, -0.295242f },
	{ -0.262866f, 0.951056f, -0.162460f },
	{ 0.000000f, 1.000000f, 0.000000f },
	{ 0.000000f, 0.955423f, 0.295242f },
	{ -0.262866f, 0.951056f, 0.162460f },
	{ 0.238856f, 0.864188f, 0.442863f },
	{ 0.262866f, 0.951056f, 0.162460f },
	{ 0.500000f, 0.809017f, 0.309017f },
	{ 0.238856f, 0.864188f, -0.442863f },
	{ 0.262866f, 0.951056f, -0.162460f },
	{ 0.500000f, 0.809017f, -0.309017f },
	{ 0.850651f, 0.525731f, 0.000000f },
	{ 0.716567f, 0.681718f, 0.147621f },
	{ 0.716567f, 0.681718f, -0.147621f },
	{ 0.525731f, 0.850651f, 0.000000f },
	{ 0.425325f, 0.688191f, 0.587785f },
	{ 0.864188f, 0.442863f, 0.238856f },
	{ 0.688191f, 0.587785f, 0.425325f },
	{ 0.809017f, 0.309017f, 0.500000f },
	{ 0.681718f, 0.147621f, 0.716567f },
	{ 0.587785f, 0.425325f, 0.688191f },
	{ 0.955423f, 0.295242f, 0.000000f },
	{ 1.000000f, 0.000000f, 0.000000f },
	{ 0.951056f, 0.162460f, 0.262866f },
	{ 0.850651f, -0.525731f, 0.000000f },
	{ 0.955423f, -0.295242f, 0.000000f },
	{ 0.864188f, -0.442863f, 0.238856f },
	{ 0.951056f, -0.162460f, 0.262866f },
	{ 0.809017f, -0.309017f, 0.500000f },
	{ 0.681718f, -0.147621f, 0.716567f },
	{ 0.850651f, 0.000000f, 0.525731f },
	{ 0.864188f, 0.442863f, -0.238856f },
	{ 0.809017f, 0.309017f, -0.500000f },
	{ 0.951056f, 0.162460f, -0.262866f },
	{ 0.525731f, 0.000000f, -0.850651f },
	{ 0.681718f, 0.147621f, -0.716567f },
	{ 0.681718f, -0.147621f, -0.716567f },
	{ 0.850651f, 0.000000f, -0.525731f },
	{ 0.809017f, -0.309017f, -0.500000f },
	{ 0.864188f, -0.442863f, -0.238856f },
	{ 0.951056f, -0.162460f, -0.262866f },
	{ 0.147621f, 0.716567f, -0.681718f },
	{ 0.309017f, 0.500000f, -0.809017f },
	{ 0.425325f, 0.688191f, -0.587785f },
	{ 0.442863f, 0.238856f, -0.864188f },
	{ 0.587785f, 0.425325f, -0.688191f },
	{ 0.688191f, 0.587785f, -0.425325f },
	{ -0.147621f, 0.716567f, -0.681718f },
	{ -0.309017f, 0.500000f, -0.809017f },
	{ 0.000000f, 0.525731f, -0.850651f },
	{ -0.525731f, 0.000000f, -0.850651f },
	{ -0.442863f, 0.238856f, -0.864188f },
	{ -0.295242f, 0.000000f, -0.955423f },
	{ -0.162460f, 0.262866f, -0.951056f },
	{ 0.000000f, 0.000000f, -1.000000f },
	{ 0.295242f, 0.000000f, -0.955423f },
	{ 0.162460f, 0.262866f, -0.951056f },
	{ -0.442863f, -0.238856f, -0.864188f },
	{ -0.309017f, -0.500000f, -0.809017f },
	{ -0.162460f, -0.262866f, -0.951056f },
	{ 0.000000f, -0.850651f, -0.525731f },
	{ -0.147621f, -0.716567f, -0.681718f },
	{ 0.147621f, -0.716567f, -0.681718f },
	{ 0.000000f, -0.525731f, -0.850651f },
	{ 0.309017f, -0.500000f, -0.809017f },
	{ 0.442863f, -0.238856f, -0.864188f },
	{ 0.162460f, -0.262866f, -0.951056f },
	{ 0.238856f, -0.864188f, -0.442863f },
	{ 0.500000f, -0.809017f, -0.309017f },
	{ 0.425325f, -0.688191f, -0.587785f },
	{ 0.716567f, -0.681718f, -0.147621f },
	{ 0.688191f, -0.587785f, -0.425325f },
	{ 0.587785f, -0.425325f, -0.688191f },
	{ 0.000000f, -0.955423f, -0.295242f },
	{ 0.000000f, -1.000000f, 0.000000f },
	{ 0.262866f, -0.951056f, -0.162460f },
	{ 0.000000f, -0.850651f, 0.525731f },
	{ 0.000000f, -0.955423f, 0.295242f },
	{ 0.238856f, -0.864188f, 0.442863f },
	{ 0.262866f, -0.951056f, 0.162460f },
	{ 0.500000f, -0.809017f, 0.309017f },
	{ 0.716567f, -0.681718f, 0.147621f },
	{ 0.525731f, -0.850651f, 0.000000f },
	{ -0.238856f, -0.864188f, -0.442863f },
	{ -0.500000f, -0.809017f, -0.309017f },
	{ -0.262866f, -0.951056f, -0.162460f },
	{ -0.850651f, -0.525731f, 0.000000f },
	{ -0.716567f, -0.681718f, -0.147621f },
	{ -0.716567f, -0.681718f, 0.147621f },
	{ -0.525731f, -0.850651f, 0.000000f },
	{ -0.500000f, -0.809017f, 0.309017f },
	{ -0.238856f, -0.864188f, 0.442863f },
	{ -0.262866f, -0.951056f, 0.162460f },
	{ -0.864188f, -0.442863f, 0.238856f },
	{ -0.809017f, -0.309017f, 0.500000f },
	{ -0.688191f, -0.587785f, 0.425325f },
	{ -0.681718f, -0.147621f, 0.716567f },
	{ -0.442863f, -0.238856f, 0.864188f },
	{ -0.587785f, -0.425325f, 0.688191f },
	{ -0.309017f, -0.500000f, 0.809017f },
	{ -0.147621f, -0.716567f, 0.681718f },
	{ -0.425325f, -0.688191f, 0.587785f },
	{ -0.162460f, -0.262866f, 0.951056f },
	{ 0.442863f, -0.238856f, 0.864188f },
	{ 0.162460f, -0.262866f, 0.951056f },
	{ 0.309017f, -0.500000f, 0.809017f },
	{ 0.147621f, -0.716567f, 0.681718f },
	{ 0.000000f, -0.525731f, 0.850651f },
	{ 0.425325f, -0.688191f, 0.587785f },
	{ 0.587785f, -0.425325f, 0.688191f },
	{ 0.688191f, -0.587785f, 0.425325f },
	{ -0.955423f, 0.295242f, 0.000000f },
	{ -0.951056f, 0.162460f, 0.262866f },
	{ -1.000000f, 0.000000f, 0.000000f },
	{ -0.850651f, 0.000000f, 0.525731f },
	{ -0.955423f, -0.295242f, 0.000000f },
	{ -0.951056f, -0.162460f, 0.262866f },
	{ -0.864188f, 0.442863f, -0.238856f },
	{ -0.951056f, 0.162460f, -0.262866f },
	{ -0.809017f, 0.309017f, -0.500000f },
	{ -0.864188f, -0.442863f, -0.238856f },
	{ -0.951056f, -0.162460f, -0.262866f },
	{ -0.809017f, -0.309017f, -0.500000f },
	{ -0.681718f, 0.147621f, -0.716567f },
	{ -0.681718f, -0.147621f, -0.716567f },
	{ -0.850651f, 0.000000f, -0.525731f },
	{ -0.688191f, 0.587785f, -0.425325f },
	{ -0.587785f, 0.425325f, -0.688191f },
	{ -0.425325f, 0.688191f, -0.587785f },
	{ -0.425325f, -0.688191f, -0.587785f },
	{ -0.587785f, -0.425325f, -0.688191f },
	{ -0.688191f, -0.587785f, -0.425325f }
};
struct md2_header_t{
	i32 ident;                  /* magic number: "IDP2" */
	i32 version;                /* version: must be 8 */

	i32 skinwidth;              /* texture width */
	i32 skinheight;             /* texture height */

	i32 framesize;              /* size in bytes of a frame */

	i32 num_skins;              /* number of skins */
	i32 num_vertices;           /* number of vertices per frame */
	i32 num_st;                 /* number of texture coordinates */
	i32 num_tris;               /* number of triangles */
	i32 num_glcmds;             /* number of opengl commands */
	i32 num_frames;             /* number of frames */

	i32 offset_skins;           /* offset skin data */
	i32 offset_st;              /* offset texture coordinate data */
	i32 offset_tris;            /* offset triangle data */
	i32 offset_frames;          /* offset frame data */
	i32 offset_glcmds;          /* offset OpenGL command data */
	i32 offset_end;             /* offset end of file */
};
struct md2_skin_t{
	char name[64];
};
struct md2_texCoord_t{
	i16 s;
	i16 t;
}; 
struct md2_triangle_t{
	u16 vertex[3];   /* vertex indices of the triangle */
	u16 st[3];       /* tex. coord. indices */
};
struct md2_vertex_t{
	u8 v[3];         /* position */
	u8 normalIndex;  /* normal vector index */
};
struct md2_frame_t{
	float scale[3];               /* scale factor */
	float translate[3];           /* translation vector */
	char name[16];              /* frame name */
	struct md2_vertex_t *verts; /* list of frame's vertices */
};
struct md2_model_t {
	md2_header_t header;
	md2_skin_t *skins;
	md2_texCoord_t *texcoords;
	md2_triangle_t *triangles;
	md2_frame_t *frames;
	i32 *glcmds;
};
RenderObject *Resources::loadMD2(string name, char *block, u32 len) {
	char *bstart = block;
	
	if (len <= sizeof(md2_header_t)) {
		printf("Model \"%s\" doesn't start with an MD2 header!\n", name.c_str());
		return nullptr;
	}
	
	md2_header_t header = *(md2_header_t*)block;
	
	if ((header.ident != 844121161) || (header.version != 8)) {
		printf("Model \"%s\" doesn't start with an MD2 header!\n", name.c_str());
		return nullptr;
	}
	if (len != header.offset_end){
		printf("Model \"%s\" doesn't have %u bytes!\n", name.c_str(), header.offset_end);
		return nullptr;
	}
	md2_model_t mdl = md2_model_t();
	mdl.header = header;
	mdl.skins = (md2_skin_t *) malloc(sizeof(md2_skin_t) * header.num_skins);
	mdl.texcoords = (md2_texCoord_t *) malloc(sizeof(md2_texCoord_t) * header.num_st);
	mdl.triangles = (md2_triangle_t *) malloc(sizeof(md2_triangle_t) * header.num_tris);
	mdl.frames = (md2_frame_t *) malloc(sizeof(md2_frame_t) * header.num_frames);
	mdl.glcmds = (i32 *) malloc(sizeof(i32) * header.num_glcmds);
	
	block = bstart + header.offset_skins;
	for (u32 i = 0; i < header.num_skins; i++)
		mdl.skins[i] = *(md2_skin_t*)(block + sizeof(md2_skin_t) * i);
	
	block = bstart + header.offset_st;
	for (u32 i = 0; i < header.num_st; i++)
		mdl.texcoords[i] = *(md2_texCoord_t*)(block + sizeof(md2_texCoord_t) * i);
	
	block = bstart + header.offset_tris;
	for (u32 i = 0; i < header.num_tris; i++) 
		mdl.triangles[i] = *(md2_triangle_t*)(block + sizeof(md2_triangle_t) * i);
	
	block = bstart + header.offset_glcmds;
	for (u32 i = 0; i < header.num_glcmds; i++)
		mdl.glcmds[i] = *(i32*)(block + sizeof(i32) * i);
	
	vector<FrameState> states;
	
	for (u32 i = 0; i < header.num_frames; ++i) {
		block = bstart + header.offset_frames + header.framesize * i;
		
		mdl.frames[i].verts = (md2_vertex_t*) malloc(sizeof(md2_vertex_t) * header.num_vertices);

		*((vec3*)&mdl.frames[i].scale) = *(vec3*)block;
		*((vec3*)&mdl.frames[i].translate) = *(vec3*)(block + 3 * sizeof(GLfloat));
		memcpy(mdl.frames[i].name, block + 2 * 3 * sizeof(GLfloat), 16);
		
		for (u32 j = 0; j < header.num_vertices; j++)
			mdl.frames[i].verts[j] = *(md2_vertex_t*)(block + 2 * 3 * sizeof(GLfloat) + 16 + j * sizeof(md2_vertex_t));
		
		vector<string> vs = StringUtils::splitStringNum(string(mdl.frames[i].name, 16));
		if (vs.size() < 2) {
			printf("WARNING Trying to create FrameState! Syntax: name<frameNumber>\n");
			for (int j = 0; j < i; j++)
				free(mdl.frames[j].verts);
			free(mdl.skins);
			free(mdl.texcoords);
			free(mdl.triangles);
			free(mdl.frames);
			free(mdl.glcmds);
			return nullptr;
		}
		bool isNum = StringUtils::isNumber(vs[1]);
		if (!isNum) {
			printf("WARNING Trying to create FrameState! Syntax: name<frameNumber>; NaN\n");
			for (int j = 0; j < i; j++)
				free(mdl.frames[j].verts);
			free(mdl.skins);
			free(mdl.texcoords);
			free(mdl.triangles);
			free(mdl.frames);
			free(mdl.glcmds);
			return nullptr;
		}
		unsigned int numb = StringUtils::parseInt(vs[1]);
		string name = vs[0];
		if (numb > 100) {
			name = StringUtils::numstring(numb / 100, name, "");
			numb %= 100;
		}
		unsigned int offset = i;
		bool contains = false;
		for (u32 k = 0; k < states.size(); k++)
			if (states[k].getName() == name){
				states[k].update(numb);
				contains = true;
				break;
			}
		if (!contains) states.push_back(FrameState(name, offset, numb));
	}
	
	
	u32 length = header.num_frames * header.num_tris * 3 * 8;
	GLfloat *buffer = new GLfloat[length];

	vec3 mi = vec3::max(), ma = vec3::min();
	
	for (i32 k = 0; k < header.num_frames; k++) {
		for (i32 i = 0; i < header.num_tris; ++i) {
			for (i32 j = 0; j < 3; ++j) {
				md2_frame_t *pframe = &mdl.frames[k];
				md2_vertex_t *pvert = &pframe->verts[mdl.triangles[i].vertex[j]];

				GLfloat s = (GLfloat)mdl.texcoords[mdl.triangles[i].st[j]].s / mdl.header.skinwidth;
				GLfloat t = (GLfloat)mdl.texcoords[mdl.triangles[i].st[j]].t / mdl.header.skinheight;

				u32 pos = k * mdl.header.num_tris * 3 * 8 + i * 3 * 8 + j * 8;

				vec3 vertpos = vec3((pframe->scale[0] * pvert->v[0]) + pframe->translate[0], (pframe->scale[2] * pvert->v[2]) + pframe->translate[2], (pframe->scale[1] * pvert->v[1]) + pframe->translate[1]);
				
				if (vertpos.x < mi.x)mi.x = vertpos.x;
				if (vertpos.y < mi.y)mi.y = vertpos.y;
				if (vertpos.z < mi.z)mi.z = vertpos.z;
				if (vertpos.x > ma.x)ma.x = vertpos.x;
				if (vertpos.y > ma.y)ma.y = vertpos.y;
				if (vertpos.z > ma.z)ma.z = vertpos.z;
				
				buffer[pos] = vertpos.x;
				buffer[pos + 1] = vertpos.y;
				buffer[pos + 2] = vertpos.z;

				buffer[pos + 3] = s;
				buffer[pos + 4] = t;

				buffer[pos + 5] = anorms_table[pvert->normalIndex][0];
				buffer[pos + 6] = anorms_table[pvert->normalIndex][1];
				buffer[pos + 7] = anorms_table[pvert->normalIndex][2];
			}
		}
	}
	printf("Successfully created the MD2 buffer! (%i %i)\n", header.num_frames, header.num_tris);
	for (int j = 0; j < header.num_frames; j++)
		free(mdl.frames[j].verts);
	free(mdl.skins);
	free(mdl.texcoords);
	free(mdl.triangles);
	free(mdl.frames);
	free(mdl.glcmds);
	
	return MD2Model::create(name, buffer, header.num_tris * 3, header.num_frames, states, AABB(mi, ma));
}