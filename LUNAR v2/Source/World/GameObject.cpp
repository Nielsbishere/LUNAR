#include "../../Headers/World/GameObject.h"
#include "../../Headers/Math/MathDefines.h"
#include "../../Headers/Render/Shader.h"
#include "../../Headers/Render/AnimationState.h"
#include "../../Headers/Render/MD2Model.h"
#include "../../Headers/Render/Mesh.h"
#include "../../Headers/Defines.h"
#include "../../Headers/World/World.h"
#include "../../Headers/Entity/Behavior.h"
using namespace osomi;

GameObject::GameObject(World *_w, std::string n, RenderObject *render, Texture *texture, vec3 _position, vec3 _rotation, vec3 _scale): w(_w), name(n), ro(render), t(texture), position(_position), scale(_scale), velocity(vec3()), onGround(false), uvScale(vec2(1,1)), defiesGravity(false), isStatic(false), friction(0), dead(false), hidden(false), ignoresCollision(false), spec(nullptr) {
	if (Defines::instanceof<MD2Model>(ro)) {
		fs = ((MD2Model*)ro)->getAnimationState("Idle") ;
		if (fs == nullptr)fs = ((MD2Model*)ro)->getAnimationState("Stand");
	}
	else fs = nullptr;
	animationTime = 0;
	animationSwitched = false;
}
//Stupid c-word stuff starts here
//Thanks to http://www.peroxide.dk/papers/collision/collision.pdf for the main collision code and Sander Snippe for helping
void GameObject::collision(Triangle ttri, CPackage &cp, vec3 velocity) {
	AABB boundingBox = getAABB();
	mat4 espace = mat4::scale(boundingBox.getSize().inverse());
	mat4 invespace = espace;
	invespace.inverse();
	
	vec3 basePoint = espace * vec4(boundingBox.getCenter(), 1);
	
	Triangle etri = ttri * espace;
	
	vec3 evel = espace * vec4(velocity, 0);
	vec3 edir = evel.normalize();
	
	if (ignoresCollision){
		evel = vec3(0, -0.00981, 0);
		edir = vec3(0, -1, 0);
	}
	
	if (edir * etri.asPlane().getNormal() > 0 && !ignoresCollision)return;
	
	vec3 p1 = etri.a();
	vec3 p2 = etri.b();
	vec3 p3 = etri.c();
	
	Plane trianglePlane = etri.asPlane();
	
	vec3 norm = vec3();
	
	double t0, t1;
	bool embeddedInPlane = false;
	
	double signedDistToTrianglePlane = trianglePlane.distance(basePoint);
	
	float normalDotVelocity = trianglePlane.getNormal() * evel;
	// if sphere is travelling parrallel to the plane:
	if (normalDotVelocity == 0.0f) {
		if (fabs(signedDistToTrianglePlane) >= 1.0f) {
		// Sphere is not embedded in plane.
		// No collision possible:
			return;
		}
		else {
		// sphere is embedded in plane.
		// It intersects in the whole range [0..1]
			embeddedInPlane = true;
			t0 = 0.0;
			t1 = 1.0;
		}
	}
	else {
	// N dot D is not 0. Calculate intersection interval:
		t0 = (-1.0 - signedDistToTrianglePlane) / normalDotVelocity;
		t1 = (1.0 - signedDistToTrianglePlane) / normalDotVelocity;
		// Swap so t0 < t1
		if (t0 > t1) {
			double temp = t1;
			t1 = t0;
			t0 = temp;
		}
		// Check that at least one result is within range:
		if (t0 > 1.0f || t1 < 0.0f) {
		// Both t values are outside values [0,1]
		// No collision possible:
			return;
		}
		// Clamp to [0,1]
		if (t0 < 0.0) t0 = 0.0;
		if (t1 < 0.0) t1 = 0.0;
		if (t0 > 1.0) t0 = 1.0;
		if (t1 > 1.0) t1 = 1.0;
	}
	// OK, at this point we have two time values t0 and t1
	// between which the swept sphere intersects with the
	// triangle plane. If any collision is to occur it must
	// happen within this interval.
	vec3 collisionPoint;
	bool foundCollison = false;
	float t = 1.0;
	// First we check for the easy case - collision inside
	// the triangle. If this happens it must be at time t0
	// as this is when the sphere rests on the front side
	// of the triangle plane. Note, this can only happen if
	// the sphere is not embedded in the triangle plane.
	if(!embeddedInPlane) {
		vec3 planeIntersectionPoint =
		(basePoint - trianglePlane.getNormal()) + evel * t0;
		if (etri.intersects(planeIntersectionPoint)){
			foundCollison = true;
			t = t0;
			collisionPoint = planeIntersectionPoint;
			
			norm = trianglePlane.getNormal();
		}
	}
	// if we haven’t found a collision already we’ll have to
	// sweep sphere against points and edges of the triangle.
	// Note: A collision inside the triangle (the check above)
	// will always happen before a vertex or edge collision!
	// This is why we can skip the swept test if the above
	// gives a collision!
	if (!foundCollison) {
	// some commonly used terms:
		float velocitySquaredLength = evel.squareLength();
		float a, b, c; // Params for equation
		float newT;
		// For each vertex or edge a quadratic equation have to
		// be solved. We parameterize this equation as
		// a*t^2 + b*t + c = 0 and below we calculate the
		// parameters a,b and c for each test.
		// Check against points:
		a = velocitySquaredLength;
		// P1
		b = 2.0*(evel * (basePoint - p1));
		c = (p1 - basePoint).squareLength() - 1.0;
		if (math::getLowestRoot(a, b, c, t, &newT)) {
			t = newT;
			foundCollison = true;
			collisionPoint = p1;
			
			norm = -(vec3(invespace * vec4(collisionPoint, 1)) - (this->position + this->velocity * t)).normalize();
		}
		// P2
		b = 2.0*(evel * (basePoint - p2));
		c = (p2 - basePoint).squareLength() - 1.0;
		if (math::getLowestRoot(a, b, c, t, &newT)) {
			t = newT;
			foundCollison = true;
			collisionPoint = p2;

			norm = -(vec3(invespace * vec4(collisionPoint, 1)) - (this->position + this->velocity * t)).normalize();
		}
		// P3
		b = 2.0*(evel * (basePoint - p3));
		c = (p3 - basePoint).squareLength() - 1.0;
		if (math::getLowestRoot(a, b, c, t, &newT)) {
			t = newT;
			foundCollison = true;
			collisionPoint = p3;

			norm = -(vec3(invespace * vec4(collisionPoint, 1)) - (this->position + this->velocity * t)).normalize();
		}
		// Check agains edges:
		// p1 -> p2:
		vec3 edge = p2 - p1;
		vec3 baseToVertex = p1 - basePoint;
		float edgeSquaredLength = edge.squareLength();
		float edgeDotVelocity = edge * (evel);
		float edgeDotBaseToVertex = edge * (baseToVertex);
		// Calculate parameters for equation
		a = edgeSquaredLength * -velocitySquaredLength +
		edgeDotVelocity*edgeDotVelocity;
		b = edgeSquaredLength*(2 * (evel * baseToVertex)) -
		2.0*edgeDotVelocity*edgeDotBaseToVertex;
		c = edgeSquaredLength*(1 - baseToVertex.squareLength()) +
		edgeDotBaseToVertex*edgeDotBaseToVertex;
		// Does the swept sphere collide against infinite edge?
		if(math::getLowestRoot(a, b, c, t, &newT)) {
		// Check if intersection is within line segment:
			float f = (edgeDotVelocity*newT - edgeDotBaseToVertex) /
			edgeSquaredLength;
			if (f >= 0.0 && f <= 1.0) {
			// intersection took place within segment.
				t = newT;
				foundCollison = true;
				collisionPoint = p1 + edge * f;
				
				//Calculate edge normal
				norm = -(vec3(invespace * vec4(collisionPoint, 1)) - (this->position + this->velocity * t)).normalize();
			}
		}
		// p2 -> p3:
		edge = p3 - p2;
		baseToVertex = p2 - basePoint;
		edgeSquaredLength = edge.squareLength();
		edgeDotVelocity = edge * (evel);
		edgeDotBaseToVertex = edge * (baseToVertex);
		a = edgeSquaredLength * -velocitySquaredLength +
		edgeDotVelocity*edgeDotVelocity;
		b = edgeSquaredLength*(2*(evel *baseToVertex)) -
		2.0*edgeDotVelocity*edgeDotBaseToVertex;
		c = edgeSquaredLength*(1 - baseToVertex.squareLength()) +
		edgeDotBaseToVertex*edgeDotBaseToVertex;
		if (math::getLowestRoot(a, b, c, t, &newT)) {
			float f = (edgeDotVelocity*newT - edgeDotBaseToVertex) /
			edgeSquaredLength;
			if (f >= 0.0 && f <= 1.0) {
				t = newT;
				foundCollison = true;
				collisionPoint = p2 + edge * f;
			
				//Calculate edge normal
				norm = -(vec3(invespace * vec4(collisionPoint, 1)) - (this->position + this->velocity * t)).normalize();
			}
		}
		// p3 -> p1:
		edge = p1 - p3;
		baseToVertex = p3 - basePoint;
		edgeSquaredLength = edge.squareLength();
		edgeDotVelocity = edge * (evel);
		edgeDotBaseToVertex = edge * (baseToVertex);
		a = edgeSquaredLength * -velocitySquaredLength +
		edgeDotVelocity*edgeDotVelocity;
		b = edgeSquaredLength*(2*(evel * baseToVertex)) -
		2.0*edgeDotVelocity*edgeDotBaseToVertex;
		c = edgeSquaredLength*(1 - baseToVertex.squareLength()) +
		edgeDotBaseToVertex*edgeDotBaseToVertex;
		if (math::getLowestRoot(a, b, c, t, &newT)) {
			float f = (edgeDotVelocity*newT - edgeDotBaseToVertex) /
			edgeSquaredLength;
			if (f >= 0.0 && f <= 1.0) {
				t = newT;
				foundCollison = true;
				collisionPoint = p3 + edge * f;
			
				//Calculate edge normal
				norm = -(vec3(invespace * vec4(collisionPoint, 1)) - (this->position + this->velocity * t)).normalize();
			}
		}
	}
	if (foundCollison) {
		float distToCollision = t * evel.length();
		
		if (!cp.hasCollision || distToCollision < cp.nearest) {
			cp.nearest = distToCollision;
			mat4 invEspace = espace;
			invEspace.inverse();
			cp.intersectionPoint = invEspace * vec4(collisionPoint, 1);
			cp.hasCollision = true;
			cp.norm = norm;
		}
	}
}

