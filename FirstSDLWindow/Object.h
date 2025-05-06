#pragma once
#include <SDL_rect.h>

struct Color {
	unsigned char r, g, b, a;	// RGB and alpha for opacity
};

struct vector {		// 2D collection of X and Y values
	float x = 0;
	float y = 0;
	vector(float x = 0, float y = 0) {
		this->x = x;
		this->y = y;
	}
	vector& operator= (const vector& in) {
		x = in.x;
		y = in.y;
		return *this;
	}
	vector operator+ (const vector& in) {
		return vector(in.x + x, in.y + y);
	}
	vector operator- (const vector& in) {
		return vector(x - in.x, y - in.y);
	}
	vector operator* (const float& in) {
		return vector(x * in, y * in);
	}
	float dot(const vector& in) {	// Dot product
		return x * in.x + y * in.y;
	}
};

struct AxisAlignedBoundingBox {
	vector* center;	// Pointer so that we can keep track of the rapidly changing positions
	float radi[2];	// 1/2 of width and length
	SDL_Rect toSDLRect();
};


class Object {	// I'm making all of this public for ease
public:
	vector pos;
	vector vel;
	vector acc;
	bool isVisible;
	bool isStatic;
	Color color;	// Automatically set to white
	size_t id;
	int mass;		// This is probably always going to be 1, but we can change this for fun

	bool isCircle;
	float radius;

	// Colliders
	AxisAlignedBoundingBox* AABB;

	int createAABB();	// Returns 1 on a successful creation, 0 on failure
	int destroyAABB();	// Returns 1 on a successful deletion
	Object(float x, float y, size_t ident);
	Object(float x, float y, float radius, size_t ident);
};
