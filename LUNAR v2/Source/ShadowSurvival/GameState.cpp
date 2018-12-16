#include "../../Headers/ShadowSurvival/GameState.h"

#include "../../Headers/Resource/Resources.h"
#include "../../Headers/World/Camera.h"
#include "../../Headers/World/GameObject.h"
#include "../../Headers/Input/InputHandler.h"
#include "../../Headers/UI/Billboard.h"
#include "../../Headers/World/World.h"
#include "../../Headers/Helper/StringUtils.h"

#include "../../Headers/Entity/Behavior/PlayerBehavior.h"

#include "../../Headers/MarchingSquares/MarchingSquares.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../Headers/API/stbi/write.h"

void screenshot(GLint endX, GLint endY, std::string filename) {
	GLint startX = 0, startY = 0;
	GLubyte *fpixels = new GLubyte[3 * endX * endY];

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glReadPixels(startX, startY, endX, endY, GL_RGB, GL_UNSIGNED_BYTE, fpixels);
	
	GLubyte *pixels = new GLubyte[3 * endX * endY];
	for (u32 i = 0; i < endX; i++)
		for (u32 j = 0; j < endY; j++)
			for (u32 k = 0; k < 3; k++)
				pixels[(endY - 1 - j) * endX * 3 + i * 3 + k] = fpixels[j * endX * 3 + i * 3 + k];
	delete[] fpixels;

	stbi_write_png(filename.c_str(), endX, endY, 3, pixels, endX * 3);

	delete[] pixels;
}


