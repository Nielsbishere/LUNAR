#pragma once
#include "../World/Camera.h"
#include "../Render/Light.h"
#include <vector>
class State{
public:
	virtual ~State(){}
	
	//For things that have to be drawn using deferred rendering (Like; the world)
	virtual void render() = 0;

	//For things that have to be drawn using forward rendering (Like; billboards, particles, etc.)
	virtual void renderForward() = 0;
	
	//For things that have to be done before rendering; like moving objects
	virtual void update(double delta) = 0;
	
	//For things that have to be done after rendering and updating; like updating the input handler, handling screenshots, etc.
	virtual void postRender() = 0;

	virtual osomi::Camera *getMainCamera() = 0;
	virtual std::vector<osomi::Light> getLights() = 0;
};