void GameObject::collision(CChunk *chunk, GameObject *obj, AABB bounding, u32 &cn, CPackage &cp, vec3 velocity) {
	if (chunk == nullptr || obj == nullptr || obj->isHidden() || !bounding.intersect(chunk->getAABB() * obj->getModel()))return;
	double near = !cp.hasCollision ? f32_MAX : cp.nearest;
	std::vector<Triangle> tr = chunk->getTriangles();
	for (u32 i = 0; i < tr.size(); i++)
		collision(tr[i] * obj->getModel(), cp, velocity);
	cn += tr.size();
	u32 count = 0;
	CChunk **c = chunk->getChunks(count);
	if (c != nullptr) {
		for (u32 i = 0; i < count; i++)
			collision(c[i], obj, bounding, cn, cp, velocity);
	}
	
	if (near > cp.nearest)cp.closest = obj;
}

bool GameObject::doCollision(double delta){
	vec3 d = velocity * delta;
	
	AABB bounding = AABB::encapsulate(getAABB(), getAABB() + d);
	
	u32 triCount = 0;
	
	CPackage p = CPackage();
		
	for (u32 i = 0; i < w->getObjects(); i++) {
		GameObject *ptr = w->at(i);
		CChunk *chunk = nullptr;
		if (!ptr->isStatic || !ptr->getAABB().intersect(bounding) || !Defines::instanceof<Mesh>(ptr->ro))continue;
		if ((chunk = ((Mesh*)ptr->ro)->getChunk()) == nullptr)continue;
		collision(chunk, ptr, bounding, triCount, p, d);
	}
	
	if (p.hasCollision && usesCollision()) {
		float mokerPittigKleineDelta = 0.05;
		vec3 correction = -d.normalize() * mokerPittigKleineDelta;
		position = position + d * p.t + correction;
		
		vec3 n = p.norm.normalize();
		velocity = velocity - (n * (velocity * n));
		velocity = velocity * (1 - getFriction()) * (1 - p.closest->getFriction());
	}
	else
		position += d;
	
	return p.hasCollision;
}
bool GameObject::isStuck(){
	bool old = ignoresCollision;
	ignoresCollision = true;
	bool collides = doCollision(0);
	ignoresCollision = old;
	return collides;
}
//End of the C-word

