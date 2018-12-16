//List of GameObject
//Marching cubes
	//Cave is now called 'WorldMesh'
	//It is divided in chunks (big)
	//Parts	(smaller; smallest in the render engine)
	//Fragments (cell; smallest in the collision engine)
//Collision
//A*
//Weapons
//UI
//Add BatchHandler for handling multiple GameObjects efficiently
//Add oiEM oiOM oiM writing and reading (and converting TexturedModel (1 material) to oiRM)
//Skybox

//Use smart pointers for resources.

#pragma once
#include "../../Headers/State/State.h"
#include <vector>
using namespace std;

namespace osomi{
	class Camera;
	class Shader;
	class ComputeShader;
	class GameObject;
	class InputHandler;
	class BillboardHelper;
	class Light;
	class World;
}
using namespace osomi;

class GameState : public State{
private:
	Camera *c;
	Shader *modelShader, *md2Shader, *billBoard, *caveShader, *postProcessing;
	ComputeShader *ppCompute;
	World *w;
	InputHandler *ih;
	
	BillboardHelper *bh;
	
	vector<Light> lights;
public:
	GameState(InputHandler *_ih);
	~GameState();
	void update(double delta) override;
	void render() override;
	void renderForward() override;
	void postRender() override;

	Camera *getMainCamera() override { return c; }
	vector<Light> getLights() override { return lights; }
};