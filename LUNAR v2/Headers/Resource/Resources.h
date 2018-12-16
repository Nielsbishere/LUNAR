#pragma once
#include <vector>
#include <string>

#include "Texture.h"
#include "../Render/RenderObject.h"
#include "../Render/Shader.h"
#include "../Math/Types.h"

namespace osomi {
	class Mesh;
	class TexturedMesh;
	class ComputeShader;
	class Resources {
	private:
		static std::vector<Texture*> textures;
		static std::vector<RenderObject*> models;
		static std::vector<Shader*> shaders;
		
		static RenderObject *loadRawModel(std::string name, char *bytes, u32 len);
		static RenderObject *loadMD2(std::string name, char *bytes, u32 len);
		static RenderObject *loadObj(std::string name, std::string path);
		
		static RenderObject *loadModel(std::string name, float *vert, u32 vertBytes, u8 *dat, u32 bytePerIndex, u32 indBytes);
		
		static bool writeRM(std::string path, Mesh *m);
		static bool writeRM(std::string path, TexturedMesh *m);
	public:
		static Texture *loadTexture(std::string path, std::string name);
		static RenderObject *loadModel(std::string path, std::string name);
		//Loads a shader with flags; Those flags decide whether or not certain things are included in the fragment/vertex shader.
		//bool usesDeferred, bool usesLighting
		//Combination 10 doesn't exist; when you use deferred it will always use lighting
		static Shader *loadShader(std::string vertPath, std::string fragPath, std::string name, u8 flags);
		//Load compute shader; returns nullptr when not available
		static ComputeShader *loadShader(std::string csPath, std::string name);
		
		static bool hasTexture(std::string name);
		static bool hasModel(std::string name);
		static bool hasShader(std::string name);
		
		static Texture *fetchTexture(std::string name);
		static RenderObject *fetchModel(std::string name);
		static Shader *fetchShader(std::string name);
		
		static bool unloadTexture(std::string name);
		static bool unloadModel(std::string name);
		static bool unloadShader(std::string name);
		
		//TODO: Use ref counter; if a texture gets loaded a couple times, it will increase at array[index] and if it gets unloaded it will decrease. If that reaches 0, it will get deleted.
		
		//TODO: Write texture and model
		
		static bool writeModel(std::string path, std::string name);
	};
}