GameObject::~GameObject(){
	for (u32 i = 0; i < behaviors.size(); i++)
		delete behaviors[i];
}

void GameObject::applyBehavior(Behavior *b){
	if (b == nullptr || hasBehavior(b->getName())) {
		if (b != nullptr)delete b;
		return;
	}
	behaviors.push_back(b);
	b->init(this);
}
bool GameObject::hasBehavior(std::string name){
	for (u32 i = 0; i < behaviors.size(); i++)
		if (behaviors[i]->getName() == getName())return true;
	return false;
}
Behavior *GameObject::getBehavior(std::string name){
	for (u32 i = 0; i < behaviors.size(); i++)
		if (behaviors[i]->getName() == name)return behaviors[i];
	return nullptr;
}

void GameObject::update(double delta, InputHandler *ih){
	for (u32 i = 0; i < behaviors.size(); i++){
		behaviors[i]->onUpdate(this, delta);
		behaviors[i]->onUpdate(this, delta, ih);
	}
	
	if (!defiesGravity) {
		velocity -= vec3(0, 9.81, 0) * delta;
		setAnimationState("Falling");
	}
	if (!isStatic) {
		if(!ignoresCollision){
			u32 maxR = 0;
			while (doCollision(delta) && maxR < 65536) {
				maxR++;
			}
		}
	}
	else velocity = vec3();
	
	if(!animationSwitched)animationTime += delta;
	animationSwitched = false;
}
vec3 GameObject::getForward(){
	vec3 fwd = mat4::rotateDeg(rotation) * vec4(0, 0, -1, 0);
	return fwd.normalize();
}
void GameObject::nobindRender(Shader *s, Camera *c, std::vector<Light> lights){
	s->set("uvScale", uvScale);
	if (Defines::instanceof<MD2Model>(ro))
		((MD2Model*)ro)->setTemporary(fs, animationTime);
	ro->render(s, getModel(), c->getView(), c->getProjection(), c->getPosition(), lights, c->toFrustum());
}
void GameObject::render(Shader *s, Camera *c, std::vector<Light> lights){
	if (t != nullptr)t->bind(GL_TEXTURE_2D);
	nobindRender(s, c, lights);
}
void GameObject::setAnimationState(std::string name){
	if (!Defines::instanceof<MD2Model>(ro))return;
	FrameState *temp = ((MD2Model*)ro)->getAnimationState(name);
	if(temp != fs && temp != nullptr){
		animationTime = 0;
		animationSwitched = true;
		fs = temp;
	}
}