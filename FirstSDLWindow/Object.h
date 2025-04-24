#pragma once

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
	vector operator* (const float& in) {
		return vector(x * in, y * in);
	}
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

	Object(float x, float y, size_t ident);
	Object(float x, float y, float radius, size_t ident);
};
