#include "Object.h"
#include <cstdlib>

int Object::createAABB() {	
	AABB = new AxisAlignedBoundingBox;
	if (isCircle) {
		AABB->center = pos;
		AABB->radi[0] = radius;
		AABB->radi[1] = radius;
	}
	else {	// Currently will only work for circles
		return 0;
	}
	return 1;
}

int Object::destroyAABB() {
	delete AABB;
	AABB = NULL;
	return 0;
}

Object::Object(float x, float y, size_t ident) {
	pos.x = x;
	pos.y = y;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;
	isVisible = true;
	isStatic = false;
	isCircle = false;
	radius = 0.5;
	id = ident;
	mass = 1;

	// Colliders
	AABB = NULL;
}

Object::Object(float x, float y, float radius, size_t ident) {
	pos.x = x;
	pos.y = y;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;
	isVisible = true;
	isStatic = false;
	isCircle = true;
	this->radius = radius;
	id = ident;	
	mass = 1;

	// Colliders
	AABB = NULL;
}