void GameState::render() {
	w->render(c, lights);
}
void GameState::renderForward() {
	billBoard->use();
	bh->render(billBoard, c);
}
void GameState::update(double delta) {
	w->update(delta, ih);

	c->update(ih, CODE_W, CODE_S, CODE_D, CODE_A, CODE_E, CODE_Q, 90, 90, 15, 15, 20, delta);
	
//	GameObject *go = w->get("Knight");
//	GameObject *go2 = w->get("Sphere object");
//	go2->move(go->getAABB().getCenter());
//	go2->setScale(go->getAABB().getSize());
}
void GameState::postRender(){	
	if (ih->isKeyPressed(CODE_F3)) screenshot(1920, 1080, "../Resources/Screenshot.png");
}
GameState::~GameState() {
	delete c;
	delete w;
	delete bh;
	
	Resources::unloadTexture("Dirt");
	Resources::unloadTexture("Stone");
	Resources::unloadTexture("Rock");
	Resources::unloadTexture("Knight");
	Resources::unloadModel("Sphere");
	Resources::unloadModel("Plane");
	Resources::unloadModel("Knight");
	Resources::unloadShader("Model shader");
	Resources::unloadShader("MD2 shader");
	Resources::unloadShader("Billboard shader");
	Resources::unloadShader("Terrain shader");
	Resources::unloadShader("PostProcessing");
	Resources::unloadShader("PostProcessing-compute");
	
	//TODO: Bind shaders to Scene and delete those there
	//TODO: Resources 'loadModel' will add ref count and 'unloadModel' will decrease it
	//TODO: When refcount == 0 it will destroy the object
}
GameState::GameState(InputHandler *_ih) {
	ih = _ih;
		
	c = Camera::create(vec3(0, 40, 15));
	c->rotate(-45, -20);

	postProcessing = Resources::loadShader("../Resources/Shaders/postprocessing.vsh", "../Resources/Shaders/postprocessing.fsh", "PostProcessing", 0b00);
	ppCompute = Resources::loadShader("../Resources/Shaders/pp.csh", "PostProcessing-compute");

	bh = new BillboardHelper();
	Resources::loadTexture("../Resources/Textures/Bars.png", "Bars");
//	bh->add("Exp", Resources::loadTexture("../Resources/Textures/Bars.png", "Bars"), vec2(.5f - .125f, -1 + .125 + .25/3*2), vec3(.125, .25 / 3, .25), vec3(1, 1, 1), vec4(0, 36 / 256.f, 214 / 256.f, 70 / 256.f));
//	bh->add("Health", Resources::loadTexture("../Resources/Textures/Bars.png", "Bars"), vec2(.5f - .125f, -1 + .125), vec3(.125, .25 / 3, .25), vec3(1, 1, 1), vec4(0, 70 / 256.f, 214 / 256.f, 105 / 256.f));
	
	w = new World("GameState world");
		
	modelShader = Resources::loadShader("../Resources/Shaders/model.vsh", "../Resources/Shaders/model.fsh", "Model shader", 0b01);
	md2Shader = Resources::loadShader("../Resources/Shaders/md2.vsh", "../Resources/Shaders/md2.fsh", "MD2 shader", 0b01);
	billBoard = Resources::loadShader("../Resources/Shaders/billboard.vsh", "../Resources/Shaders/billboard.fsh", "Billboard shader", 0b00);
	caveShader = Resources::loadShader("../Resources/Shaders/terrain.vsh", "../Resources/Shaders/terrain.fsh", "Terrain shader", 0b01);
		
	GameObject *go = w->add("Knight", md2Shader, Resources::loadModel("../Resources/Models/knight.md2", "Knight"), Resources::loadTexture("../Resources/Textures/knight.png", "Knight"), vec3(15, 25, -20), vec3(), vec3(.1, .1, .1));
	go->setFriction(.1);
	go->applyBehavior(new PlayerBehavior(bh, 30.f, 10.f, 20.f, 8.f, 10.f, 100.f));
	go->attachSpecular(Resources::loadTexture("../Resources/Textures/s_knight.png", "Knight specular"));
	
	go = w->add("Enemy", md2Shader, Resources::loadModel("../Resources/Models/knight.md2", "Knight"), Resources::loadTexture("../Resources/Textures/knight.png", "Knight"), vec3(35, 15, -20), vec3(), vec3(.1, .1, .1));
	go->setFriction(.05);
	go->attachSpecular(Resources::loadTexture("../Resources/Textures/s_knight.png", "Knight specular"));
	go->applyBehavior(new LivingBehavior(bh, 50.f, 5.f));
	
	go = w->add("Plane object", modelShader, Resources::loadModel("../Resources/Models/Plane.oiRM", "Plane"), Resources::loadTexture("../Resources/Textures/Rock.png", "Rock"), vec3(), vec3(), vec3(50, 1, 50));
	go->setUvScale(vec2(15, 15));
	go->setStatic();
	go->attachSpecular(Resources::loadTexture("../Resources/Textures/s_Rock.png", "Rock specular"));
	
//	go = w->add("Sphere object", modelShader, Resources::loadModel("../Resources/Models/Sphere.obj", "Sphere"), Resources::loadTexture("../Resources/Textures/Dirt.png", "Dirt"), vec3());
//	go->setGravity(false);
		
	//TODO: MarchingSquares::generate returns a Mesh* which is NOT in the resource manager; fix pls
	/*go = w->add("Marching squares object", caveShader, (RenderObject*)MarchingSquares::generate("Marching squares", 8, 8, vec2(.125*2, .125*2), vec2(10, 10), vec3(10, 0.5, 2.5)), Resources::loadTexture("../Resources/Textures/Stone.png", "Stone"), vec3(0, 12, 0), vec3(), vec3(50, 4, 50));
	go->setStatic();
	go->attachSpecular(Resources::loadTexture("../Resources/Textures/s_Stone.png", "Stone specular"));*/
	
	RenderObject *sphere = Resources::loadModel("../Resources/Models/Sphere.obj", "Sphere");
	Texture *dirt = Resources::loadTexture("../Resources/Textures/Dirt.png", "Dirt");
	Texture *dspec = Resources::loadTexture("../Resources/Textures/s_Dirt.png", "Dirt specular");

	for (u32 i = 0; i < 1024; i++) {
		vec3 posi = vec3((i / 32) / 32.f * 80 - 20, 1, (i % 32) / 32.f * 80 - 20);
		vec3 coli = vec3(i / 1024.f, (i % 32) / 32.f, (i / 32) / 32.f) / 2.0 + 0.5;
		float powi = 1.5;

		//Calculate affected radius
		if (coli == vec3())continue;

		Light l = Light(posi, coli, powi);
		float r = l.calculateRadius();

		#ifndef __FINAL__
		/*go = w->add(StringUtils::numstring(i, "Light #", ""), modelShader, sphere, dirt, posi + vec3(0, r, 0), vec3(), vec3(r, r, r));
		go->attachSpecular(dspec);
		go->setGravity(false);
		go->ignoreCollision(true);*/
		#endif

		lights.push_back(l);
	}

	/*u32 gw = 7, gh = 7;
	for (u32 j = 0; j <= gh; j++){
		for (u32 i = 0; i <= gw; i++) {
			go = w->add(StringUtils::numstring(j * (gw + 1) + i, "Grid object #", ""), modelShader, sphere, dirt, vec3((i / (gw / 2.f) - 1.f) * 50, 6.5, (j / (gh / 2.f) - 1.f) * 50), vec3(), vec3(1.f / gw * 25, 6, 1.f / gh * 25));
			go->attachSpecular(dspec);
			go->setGravity(false);
			go->ignoreCollision(true);
			if (go->isStuck())go->setHidden(true);
		}
	}*/

}