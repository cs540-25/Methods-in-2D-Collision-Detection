#include "Object.h"
#include <cstdlib>

Object::Object(float x, float y, float ident) {
	pos.x = x;
	pos.y = y;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;
	isVisible = true;
	isStatic = false;
	type = FOOTMAN;
	if (ident == 0) {
		id = rand();
	}
	else {
		id = ident;
	}
}