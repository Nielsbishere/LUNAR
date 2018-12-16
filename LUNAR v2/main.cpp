#include "Headers/Context/Game.h"
#include "Headers/Math/MathDefines.h"

#include "Headers/Input/InputHandler.h"

#include "Headers/ShadowSurvival/GameState.h"

using namespace std;
using namespace osomi;

State *st = nullptr;
InputHandler *ih = nullptr;

void update(double delta){
	if (st != nullptr)st->update(delta);
}

void render(){
	if (st != nullptr)st->render();
}
void renderForward() {
	if (st != nullptr)st->renderForward();
}
void postRender(){
	if (st != nullptr)st->postRender();
}
Camera *getCamera() {
	if (st != nullptr)return st->getMainCamera();
	return nullptr;
}
vector<Light> getLights() {
	if (st != nullptr)return st->getLights();
	return vector<Light>();
}

void setState(string s){
	if (s == "Game"){
		st = new GameState(ih);
	}else if(s == "Menu"){
		
	}
}

int main(int argc, char *argv[]){
	printf("Starting game\n");
	
	std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();
	
	Game *g = Game::create(1920, 1080, "Shadow survival", &update, &render, &renderForward, &postRender, &getCamera, &getLights, &ih);
	
	chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
	printf("Initializing the context and input handler took %f seconds!\n", chrono::duration_cast<chrono::duration<double>>(t1 - t).count());
	
	setState("Game");
	
	chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
	printf("Initializing the state took %f seconds!\n", chrono::duration_cast<chrono::duration<double>>(t2 - t1).count());
	
	g->start();
	
	chrono::high_resolution_clock::time_point t3 = chrono::high_resolution_clock::now();
	printf("The game ran for %f seconds!\n", chrono::duration_cast<chrono::duration<double>>(t3 - t2).count());
	
	ih->close();
	if(st != nullptr)delete st;
	delete g;
	printf("Ended game!\n");
	delete ih;
}