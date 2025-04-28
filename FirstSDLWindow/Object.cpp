#include "Object.h"
#include <cstdlib>

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
}
