#include "Object.h"
#include <cstdlib>

int Object::createAABB() {	
	AABB = new AxisAlignedBoundingBox;
	if (isCircle) {
		AABB->center = &pos;
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
	isVisible = true;
	isStatic = false;
	isCircle = false;
	radius = 0.5;
	id = ident;
	mass = 1;
	isColliding = false;
	isOverlapping = false;

	// Colliders
	AABB = NULL;
}

Object::Object(float x, float y, float radius, size_t ident) {
	pos.x = x;
	pos.y = y;
	color = Color(255, 255, 255, 255);
	isVisible = true;
	isStatic = false;
	isCircle = true;
	this->radius = radius;
	id = ident;	
	mass = 1;
	isColliding = false;
	isOverlapping = false;

	// Colliders
	AABB = NULL;
}

SDL_Rect AxisAlignedBoundingBox::toSDLRect()
{
	SDL_Rect ret;
	ret.x = center->x - radi[0];	// These are supposed to be on the top left of the rectangle
	ret.y = center->y - radi[1];
	ret.w = radi[0] * 2;
	ret.h = radi[1] * 2;
	return ret;
}

vector AxisAlignedBoundingBox::min()
{
	return vector(center->x - radi[0], center->y - radi[1]);
}

vector AxisAlignedBoundingBox::max()
{
	return vector(center->x + radi[0], center->y + radi[1]);
}

Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

Color& Color::operator=(const Color& in)
{
	r = in.r;
	g = in.g;
	b = in.b;
	a = in.a;
	return (*this);
}
