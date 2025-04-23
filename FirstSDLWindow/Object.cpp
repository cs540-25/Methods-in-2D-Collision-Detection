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
	type = FOOTMAN;
	id = ident;
}